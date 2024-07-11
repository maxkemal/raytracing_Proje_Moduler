#pragma once

#include "Material.h"
#include "Texture.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Ray.h"
#include "Hittable.h"
#include <memory>




class Lambertian : public Material {
public:
    struct TextureTransform {
        Vec2 scale;
        double rotation_degrees;
        Vec2 translation;
        Vec2 tilingFactor;
        WrapMode wrapMode;

        TextureTransform(
            const Vec2& scale = Vec2(1.0, 1.0),
            double rotation = 0.0,
            const Vec2& translation = Vec2(0.0, 0.0),
            const Vec2& tiling = Vec2(1.0, 1.0),
            WrapMode wrap = WrapMode::Repeat
        ) : scale(scale), rotation_degrees(rotation), translation(translation),
            tilingFactor(tiling), wrapMode(wrap) {}
    };

  
   // Lambertian(std::shared_ptr<Texture> tex) : texture(tex) {}
    // Existing constructors
    Lambertian();
    Lambertian(const Vec3& albedo, float roughness, float metallic);
    Lambertian(const std::shared_ptr<Texture>& albedoTexture, float roughness = 0.5f, float metallic = 0.0f);
    Lambertian(const std::shared_ptr<Texture>& albedoTexture, const std::shared_ptr<Texture>& roughnessTexture, float metallic = 0.0f);
    Lambertian(const std::shared_ptr<Texture>& albedoTexture, const std::shared_ptr<Texture>& roughnessTexture, const std::shared_ptr<Texture>& metallicTexture);
    Lambertian(const std::shared_ptr<Texture>& albedoTexture, const std::shared_ptr<Texture>& roughnessTexture, float metallic, const std::shared_ptr<Texture>& normalTexture, const TextureTransform& transform = TextureTransform());
    Lambertian(const std::shared_ptr<Texture>& albedoTexture, const std::shared_ptr<Texture>& roughnessTexture, const std::shared_ptr<Texture>& metallicTexture, const std::shared_ptr<Texture>& normalTexture, const TextureTransform& transform = TextureTransform());
    Lambertian(const std::shared_ptr<Texture>& albedoTexture, float roughness, float metallic, const std::shared_ptr<Texture>& normalTexture, const TextureTransform& transform = TextureTransform());
    Lambertian(const std::shared_ptr<Texture>& albedoTexture, float roughness, float metallic, const TextureTransform& transform = TextureTransform());
  
    virtual bool hasTexture() const override;

    virtual std::shared_ptr<Texture> getTexture() const override;

   

    // Existing methods
    virtual MaterialType type() const override { return MaterialType::Lambertian; }
    virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const override;
    bool has_normal_map() const override { return normalProperty.texture != nullptr; }
    Vec3 get_normal_from_map(double u, double v) const override;
    float get_normal_strength() const override { return normalProperty.intensity; }

   

    void setTextureTransform(const TextureTransform& transform);
    void setTilingFactor(const Vec2& factor) { tilingFactor = factor; }
    void set_normal_map(std::shared_ptr<Texture> normalMap, float normalStrength = 1.0f);

    // New methods for PBR
    void setSpecular(const Vec3& specular, float intensity = 1.0f);
    void setSpecularTexture(const std::shared_ptr<Texture>& tex, float intensity = 1.0f);
    void setEmission(const Vec3& emission, float intensity = 1.0f);
    void setEmissionTexture(const std::shared_ptr<Texture>& tex, float intensity = 1.0f);
    void setClearcoat(float clearcoat, float clearcoatRoughness = 0.1f);
    void setAnisotropic(float anisotropic, const Vec3& anisotropicDirection = Vec3(1, 0, 0));
    void setSubsurfaceScattering(const Vec3& sssColor, float sssRadius);
    Vec3 getTextureColor(double u, double v) const;
    Vec2 applyTextureTransform(double u, double v) const;
    void setMetallic(float metallic, float intensity = 1.0f);
    void setMetallicTexture(const std::shared_ptr<Texture>& tex, float intensity = 1.0f);
   

  
private:
    MaterialProperty albedoProperty;
    MaterialProperty roughnessProperty;
    MaterialProperty metallicProperty;
    MaterialProperty normalProperty;
    Vec2 tilingFactor;
    TextureTransform textureTransform;

    // New properties for PBR
    MaterialProperty specularProperty;
    MaterialProperty emissionProperty;
    float clearcoat;
    float clearcoatRoughness;
    float anisotropic;
    Vec3 anisotropicDirection;
    Vec3 subsurfaceColor;
    float subsurfaceRadius;
     
    // Helper methods
    UVData transformUV(double u, double v) const;
    Vec2 applyWrapMode(const UVData& uvData) const;    
    Vec2 applyPlanarWrapping(double u, double v) const;
  
    Vec2 applyRepeatWrapping(const Vec2& uv) const;
    Vec2 applyMirrorWrapping(const Vec2& uv) const;
    Vec2 applyClampWrapping(const Vec2& uv) const;
    Vec2 applyPlanarWrapping(const Vec2& uv) const;
    Vec2 applyCubicWrapping(const Vec2& uv) const;
  
    Vec3 computeScatterDirection(const Vec3& N, const Vec3& T, const Vec3& B, float roughness) const;
    void createCoordinateSystem(const Vec3& N, Vec3& T, Vec3& B) const;
    Vec3 reflect(const Vec3& v, const Vec3& n) const;
    Vec3 random_in_unit_sphere() const;
    Vec3 computeFresnel(const Vec3& F0, float cosTheta) const;
    Vec3 getPropertyValue(const MaterialProperty& prop, const Vec2& uv) const;
    Vec2 applyTiling(double u, double v) const;
    Vec3 lerp(const Vec3& a, const Vec3& b, float t) const;
  
    // New helper methods for PBR
    Vec3 computeAnisotropicDirection(const Vec3& N, const Vec3& T, const Vec3& B, float roughness, float anisotropy) const;
    Vec3 computeSubsurfaceScattering(const Vec3& N, const Vec3& V, float thickness) const;
};