#include "SSSMaterial.h"

SSSMaterial::SSSMaterial(const Vec3& albedo, const Vec3& scattering_color, double scattering_distance, double phase_function, double fresnel_effect)
    : albedo(albedo), scattering_color(scattering_color), scattering_distance(scattering_distance), phase_function(phase_function) {}


Vec3 SSSMaterial::emitted(double u, double v, const Vec3& p) const {
    return Vec3(0, 0, 0); // No emission for SSS material
}
// Yard�mc� fonksiyonlar
Vec3 lerp(const Vec3 & a, const Vec3 & b, double t) {
    return a * (1 - t) + b * t;
}

bool SSSMaterial::scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
    // Basit bir y�zey/derin sa��lma olas�l���
    
    double random_factor = static_cast<double>(rand()) / RAND_MAX;

    if (random_factor < phase_function) {
        // Y�zey sa��lmas�
        Vec3 scatter_direction = rec.normal + Vec3::random_in_unit_sphere();
        scattered = Ray(rec.point, scatter_direction.normalize());
        attenuation = albedo;
    }
    else {
        // Derin sa��lma
        // Basit bir sa��lma mesafesi hesaplama
        double scatter_distance = scattering_distance * static_cast<double>(rand()) / RAND_MAX;
        Vec3 scatter_direction = Vec3::random_in_unit_sphere();

        Vec3 new_origin = rec.point + scatter_direction * scattering_distance;
        scattered = Ray(new_origin, scatter_direction.normalize());

        // Derinli�e ba�l� renk kar���m�
        double depth = exp(-scatter_distance * 2.0); // 2.0 sa��lma katsay�s� olarak kullan�labilir
        attenuation = lerp(scattering_color, albedo, depth);
    }

    // Basit ge�irgenlik efekti
   // I����n objenin i�inden ge�ip ge�medi�ini kontrol ediyoruz
    double cos_theta = Vec3::dot(-r_in.direction, rec.normal);
    if (cos_theta < 0) {  // I��k objenin i�inden ge�iyor
        double translucency = exp(-rec.t * 0.5); // 0.5 ge�irgenlik fakt�r� olarak kullan�labilir
        attenuation = lerp(attenuation, scattering_color, translucency);
    }

    return true;
}


MaterialType SSSMaterial::type() const {
    return MaterialType::SSS;
}




