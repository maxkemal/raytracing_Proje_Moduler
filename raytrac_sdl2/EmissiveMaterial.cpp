#include "EmissiveMaterial.h"

EmissiveMaterial::EmissiveMaterial(const Vec3& emit)
    : emission(emit) {}

bool EmissiveMaterial::scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
    return false; // Emissive materials do not scatter light
}

Vec3 EmissiveMaterial::emitted(double u, double v, const Vec3& p) const {
    return emission;
}
