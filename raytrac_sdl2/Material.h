# pragma once

#include "Ray.h"
#include "Vec3.h"
#include "Hittable.h"
#include <memory>
#include "Texture.h"

enum class MaterialType {
    Lambertian,
    Metal,
    Dielectric,
    Volumetric,   
   
};
struct UVData {
    Vec2 original;
    Vec2 transformed;
};

enum class WrapMode {
    Repeat,
    Mirror,
    Clamp,
    Planar,
    Cubic
};
struct MaterialProperty {
    Vec3 color;
    float intensity;
    
    std::shared_ptr<Texture> texture;
    operator float() const {
        return intensity;  // veya baþka bir uygun deðer
    }

    // Veya
    float getValue() const {
        return intensity;  // veya baþka bir uygun deðer
    }
    MaterialProperty(const Vec3& c = Vec3(1, 1, 1), float i = 1.0f, std::shared_ptr<Texture> tex = nullptr)
        : color(c), intensity(i), texture(tex) {}
};

class Material {
public:
    MaterialProperty albedoProperty;
    MaterialProperty roughnessProperty;
    MaterialProperty metallicProperty;
    MaterialProperty normalProperty;
    virtual Vec2 applyTextureTransform(double u, double v) const {
        return Vec2(u, v);  // Varsayýlan olarak dönüþüm uygulamaz
    }
    virtual std::shared_ptr<Texture> getTexture() const {
        if (albedoProperty.texture) return albedoProperty.texture;
        if (roughnessProperty.texture) return roughnessProperty.texture;
        if (metallicProperty.texture) return metallicProperty.texture;
        if (normalProperty.texture) return normalProperty.texture;
        return nullptr;
    }


    virtual ~Material() = default;
    MaterialProperty shininess;
    MaterialProperty metallic;
    virtual MaterialType type() const = 0;
    
    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const = 0;
    virtual float get_metallic() const { return metallic.getValue(); }
    virtual Vec3 emitted(double u, double v, const Vec3& p) const {
        return Vec3(0, 0, 0);
    }

    // Texture handling
    virtual bool hasTexture() const { return texture != nullptr; }
   
    void setTexture(std::shared_ptr<Texture> tex) { texture = tex; }

    // Material properties
   
    
   
    virtual float getIOR() const { return ior; } // Yeni: Kýrýlma indeksi
    virtual Vec3 getF0() const { return f0; } // Yeni: Fresnel yansýma katsayýsý
    virtual bool has_normal_map() const { return false; }
    virtual Vec3 get_normal_from_map(double u, double v) const { return Vec3(0, 0, 1); }
    virtual float get_normal_strength() const { return 1.0f; }
    virtual float get_shininess() const {
        return shininess.getValue() * 128.0f;  // veya shininess.intensity
    }
   
    // Tiling
    Vec2 tilingFactor = Vec2(1, 1);
    void setTilingFactor(const Vec2& factor) { tilingFactor = factor; }
    Vec2 getTilingFactor() const { return tilingFactor; }

    // Advanced scattering methods
    virtual bool volumetric_scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
        return false;
    }
    virtual bool sss_scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
        return false;
    }

    // Yeni: Anizotropik malzemeler için
    virtual Vec3 getAnisotropicDirection() const { return Vec3(1, 0, 0); }
    virtual float getAnisotropy() const { return 0.0f; }

    // Yeni: Emisyon özelliði
    virtual Vec3 getEmission() const { return Vec3(0, 0, 0); }

protected:
    Vec3 albedo;
   
    float roughness = 0.5f;
    float ior = 1.5f; // Yeni: Varsayýlan kýrýlma indeksi
    Vec3 f0 = Vec3(0.04f); // Yeni: Varsayýlan Fresnel yansýma katsayýsý
    std::shared_ptr<Texture> texture;

    // Yeni: Malzeme özelliklerini ayarlamak için yardýmcý metotlar
    void setAlbedo(const Vec3& a) { albedo = a; }
   // void setMetallic(float m) { metallic = m; }
    void setRoughness(float r) { roughness = r; }
    void setIOR(float i) { ior = i; }
    void setF0(const Vec3& f) { f0 = f; }
};