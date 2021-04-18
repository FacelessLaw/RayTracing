#ifndef __TRIANGLE_HPP__
#define __TRIANGLE_HPP__

#include "geometry.hpp"

Vec3f crossVec(const Vec3f& a, const Vec3f& b) {
  return Vec3f(
      a[1] * b[2] - a[2] * b[1],
      a[2] * b[0] - a[0] * b[2],
      a[0] * b[1] - a[1] * b[0]
  );
}

// ------ Алгоритм Моллера — Трумбора -------
float triangle_intersection(
    const Vec3f &start_ray,
    const Vec3f &ray,
    const Vec3f &node_0,
    const Vec3f &node_1,
    const Vec3f &node_2
) {
    Vec3f ray_from_0_to_1 = node_1 - node_0;
    Vec3f ray_from_0_to_2 = node_2 - node_0;

    Vec3f pvec = crossVec(ray, ray_from_0_to_2);
    float det = ray_from_0_to_1 * pvec;

    if (abs(det) < 1e-4) {
        return 0;
    }

    float inv_det = 1 / det;
    Vec3f tvec = start_ray - node_0;
    float u = (tvec * pvec) * inv_det;
    if (u < 0 || u > 1) {
        return 0;
    }

    Vec3f qvec = crossVec(tvec, ray_from_0_to_1);
    float v = ray * qvec * inv_det;
    if (v < 0 || u + v > 1) {
        return 0;
    }
    return ((ray_from_0_to_2 * qvec) * inv_det);
}

#endif