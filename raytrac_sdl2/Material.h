#ifndef MATERIAL_H
#define MATERIAL_H

#include "Ray.h"
#include "Vec3.h"
#include "Hittable.h"
enum class MaterialType {
    Lambertian,
    Metal,
    Dielectric,
    Volumetric
};

class Material {
public:
    virtual MaterialType type() const = 0;
    virtual Vec3 emitted(double u, double v, const Vec3& p) const {
        return Vec3(0, 0, 0);
    }
    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const = 0;
    virtual ~Material() = default;
};

#endif // MATERIAL_H
