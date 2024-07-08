#ifndef EMISSIVEMATERIAL_H
#define EMISSIVEMATERIAL_H

#include "Material.h"

class EmissiveMaterial : public Material {
public:
    EmissiveMaterial(const Vec3& emit);

    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const override;

    virtual Vec3 emitted(double u, double v, const Vec3& p) const override;

private:
    Vec3 emission;
};

#endif // EMISSIVEMATERIAL_H
