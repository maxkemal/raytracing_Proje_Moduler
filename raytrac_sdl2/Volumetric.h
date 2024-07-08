#ifndef VOLUMETRIC_H
#define VOLUMETRIC_H

#include "Material.h"
#include "Vec3.h"
#include "Ray.h"
#include <algorithm>

class Volumetric : public Material {
public:
    Volumetric(const Vec3& a, double d, double ap, const Vec3& c, double max_d);

    virtual Vec3 emitted(double u, double v, const Vec3& p) const override;
    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const override;

    virtual MaterialType type() const override { return MaterialType::Volumetric; }

private:
    Vec3 albedo;
    double density;
    double absorption_probability;
    Vec3 center;
    double max_distance;

    double calculate_density(double distance_to_center) const;
    double calculate_absorption(double distance_to_center) const;
};

#endif // VOLUMETRIC_H
