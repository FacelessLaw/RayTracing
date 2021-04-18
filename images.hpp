#ifndef __PICTURE_HPP__
#define __PICTURE_HPP__

#include "figures/geometry.hpp"
#include "figures/materials.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Picture {
    Vec3f background;
    std::vector<Vec3f> date;
    int width;
    int height;

    Picture(const char* path_to_picture) {
        int some;
        unsigned char *pixmap = stbi_load(path_to_picture, &width, &height, &some, 0);
        date.resize(width * height);
        #pragma omp parallel for
        for (int j = height - 1; j >= 0 ; j--) {
            for (int i = 0; i < width; i++) {
                date[i + j * width] =
                    Vec3f(
                        pixmap[(i + j * width) * 3],
                        pixmap[(i + j * width) * 3 + 1],
                        pixmap[(i + j * width) * 3 + 2]
                    ) * (1 / float(255));
            }
        }
        stbi_image_free(pixmap);
    }
    Picture(Vec3f background): background(background) {}

    Picture() {}

    Vec3f operator[](const size_t& ind) {
        return date[ind];
    }

    void get_pixel_from_ray(const Vec3f& ray, Material& material) {
        if (date.size() == 0) {
        material.diffuse_color = background;
        } else {
        float percent_width = 0.5 + (atan2(ray[2], ray[0])) / 2 / M_PI;
        float percent_height = 0.5f - asin(ray[1]) / M_PI;

        int width_i = percent_width * width;
        int height_j = percent_height * height;

        width_i = width_i < 0 ? 0 : width_i > width_i - 1 ? width_i - 1 : width_i;
        height_j = height_j < 0 ? 0 : height_j > height_j - 1 ? height_j - 1 : height_j;

        material.diffuse_color = date[width_i + height_j * width];
        }
    }
};

#endif
