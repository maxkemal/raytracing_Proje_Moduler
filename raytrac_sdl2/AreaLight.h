#ifndef AREA_LIGHT_H
#define AREA_LIGHT_H

#include "Light.h"
#include "Vec3.h"

class AreaLight : public Light {
public:
    AreaLight(const Vec3& pos, const Vec3& u_vec, const Vec3& v_vec, double w, double h, const Vec3& intens)
        : position(pos), u(u_vec), v(v_vec), width(w), height(h), intensity(intens) {
        direction = u.cross(v).normalize();
    }

    Vec3 random_point() const override {
        double rand_u = static_cast<double>(rand()) / RAND_MAX;
        double rand_v = static_cast<double>(rand()) / RAND_MAX;
        return position + u * rand_u * width + v * rand_v * height;
    }

    Vec3 getPosition() const { return position; }
    Vec3 getU() const { return u; }
    Vec3 getV() const { return v; }
    double getWidth() const { return width; }
    double getHeight() const { return height; }
    Vec3 getIntensity() const { return intensity; }
    Vec3 getDirection() const { return direction; }

    LightType type() const override { return LightType::Area; }

private:
    Vec3 position;
    Vec3 u;
    Vec3 v;
    double width;
    double height;
    Vec3 intensity;
    Vec3 direction;
};


#endif // AREA_LIGHT_H
