#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "Material.h"

class Dielectric : public Material {
public:
    MaterialType type() const override { return MaterialType::Dielectric; }
    Dielectric(double index_of_refraction,
        const Vec3& color = Vec3(0.95, 0.95, 1.0),
        double caustic_intensity = 0.1,
        double thickness = 0.006,  // Typical car windshield thickness
        double tint_factor = 0.2,
        double scratch_density = 0.001);

    virtual Vec3 emitted(double u, double v, const Vec3& p) const override;

    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const override;
    double getIndexOfRefraction() const;

private:
    double ir; // Index of Refraction
    Vec3 color; // Base color of the glass
    double caustic_intensity; // Intensity of the caustic effect
    double thickness; // Thickness of the glass
    double tint_factor; // How much the glass is tinted
    double scratch_density; // Density of scratches on the glass

    double calculate_caustic_factor(double cos_theta, double refraction_ratio, bool is_reflected) const;
    Vec3 apply_tint(const Vec3& color) const;
    Vec3 apply_scratches(const Vec3& color, const Vec3& point) const;
    static double reflectance(double cosine, double ref_idx);
};

#endif // DIELECTRIC_H