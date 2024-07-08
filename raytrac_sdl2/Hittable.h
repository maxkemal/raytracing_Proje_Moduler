#ifndef HITTABLE_H
#define HITTABLE_H

#include "Ray.h"
#include "AABB.h"
#include <vector>
#include <memory> // std::shared_ptr kullanýmý için

class Material; // Ýleri bildirim
struct HitRecord {
    Vec3SIMD point;
    Vec3SIMD normal;
    std::shared_ptr<Material>  material;
    double t;
    double u;
    double v;
    bool front_face;

    inline void set_face_normal(const Ray& r, const Vec3& outward_normal) {
        front_face = Vec3::dot(r.direction, outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable {
public:
    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const = 0;
    virtual bool bounding_box(double time0, double time1, AABB& output_box) const = 0;
    virtual ~Hittable() = default;

    std::vector<std::shared_ptr<Hittable>> objects; // objects üyesi eklendi
};

#endif // HITTABLE_H
