#ifndef __PYRAMID_HPP__
#define __PYRAMID_HPP__

#include "geometry.hpp"
#include "materials.hpp"
#include "triangle.hpp"

struct Pyramid
{
    Vec3f points[4];
    std::vector <std::pair<int, std::pair<int, int>>> numbers;
    Material material;

    Pyramid(
        const Vec3f &_a,
        const Vec3f &_b,
        const Vec3f &_c,
        const Vec3f &_d,
        const Material &_m
    ): material(_m)
    {
        points[0] = _a;
        points[1] = _b;
        points[2] = _c;
        points[3] = _d;
        numbers = {
            {0, {1, 2}},
            {0, {1, 3}},
            {0, {2, 3}},
            {1, {2, 3}}
        };
    }

    bool ray_intersect(
        const Vec3f &start_ray,
        const Vec3f &ray,
        Vec3f &crossPoint,
        Vec3f &vecCrossPointToCenter,
        float &nearestDistanceToCrossPoint
    ) const {
        bool is_intersect = false;

        for (auto& it : numbers) {
            float dist = triangle_intersection(
                start_ray, ray, points[it.first],
                points[it.second.first], points[it.second.second]);
            if (dist > 1e-3 && dist < nearestDistanceToCrossPoint) {
                crossPoint = start_ray + ray * dist;
                nearestDistanceToCrossPoint = dist;
                vecCrossPointToCenter = crossVec(
                    points[it.second.second] - points[it.first],
                    points[it.second.first] - points[it.first]
                ).normalize();

                is_intersect = true;
            }
        }

        if (is_intersect)
            return true;
        return false;
    }
};

#endif
