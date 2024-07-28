#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include "Light.h"
#include "Vec3.h"

class DirectionalLight : public Light {
public:
    DirectionalLight(const Vec3& dir, const Vec3& intens);
    Vec3 getDirection(const Vec3& point) const override {
        return -direction.normalize(); // I��k y�n�n�n tersi
    }

    Vec3 getIntensity(const Vec3& point) const override {
        return intensity; // Mesafeden ba��ms�z
    }

    Vec3 random_point() const override;
    LightType type() const override;
};

#endif // DIRECTIONAL_LIGHT_H