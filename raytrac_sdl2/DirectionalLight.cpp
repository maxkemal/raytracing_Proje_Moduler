#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const Vec3& dir, const Vec3& intens) {
    direction = dir.normalize();
    intensity = intens;
    position = Vec3(0, 0, 0); // Arbitrary position, not used
}

Vec3 DirectionalLight::random_point() const {
    // Yönlü ýþýk için sonsuzda bir nokta
    return position + direction * 1000000.0;
}

LightType DirectionalLight::type() const {
    return LightType::Directional;
}