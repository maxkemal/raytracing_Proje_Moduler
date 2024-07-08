#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "Material.h"

class Dielectric : public Material {
public:
    MaterialType type() const override { return MaterialType::Dielectric; }
    Dielectric(double index_of_refraction);

    virtual Vec3 emitted(double u, double v, const Vec3& p) const override;

    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const override;

private:
    double ir; // Index of Refraction

    static double reflectance(double cosine, double ref_idx);
};

#endif // DIELECTRIC_H
