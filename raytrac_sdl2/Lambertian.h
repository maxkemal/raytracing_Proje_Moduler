#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "Material.h"
#include "Vec3.h"

class Lambertian : public Material {
public:
    Lambertian(const Vec3& a);

    virtual Vec3 emitted(double u, double v, const Vec3& p) const override;
    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const override;

    virtual MaterialType type() const override { return MaterialType::Lambertian; }

private:
    Vec3 albedo;
};

#endif // LAMBERTIAN_H
