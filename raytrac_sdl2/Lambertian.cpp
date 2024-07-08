#include "Lambertian.h"
#include "Ray.h"


Lambertian::Lambertian(const Vec3& a) : albedo(a) {}

Vec3 Lambertian::emitted(double u, double v, const Vec3& p) const {
    return Vec3(0, 0, 0);
}

bool Lambertian::scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
    Vec3 scatter_direction = rec.normal + Vec3::random_unit_vector();
    scattered = Ray(rec.point, scatter_direction);
    attenuation = albedo;
    return true;
}
