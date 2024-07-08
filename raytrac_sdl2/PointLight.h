#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"
#include "Vec3.h"

class PointLight : public Light {
public:
    PointLight(const Vec3& pos, const Vec3& intens, float rad);

    Vec3 random_point() const override;
    LightType type() const override;
    Vec3 getPosition() const { return position; }
    Vec3 getIntensity() const { return intensity; }
    float getRadius() const { return radius; }
private:
    Vec3 position;
    Vec3 intensity;
    float radius;
};

#endif // POINTLIGHT_H
