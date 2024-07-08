#include "Volumetric.h"
#include "Vec3.h"
#include "Ray.h"


Volumetric::Volumetric(const Vec3& a, double d, double ap, const Vec3& c, double max_d)
    : albedo(a), density(d), absorption_probability(ap), center(c), max_distance(max_d) {}

Vec3 Volumetric::emitted(double u, double v, const Vec3& p) const {
    return Vec3(0, 0, 0); // No emission for volumetric material
}

bool Volumetric::scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
    double random = random_double();

    // Distance-based density and absorption calculation
    double distance_to_center = (rec.point - center).length();
    double local_density = calculate_density(distance_to_center);
    double local_absorption = calculate_absorption(distance_to_center);

    // Absorption check
    if (random < local_absorption) {
        attenuation = Vec3(0, 0, 0); // Complete absorption
        return false;
    }

    // Scattering
    Vec3 scatter_direction = r_in.direction + Vec3::random_in_unit_sphere() * local_density;
    scattered = Ray(rec.point, scatter_direction.normalize());
    attenuation = albedo; // Constant albedo for now

    return true;
}

double Volumetric::calculate_density(double distance_to_center) const {
    // Example density calculation based on distance to center
    double density_factor = density * (1.0 - distance_to_center / max_distance);
    return std::max(0.0, density_factor);
}

double Volumetric::calculate_absorption(double distance_to_center) const {
    // Example absorption calculation based on distance to center
    double absorption_factor = absorption_probability * (distance_to_center / max_distance);
    return std::min(1.0, absorption_factor);
}
