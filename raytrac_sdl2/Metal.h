#ifndef METAL_H
#define METAL_H

#include "Material.h"
#include "Vec3.h"

class Metal : public Material {
public:
    Metal(const Vec3& a, double f);
    MaterialType type() const override;
    virtual Vec3 emitted(double u, double v, const Vec3& p) const override;
    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const override;

private:
    Vec3 albedo;
    double fuzz;
};

#endif // METAL_H
