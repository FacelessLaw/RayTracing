#ifndef __CYLINDER_HPP__
#define __CYLINDER_HPP__

#include "geometry.hpp"
#include "materials.hpp"

struct Cylinder
{
    Vec3f center;
    float radius;
    float height;
    Material material;

    Cylinder(
        const Vec3f &c,
        const float &r,
        const float &h,
        const Material &m
    ): center(c), radius(r), height(h), material(m) {}

    bool ray_intersect(
        const Vec3f &orig,
        const Vec3f &dir,
        float &t0
    ) const {
        Vec3f med = orig - center;
        float a = dir.x * dir.x + dir.z * dir.z;
        float b = (med.x * dir.x + med.z * dir.z) * 2.;
        float c = med.x * med.x + med.z * med.z - radius * radius;

        float D = b * b - 4 * a * c;

        if (D < 0)
            return false;
        
        t0 = (-b - sqrtf(D)) / (2 * a);
        if (t0 > 1e-6) {
            Vec3f t = orig + dir * t0;
            if (t.y < -100)
                return false;
            if (t.y > height)
                return false;
            return true;
        }
        
        t0 = (-b + sqrtf(D)) / (2 * a);
        if (t0 < 1e-6)
            return false;
        
        Vec3f t = orig + dir * t0;
            if (t.y < -100)
                return false;
            if (t.y > height)
                return false;
            return true;
    }
};

#endif
