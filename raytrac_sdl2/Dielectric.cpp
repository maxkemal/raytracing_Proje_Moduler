#include "Dielectric.h"
#include "Ray.h"
#include "Hittable.h"
#include <cmath>

Dielectric::Dielectric(double index_of_refraction, const Vec3& color, double caustic_intensity,
    double thickness, double tint_factor, double scratch_density)
    : ir(index_of_refraction), color(color), caustic_intensity(caustic_intensity),
    thickness(thickness), tint_factor(tint_factor), scratch_density(scratch_density) {}

Vec3 Dielectric::emitted(double u, double v, const Vec3& p) const {
    return Vec3(0.0, 0.0, 0.0);
}

bool Dielectric::scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
    attenuation = color;
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

    // Thickness effect
    Vec3 offset = rec.normal * thickness * (rec.front_face ? -1 : 1);
    scattered = Ray(rec.point + offset, direction);

    // Caustic effect
    double caustic_factor = calculate_caustic_factor(cos_theta, refraction_ratio, cannot_refract);
    Vec3 caustic_color = Vec3(1.0, 1.0, 0.9) * caustic_factor;
    attenuation = attenuation + caustic_color;

    // Apply tint
    attenuation = apply_tint(attenuation);

    // Apply scratches
    attenuation = apply_scratches(attenuation, rec.point);

    return true;
}

double Dielectric::getIndexOfRefraction() const {
    return ir;
}

double Dielectric::calculate_caustic_factor(double cos_theta, double refraction_ratio, bool is_reflected) const {
    double base_factor = pow(1.0 - cos_theta, 3) * caustic_intensity;
    if (is_reflected) {
        return base_factor * 0.5;  // Reflected light produces less intense caustics
    }
    else {
        return base_factor * (1.0 + 0.5 * sin(refraction_ratio * 10));  // Add some variation based on refraction
    }
}

Vec3 Dielectric::apply_tint(const Vec3& color) const {
    Vec3 tint_color(0.7, 0.7, 0.9);  // Slight blue tint typical for car windshields
    return color * (1.0 - tint_factor) + tint_color * tint_factor;
}

Vec3 Dielectric::apply_scratches(const Vec3& color, const Vec3& point) const {
    // Simple scratch effect based on position
    if (sin(point.x * 1000) * sin(point.y * 1000) * sin(point.z * 1000) < scratch_density) {
        return color * 0.9;  // Darken color slightly to simulate scratch
    }
    return color;
}

double Dielectric::reflectance(double cosine, double ref_idx) {
    // Use Schlick's approximation for reflectance
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}