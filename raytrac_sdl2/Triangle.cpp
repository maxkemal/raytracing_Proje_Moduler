#include "Triangle.h"

#include "Ray.h"
#include "AABB.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include "ObjLoader.h"
#include "globals.h"
#include "Lambertian.h"

Triangle::Triangle()
    : smoothGroup(0) {}

Triangle::Triangle(const Vec3& a, const Vec3& b, const Vec3& c, std::shared_ptr<Material> m)
    : v0(a), v1(b), v2(c), material(m), smoothGroup(0) {}

Triangle::Triangle(const Vec3& a, const Vec3& b, const Vec3& c,
    const Vec3& na, const Vec3& nb, const Vec3& nc,
    const Vec2& ta, const Vec2& tb, const Vec2& tc,
    std::shared_ptr<Material> m, int sg)
    : v0(a), v1(b), v2(c),
    n0(na), n1(nb), n2(nc),
    t0(ta), t1(tb), t2(tc),
    material(m), smoothGroup(sg) {
    update_bounding_box();
}


void Triangle::set_normals(const Vec3& normal0, const Vec3& normal1, const Vec3& normal2) {
    n0 = normal0.normalize();
    n1 = normal1.normalize();
    n2 = normal2.normalize();
}

void Triangle::set_transform(const Matrix4x4& t) {
    transform = t;

    update_bounding_box();
}

void Triangle::update_bounding_box() {
    Vec3 transformed_v0 = transform.transform_point(v0);
    Vec3 transformed_v1 = transform.transform_point(v1);
    Vec3 transformed_v2 = transform.transform_point(v2);

    min_point = Vec3(
        std::min({ transformed_v0.x, transformed_v1.x, transformed_v2.x }),
        std::min({ transformed_v0.y, transformed_v1.y, transformed_v2.y }),
        std::min({ transformed_v0.z, transformed_v1.z, transformed_v2.z })
    );
    max_point = Vec3(
        std::max({ transformed_v0.x, transformed_v1.x, transformed_v2.x }),
        std::max({ transformed_v0.y, transformed_v1.y, transformed_v2.y }),
        std::max({ transformed_v0.z, transformed_v1.z, transformed_v2.z })
    );
}
float safeAcos(float x) {
    if (x < -1.0f) x = -1.0f;
    else if (x > 1.0f) x = 1.0f;
    return std::acos(x);
}
bool Triangle::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    // Transform the vertices
    Vec3 transformed_v0 = transform.transform_point(v0);
    Vec3 transformed_v1 = transform.transform_point(v1);
    Vec3 transformed_v2 = transform.transform_point(v2);

    Vec3 edge1 = transformed_v1 - transformed_v0;
    Vec3 edge2 = transformed_v2 - transformed_v0;
    Vec3 h = Vec3::cross(r.direction, edge2);
    double a = Vec3::dot(edge1, h);
   
    if (std::abs(a) < EPSILON)
        return false;

    double f = 1.0 / a;
    Vec3 s = r.origin - transformed_v0;
    double u = f * Vec3::dot(s, h);

    if (u < 0.0 || u > 1.0)
        return false;

    Vec3 q = Vec3::cross(s, edge1);
    double v = f * Vec3::dot(r.direction, q);

    if (v < 0.0 || u + v > 1.0)
        return false;

    double t = f * Vec3::dot(edge2, q);

    if (t < t_min || t > t_max)
        return false;

    rec.t = t;
    rec.point = r.at(t);
    rec.u = u;
    rec.v = v;

    const double w = 1.0 - u - v;

    // Transform and normalize the normals
    Vec3 transformed_n0 = transform.transform_vector(n0).normalize();
    Vec3 transformed_n1 = transform.transform_vector(n1).normalize();
    Vec3 transformed_n2 = transform.transform_vector(n2).normalize();

    rec.interpolated_normal = (w * transformed_n0 + u * transformed_n1 + v * transformed_n2).normalize();
    rec.face_normal = Vec3::cross(edge1, edge2).normalize();

    // Set smoothGroup
    rec.smoothGroup = smoothGroup;

    // Calculate normal based on smoothGroup
    if (smoothGroup > 0) {
        float angle = safeAcos(Vec3::dot(rec.interpolated_normal, rec.face_normal));
        float angle_threshold = 60.0f * M_PI / 180.0f;
        rec.normal = (angle <= angle_threshold) ? rec.interpolated_normal : rec.face_normal;
    }
    else if (smoothGroup == 0) {
        rec.normal = rec.interpolated_normal;
    }
    else {
        rec.normal = rec.face_normal;
    }

    rec.set_face_normal(r, rec.normal);

    // Interpolate UV coordinates
    rec.uv = w * t0 + u * t1 + v * t2;
    // Hata ayýklama için UV deðerlerini yazdýrýn
   // std::cout << "Interpolated UV: (" << rec.uv.u << ", " << rec.uv.v << ")" << std::endl;

    // Set material
    rec.material = material;
       
  
    return true;
}


bool Triangle::bounding_box(double time0, double time1, AABB& output_box) const {
    output_box = AABB(min_point - (EPSILON), max_point + (EPSILON));
    return true;
}