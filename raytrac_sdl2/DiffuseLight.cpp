#include "DiffuseLight.h"

DiffuseLight::DiffuseLight(Vec3 c)
    : emit(c) {}

bool DiffuseLight::scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
    return false; // DiffuseLight materials do not scatter light
}

Vec3 DiffuseLight::emitted(double u, double v, const Vec3& p) const {
    return emit;
}
