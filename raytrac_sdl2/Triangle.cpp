#include "Triangle.h"
#include "Ray.h"
#include "AABB.h"
#include <cmath> // std::abs

// Varsayýlan oluþturucu
Triangle::Triangle()
    : v0{ 0.0f, 0.0f, 0.0f }, v1{ 0.0f, 0.0f, 0.0f }, v2{ 0.0f, 0.0f, 0.0f }, material(nullptr) {}

Triangle::Triangle(const Vec3& a, const Vec3& b, const Vec3& c, std::shared_ptr<Material> m)
    : v0(a), v1(b), v2(c), material(m) {
    // Üçgenin sýnýrlayýcý kutusunu hesapla
    min_point = Vec3(
        std::min({ v0.x, v1.x, v2.x }),
        std::min({ v0.y, v1.y, v2.y }),
        std::min({ v0.z, v1.z, v2.z })
    );
    max_point = Vec3(
        std::max({ v0.x, v1.x, v2.x }),
        std::max({ v0.y, v1.y, v2.y }),
        std::max({ v0.z, v1.z, v2.z })
    );
}

void Triangle::set_transform(const Matrix4x4& t) {
    transform = t;
}

bool Triangle::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    // Üçgenin köþe noktalarýný dönüþtür
    const Vec3 transformed_v0 = transform.transform_point(v0);
    const Vec3 transformed_v1 = transform.transform_point(v1);
    const Vec3 transformed_v2 = transform.transform_point(v2);

    // Kenar vektörlerini ve normal vektörü önceden hesapla
    const Vec3 edge1 = transformed_v1 - transformed_v0;
    const Vec3 edge2 = transformed_v2 - transformed_v0;
    const Vec3 normal = Vec3::cross(edge1, edge2).normalize();

    // Möller–Trumbore algoritmasý
    const Vec3 h = Vec3::cross(r.direction, edge2);
    const double a = Vec3::dot(edge1, h);

    if (std::abs(a) < EPSILON) return false;

    const double f = 1.0 / a;
    const Vec3 s = r.origin - transformed_v0;
    const double u = f * Vec3::dot(s, h);

    if (u < 0.0 || u > 1.0) return false;

    const Vec3 q = Vec3::cross(s, edge1);
    const double v = f * Vec3::dot(r.direction, q);

    if (v < 0.0 || u + v > 1.0) return false;

    const double t = f * Vec3::dot(edge2, q);

    if (t > t_min && t < t_max) {
        rec.t = t;
        rec.point = r.at(t);
        rec.set_face_normal(r, transform.transform_vector(normal).normalize());
        rec.material = material;
        return true;
    }

    return false;
}

bool Triangle::bounding_box(double time0, double time1, AABB& output_box) const {
    // Dönüþtürülmüþ köþe noktalarýný kullanarak sýnýrlayýcý kutuyu hesapla
    Vec3 transformed_v0 = transform.transform_point(v0);
    Vec3 transformed_v1 = transform.transform_point(v1);
    Vec3 transformed_v2 = transform.transform_point(v2);

    Vec3 min(
        std::min({ transformed_v0.x, transformed_v1.x, transformed_v2.x }) - EPSILON,
        std::min({ transformed_v0.y, transformed_v1.y, transformed_v2.y }) - EPSILON,
        std::min({ transformed_v0.z, transformed_v1.z, transformed_v2.z }) - EPSILON
    );
    Vec3 max(
        std::max({ transformed_v0.x, transformed_v1.x, transformed_v2.x }) + EPSILON,
        std::max({ transformed_v0.y, transformed_v1.y, transformed_v2.y }) + EPSILON,
        std::max({ transformed_v0.z, transformed_v1.z, transformed_v2.z }) + EPSILON
    );

    output_box = AABB(min, max);
    return true;
}
