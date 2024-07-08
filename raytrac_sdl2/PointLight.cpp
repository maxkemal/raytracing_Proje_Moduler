#include "PointLight.h"

PointLight::PointLight(const Vec3& pos, const Vec3& intens, float rad)
    : position(pos), intensity(intens), radius(rad) {}

Vec3 PointLight::random_point() const {
    // Generate a random point within the sphere of radius `radius` around the position
    Vec3 random_offset = intensity.random_in_unit_sphere() * radius;
    return position + random_offset;
}

LightType PointLight::type() const {
    return LightType::Point;
}
