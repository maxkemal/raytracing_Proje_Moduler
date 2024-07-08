#include "Dielectric.h"

Dielectric::Dielectric(double index_of_refraction)
    : ir(index_of_refraction) {}

Vec3 Dielectric::emitted(double u, double v, const Vec3& p) const {
    return Vec3(0, 0, 0); // No emission for dielectric material
}

bool Dielectric::scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
    attenuation = Vec3(1.0, 1.0, 1.0);
    double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

    Vec3 unit_direction = r_in.direction.normalize();
    double cos_theta = fmin(Vec3::dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    Vec3 direction;

    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
        direction = Vec3::reflect(unit_direction, rec.normal);
    else
        direction = Vec3::refract(unit_direction, rec.normal, refraction_ratio);

    scattered = Ray(rec.point, direction);
    return true;
}

double Dielectric::reflectance(double cosine, double ref_idx) {
    // Schlick's approximation for reflectance
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}
