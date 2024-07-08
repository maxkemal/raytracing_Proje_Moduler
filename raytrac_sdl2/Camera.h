#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include "Vec3.h"
#include "Ray.h"
#include "AABB.h"

class Camera {
private:
    struct Plane {
        Vec3 normal;
        double distance;

        Plane() : normal(Vec3()), distance(0) {}
        Plane(const Vec3& n, const Vec3& point) : normal(n.normalize()) {
            distance = -Vec3::dot(normal, point);
        }

        double distanceToPoint(const Vec3& point) const {
            return Vec3::dot(normal, point) + distance;
        }
    };

public:
    Camera(Vec3 lookfrom, Vec3 lookat, Vec3 vup, double vfov, double aspect, double aperture, double focus_dist);

    Ray get_ray(double s, double t) const;

    bool isPointInFrustum(const Vec3& point, double size) const;
    bool isAABBInFrustum(const AABB& aabb) const;
    std::vector<AABB> performFrustumCulling(const std::vector<AABB>& objects) const;

private:
    void updateFrustumPlanes();

    Vec3 origin;
    Vec3 lower_left_corner;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 u, v, w;
    double lens_radius;

    // Frustum culling için ek alanlar
    double near_dist;
    double far_dist;
    double fov;
    double aspect_ratio;
    Plane frustum_planes[6];
};

#endif // CAMERA_H
