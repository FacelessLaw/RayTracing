#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <unordered_map>
#include <algorithm>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "figures/cylinder.hpp"
#include "figures/disc.hpp"
#include "figures/geometry.hpp"
#include "figures/pyramid.hpp"
#include "figures/sphere.hpp"
#include "figures/square.hpp"
#include "images.hpp"
#include "lights.hpp"

size_t WIDTH = 512;
size_t HEIGHT = 512;
const int FOV = M_PI / 2.;
Picture background_image;
Picture wave_image;

Vec3f reflect(const Vec3f &I, const Vec3f &N) {
    return I - N * 2.f * (I * N);
}

Vec3f refract(const Vec3f &I, const Vec3f &N, const float &refractive_index) {
    float cosi = - std::max(-1.f, std::min(1.f, I*N));
    float etai = 1, etat = refractive_index;
    Vec3f n = N;
    if (cosi < 0) {
        cosi = -cosi;
        std::swap(etai, etat); n = -N;
    }
    float eta = etai / etat;
    float k = 1 - eta*eta*(1 - cosi * cosi);
    return k < 0 ? Vec3f(0,0,0) : I*eta + n*(eta * cosi - sqrtf(k));
}


bool scene_intersect(
    const Vec3f &orig,
    const Vec3f &dir,
    const std::vector<Sphere> &spheres,
    const std::vector<Pyramid> &pyramids,
    const std::vector<Cylinder> &cylinders,
    const std::vector<Disc> &discs,
    const std::vector<Sphere> &water,
    Vec3f &hit,
    Vec3f &N,
    Material &material
) {
    float nearest_dist = std::numeric_limits<float>::max();
    for (size_t i = 0; i < spheres.size(); ++i) {
        float dist_i;
        if (spheres[i].ray_intersect(orig, dir, dist_i) && dist_i < nearest_dist) {
            nearest_dist = dist_i;
            hit = orig + dir * dist_i;
            N = (hit - spheres[i].center).normalize();
            material = spheres[i].material;
        }
    }

    for (size_t i = 0; i < pyramids.size(); ++i) {
        if (pyramids[i].ray_intersect(orig, dir, hit, N, nearest_dist)) {
            material = pyramids[i].material;
        }
    }

    for (size_t i = 0; i < cylinders.size(); ++i) {
        float dist_i;
        if (cylinders[i].ray_intersect(orig, dir, dist_i) && dist_i < nearest_dist) {
            nearest_dist = dist_i;
            hit = orig + dir * dist_i;
            N = Vec3f(hit.x, 0, hit.z).normalize();
            material = cylinders[i].material;
        }
    }

    for (size_t i = 0; i < discs.size(); ++i) {
        float dist_i;
        if (discs[i].ray_intersect(orig, dir, dist_i) && dist_i < nearest_dist) {
            nearest_dist = dist_i;
            hit = orig + dir * dist_i;
            N = Vec3f(0,1,0);
            material = discs[i].material;
        }
    }

    for (size_t i = 0; i < water.size(); ++i) {
        float dist_i;
        if (water[i].ray_intersect(orig, dir, dist_i) && dist_i < nearest_dist) {
            wave_image.get_pixel_from_ray(dir, material);
            nearest_dist = dist_i;
            hit = orig + dir * dist_i;
            N = Vec3f(0,0,1);
            Vec3f curr_color = material.diffuse_color;
            material = water[i].material;
            material.diffuse_color = curr_color;
        }
    }

    return nearest_dist<1000;
}

Vec3f cast_ray(
    const Vec3f &orig,
    const Vec3f &dir,
    const std::vector<Sphere> &spheres,
    const std::vector<Pyramid> &pyramids,
    const std::vector<Cylinder> &cylinders,
    const std::vector<Disc> &discs,
    const std::vector<Sphere> &water,
    const std::vector<Light> &lights,
    size_t depth = 0
    ) {
    Vec3f point, N;
    Material material;

    if (depth > 6 || 
        !scene_intersect(
            orig, dir, spheres, pyramids, cylinders, discs, water, point, N, material
        )
    ) {
        background_image.get_pixel_from_ray(dir, material);
        return material.diffuse_color; // background color
    }
    
    Vec3f reflect_dir = reflect(dir, N).normalize();
    Vec3f refract_dir = refract(dir, N, material.refractive_index).normalize();
    Vec3f reflect_orig = reflect_dir * N < 0 ? point - N * 1e-3 : point + N * 1e-3;
    Vec3f refract_orig = refract_dir * N < 0 ? point - N * 1e-3 : point + N * 1e-3;
    Vec3f reflect_color = cast_ray(
        reflect_orig, reflect_dir, spheres, pyramids, cylinders, discs, water, lights, depth + 1
    );
    Vec3f refract_color = cast_ray(
        refract_orig, refract_dir, spheres, pyramids, cylinders, discs, water, lights, depth + 1
    );

    float diffuse_light_intensity = 0, specular_light_intensity = 0;
    for (size_t i = 0; i < lights.size(); ++i) {
        Vec3f light_dir = (lights[i].position - point).normalize();
        float light_distance = (lights[i].position - point).norm();

        Vec3f shadow_orig = light_dir * N < 0 ? point - N * 1e-3 : point + N * 1e-3;
        Vec3f shadow_pt, shadow_N;
        Material tmpmaterial;
        if (
            scene_intersect(
                shadow_orig,
                light_dir,
                spheres,
                pyramids,
                cylinders,
                discs,
                water,
                shadow_pt,
                shadow_N,
                tmpmaterial
            ) && (shadow_pt-shadow_orig).norm() < light_distance)
            continue;

        diffuse_light_intensity += lights[i].intensity * std::max(0.f, light_dir * N);
        specular_light_intensity += 
           powf(
               std::max(
                   0.f,
                   -reflect(-light_dir, N) * dir
                ),
                material.specular_exponent
            ) * lights[i].intensity;
    }
    return 
        material.diffuse_color * diffuse_light_intensity * material.albedo[0] +
        Vec3f(1., 1., 1.) * specular_light_intensity * material.albedo[1] +
        reflect_color * material.albedo[2] +
        refract_color * material.albedo[3];
}

void render(
    const std::vector<Sphere> &spheres,
    const std::vector<Pyramid> &pyramids,
    const std::vector<Cylinder> &cylinders,
    const std::vector<Disc> &discs,
    const std::vector<Sphere> &water,
    const std::vector<Light> &lights
) {
    std::vector<Vec3f> framebuffer(WIDTH * HEIGHT);

    #pragma omp parallel for
    for (size_t i = 0; i < HEIGHT; ++i) {
        for (size_t j = 0; j < WIDTH; ++j) {
            float x =  (2 * (j + 0.5) / (float)WIDTH  - 1) * tan(FOV / 2.) * WIDTH / (float)HEIGHT;
            float y = -(2 * (i + 0.5) / (float)HEIGHT - 1) * tan(FOV / 2.);
            Vec3f dir = Vec3f(x, y, -1).normalize();
            
            Vec3f ans = cast_ray(
                Vec3f(0,0,0),
                dir,
                spheres,
                pyramids,
                cylinders,
                discs,
                water,
                lights
            );
            for (int z = -1; z < 2; ++z) {
                for (int s = -1; s < 2; ++s) {
                    if (j > 0 && i > 0 && i < HEIGHT - 1 && j < WIDTH - 1) {
                        if (s == 0 || z == 0) {
                        framebuffer[j + z + (i + s) * WIDTH] =
                            framebuffer[j + z + (i + s) * WIDTH] + ans * 0.3;
                        } else {
                        framebuffer[j + z + (i + s) * WIDTH] =
                            framebuffer[j + z + (i + s) * WIDTH] + ans * 0.05;
                        }
                    }
                }
            }
        }
    }

    std::vector<unsigned char> pixmap(WIDTH * HEIGHT * 3);
    for (size_t i = 0; i < HEIGHT * WIDTH; ++i) {
        Vec3f &c = framebuffer[i];
        float max = std::max(c[0], std::max(c[1], c[2]));
        if (max > 1) c = c * (1. / max);
        for (size_t j = 0; j < 3; ++j) {
            pixmap[i * 3 + j] = (unsigned char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }

    stbi_write_jpg("312_Volkov_v2v6.jpg", WIDTH, HEIGHT, 3, pixmap.data(), 100);
}

int main(int argc, const char** argv)
{
    std::unordered_map<std::string, std::string> cmdLineParams;
    for(int i = 0; i < argc; i++)
    {
        std::string key(argv[i]);

        if(key.size() > 0 && key[0] == '-')
        {
        if(i != argc - 1) // not last argument
        {
            cmdLineParams[key] = argv[i+1];
            i++;
        }
        else
            cmdLineParams[key] = "";
        }
    }

    if(cmdLineParams.find("-w") != cmdLineParams.end()) {
        WIDTH = HEIGHT = atoi(cmdLineParams["-w"].c_str());
    }


    Material      ivory(1.0, Vec4f(0.6,  0.3, 0.1, 0.0), Vec3f(0.4, 0.4, 0.3),   50.);
    Material      ebony(1.0, Vec4f(0.6,  0.3, 0.1, 0.0), Vec3f(0.1, 0.1, 0.1),   50.);
    Material      glass(1.5, Vec4f(0.0,  0.5, 0.1, 0.9), Vec3f(0.5, 0.6, 0.6),  125.);
    Material red_rubber(1.0, Vec4f(0.9,  0.1, 0.0, 0.0), Vec3f(0.3, 0.1, 0.1),   10.);
    Material     mirror(1.0, Vec4f(0.0, 10.0, 0.8, 0.0), Vec3f(1.0, 1.0, 1.0), 1425.);
    Material        h2o(1.015, Vec4f(0.3,  0.3, 0.1, 0.78), Vec3f(0.4, 0.4, 0.5),   50.);

    background_image = Picture("../src/dark.jpg");

    std::vector<Sphere> spheres;
    /*spheres.push_back(Sphere(Vec3f(-3,    0,   -16), 1,      ivory));
    spheres.push_back(Sphere(Vec3f(-1.0, -1.5, -12), 2,      glass));*/
    spheres.push_back(Sphere(Vec3f( 1.5, -0.5, -18), 2, red_rubber));
    spheres.push_back(Sphere(Vec3f( 7,    5,   -18), 4,     mirror));

    std::vector<Pyramid> pyramids;
    pyramids.push_back(
        Pyramid(
            Vec3f( -2.5,  0, -8),
            Vec3f(  0.5, -2, -6),
            Vec3f(  1.5,  1, -8),
            Vec3f( -1.5, -2, -10),
            glass
        )
    );

    std::vector<Cylinder> cylinders;
    cylinders.push_back(
        Cylinder(
            Vec3f( 0, 6, -8),
            2,
            -3,
            ebony
        )
    );

    std::vector<Disc> discs;
    discs.push_back(
        Disc(
            Vec3f(0, -3, -8),
            2,
            ebony
        )
    );


    wave_image = Picture("../src/water_extra_big.jpg");
    std::vector<Sphere> water;
    water.push_back(
        Sphere(Vec3f( -0.25, -0.5, -8), 0.5, h2o)
    );

    std::vector<Light> lights;
    lights.push_back(Light(Vec3f(-20, 20,  20), 1.5));
    lights.push_back(Light(Vec3f( 30, 50, -25), 1.8));
    lights.push_back(Light(Vec3f( 30, 20,  30), 1.7));
    lights.push_back(Light(Vec3f( 0, 30,  -14), 1.7));

    render(spheres, pyramids, cylinders, discs, water, lights);

    return 0;
}
