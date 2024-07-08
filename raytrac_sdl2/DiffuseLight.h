#ifndef DIFFUSELIGHT_H
#define DIFFUSELIGHT_H

#include "Material.h"

class DiffuseLight : public Material {
public:
    DiffuseLight(Vec3 c);

    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const override;

    virtual Vec3 emitted(double u, double v, const Vec3& p) const override;

public:
    Vec3 emit;
};

#endif // DIFFUSELIGHT_H
