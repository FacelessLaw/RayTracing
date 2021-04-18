#ifndef __DISC_HPP__
#define __DISC_HPP__

#include "geometry.hpp"
#include "materials.hpp"

struct Disc
{
    Vec3f center;
    float radius;
    Material material;

    Disc(
        const Vec3f &c,
        const float &r,
        const Material &m
    ): center(c), radius(r), material(m) {}

    bool ray_intersect(
        const Vec3f &orig,
        const Vec3f &dir,
        float &t0
    ) const {
        if (fabs(dir.y) > 1e-3)  {
            float d = -(orig.y - center.y) / dir.y; // the checkerboard plane has equation y = level
            Vec3f pt = orig + dir * d;
            if (
                d > 0 &&
                (pt.x - center.x) * (pt.x - center.x) + 
                (pt.z - center.z) * (pt.z - center.z) <= radius * radius
            ) {
                t0 = d;
                return true;
            }
        }
        return false;
    }
};

#endif
