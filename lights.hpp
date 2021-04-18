#ifndef __LIGHTS_HPP__
#define __LIGHTS_HPP__

#include "figures/geometry.hpp"

struct Light
{
    Vec3f position;
    float intensity;
    Light(const Vec3f &p, const float &i) : position(p), intensity(i) {}
};

#endif
