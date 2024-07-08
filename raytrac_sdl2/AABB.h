#ifndef AABB_H
#define AABB_H

#include "Vec3.h"
#include "Vec3SIMD.h"
#include "Ray.h"

class AABB {
public:
    Vec3 min, max;

    AABB() {}
    AABB(const Vec3SIMD& a, const Vec3SIMD& b) : min(a), max(b) {}

    bool hit(const Ray& r, double t_min, double t_max) const {
        for (int a = 0; a < 3; a++) {
            auto invD = 1.0f / r.direction[a];
            auto t0 = (min[a] - r.origin[a]) * invD;
            auto t1 = (max[a] - r.origin[a]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }
        return true;
    }

    Vec3 getPositiveVertex(const Vec3& normal) const {
        return Vec3(
            normal.x >= 0 ? max.x : min.x,
            normal.y >= 0 ? max.y : min.y,
            normal.z >= 0 ? max.z : min.z
        );
    }
};

AABB surrounding_box(const AABB& box0, const AABB& box1);

#endif // AABB_H
