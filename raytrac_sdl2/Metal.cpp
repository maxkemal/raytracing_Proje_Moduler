#include "Metal.h"
#include "Ray.h"


Metal::Metal(const Vec3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

MaterialType Metal::type() const {
    return MaterialType::Metal;
}

Vec3 Metal::emitted(double u, double v, const Vec3& p) const {
    return Vec3(0, 0, 0);
}

bool Metal::scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
    Vec3 reflected = Vec3::reflect(r_in.direction.normalize(), rec.normal);
    scattered = Ray(rec.point, reflected + fuzz * Vec3::random_in_unit_sphere());
    attenuation = albedo;
    return (Vec3::dot(scattered.direction, rec.normal) > 0);
}
