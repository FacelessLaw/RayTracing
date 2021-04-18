#ifndef __SQUARE_HPP__
#define __SQUARE_HPP__

#include "geometry.hpp"
#include "materials.hpp"

struct Square
{
    Vec3f center;
    float side;
    Material material;

    Square(
        const Vec3f &c,
        const float &s,
        const Material &m
    ): center(c), side(s), material(m) {}

    bool ray_intersect(
        const Vec3f &orig,
        const Vec3f &dir,
        float &t0
    ) const {
        float d = -(orig.z - center.z) / dir.z; // the checkerboard plane has equation y = level
        Vec3f pt = orig + dir * d;
        if (
            d > 0 &&
            fabs(pt.x - center.x) < side / 2 && 
            fabs(pt.y - center.y) < side / 2
        ) {
            t0 = d;
            return true;
        }
    
        return false;
    }
};

#endif
