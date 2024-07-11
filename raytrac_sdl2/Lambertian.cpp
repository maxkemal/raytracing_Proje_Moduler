#include "Lambertian.h"
#include <cmath>
#include "Matrix4x4.h"

Lambertian::Lambertian(const Vec3& albedo, float roughness, float metallic)
    : albedoProperty(albedo), roughnessProperty(Vec3(roughness)), metallicProperty(Vec3(metallic)) {}

Lambertian::Lambertian(const std::shared_ptr<Texture>& albedoTexture, float roughness, float metallic)
    : albedoProperty(Vec3(1, 1, 1), 1.0f, albedoTexture),
    roughnessProperty(Vec3(roughness)),
    metallicProperty(Vec3(metallic)),
    tilingFactor(1, 1) {}
Lambertian::Lambertian(const std::shared_ptr<Texture>& albedoTexture, const std::shared_ptr<Texture>& roughnessTexture, float metallic)
    : albedoProperty(Vec3(1, 1, 1), 1.0f, albedoTexture),
    roughnessProperty(Vec3(0.5f), 1.0f, roughnessTexture),
    metallicProperty(Vec3(metallic)),
    tilingFactor(1, 1) {}
Lambertian::Lambertian(const std::shared_ptr<Texture>& albedoTexture, const std::shared_ptr<Texture>& roughnessTexture, const std::shared_ptr<Texture>& metallicTexture)
    : albedoProperty(Vec3(1, 1, 1), 1.0f, albedoTexture),
    roughnessProperty(Vec3(0.5f), 1.0f, roughnessTexture),
    metallicProperty(Vec3(0.0f), 1.0f, metallicTexture) {}
Lambertian::Lambertian(const std::shared_ptr<Texture>& albedoTexture, const std::shared_ptr<Texture>& roughnessTexture, float metallic, const std::shared_ptr<Texture>& normalTexture, const TextureTransform& transform)
    : albedoProperty(Vec3(1, 1, 1), 1.0f, albedoTexture),
    roughnessProperty(Vec3(0.5f), 1.0f, roughnessTexture),
    metallicProperty(Vec3(metallic)),
    normalProperty(Vec3(0.5, 0.5, 1.0), 1.0f, normalTexture),
    textureTransform(transform) {}
Lambertian::Lambertian(const std::shared_ptr<Texture>& albedoTexture, const std::shared_ptr<Texture>& roughnessTexture, const std::shared_ptr<Texture>& metallicTexture, const std::shared_ptr<Texture>& normalTexture, const TextureTransform& transform)
    : albedoProperty(Vec3(1, 1, 1), 1.0f, albedoTexture),
    roughnessProperty(Vec3(0.5f), 1.0f, roughnessTexture),
    metallicProperty(Vec3(0.0f), 1.0f, metallicTexture),
    normalProperty(Vec3(0.5, 0.5, 1.0), 1.0f, normalTexture),
    textureTransform(transform) {}
Lambertian::Lambertian(const std::shared_ptr<Texture>& albedoTexture, float roughness, float metallic, const std::shared_ptr<Texture>& normalTexture, const TextureTransform& transform)

    : albedoProperty(Vec3(1, 1, 1), 1.0f, albedoTexture),
    roughnessProperty(Vec3(roughness)),
    metallicProperty(Vec3(metallic)),
    normalProperty(Vec3(0.5, 0.5, 1.0), 1.0f, normalTexture),
    textureTransform(transform) {}
Lambertian::Lambertian(const std::shared_ptr<Texture>& albedoTexture, float roughness, float metallic, const TextureTransform& transform)

    : albedoProperty(Vec3(1, 1, 1), 1.0f, albedoTexture),
    roughnessProperty(Vec3(roughness)),
    metallicProperty(Vec3(metallic)),    
    textureTransform(transform) {}
    // Initialize other properties with default values

Lambertian::Lambertian()
    : textureTransform{ Vec2(1.0, 1.0), 0.0, Vec2(1.0, 1.0) } {}
bool Lambertian::hasTexture() const {
    return albedoProperty.texture != nullptr ||
        roughnessProperty.texture != nullptr ||
        metallicProperty.texture != nullptr ||
        normalProperty.texture != nullptr;
}


// Ya da setter metodu ile:
void Lambertian::set_normal_map(std::shared_ptr<Texture> normalMap, float normalStrength ) {
    normalProperty.texture = normalMap;
    normalProperty.intensity = normalStrength;
}
Vec3 Lambertian::get_normal_from_map(double u, double v) const {
    if (normalProperty.texture) {
        Vec2 Transform = applyTextureTransform(u, v);
        return normalProperty.texture->get_color(Transform.u, Transform.v);
    }
    return Vec3(0, 0, 1);
}
void Lambertian::setSpecular(const Vec3& specular, float intensity) {
    specularProperty = MaterialProperty(specular, intensity);
}
void Lambertian::setMetallic(float metallic, float intensity) {
    metallicProperty.color = Vec3(metallic, metallic, metallic);
    metallicProperty.intensity = intensity;
    metallicProperty.texture = nullptr;
}

void Lambertian::setMetallicTexture(const std::shared_ptr<Texture>& tex, float intensity) {
    metallicProperty.texture = tex;
    metallicProperty.intensity = intensity;
}
void Lambertian::setSpecularTexture(const std::shared_ptr<Texture>& tex, float intensity) {
    specularProperty = MaterialProperty(Vec3(1, 1, 1), intensity, tex);
}

void Lambertian::setEmission(const Vec3& emission, float intensity) {
    emissionProperty = MaterialProperty(emission, intensity);
}

void Lambertian::setEmissionTexture(const std::shared_ptr<Texture>& tex, float intensity) {
    emissionProperty = MaterialProperty(Vec3(1, 1, 1), intensity, tex);
}

void Lambertian::setClearcoat(float clearcoat, float clearcoatRoughness) {
    this->clearcoat = clearcoat;
    this->clearcoatRoughness = clearcoatRoughness;
}

void Lambertian::setAnisotropic(float anisotropic, const Vec3& anisotropicDirection) {
    this->anisotropic = anisotropic;
    this->anisotropicDirection = anisotropicDirection.normalize();
}

void Lambertian::setSubsurfaceScattering(const Vec3& sssColor, float sssRadius) {
    this->subsurfaceColor = sssColor;
    this->subsurfaceRadius = sssRadius;
}
 std::shared_ptr<Texture> Lambertian::getTexture() const {
    if (albedoProperty.texture) return albedoProperty.texture;
    if (roughnessProperty.texture) return roughnessProperty.texture;
    if (metallicProperty.texture) return metallicProperty.texture;
    if (normalProperty.texture) return normalProperty.texture;
    return nullptr;
}

Vec3 Lambertian::getTextureColor(double u, double v) const {
    UVData uvData = transformUV(u, v);
    Vec2 finalUV = applyWrapMode(uvData);
    return texture->get_color(finalUV.u, finalUV.v);
}

Vec2 Lambertian::applyTextureTransform(double u, double v) const {
   
    // Merkezi (0.5, 0.5) olarak kabul edelim
    u -= 0.5;
    v -= 0.5;
    // Ölçeklendirme
    u *= textureTransform.scale.u;
    v *= textureTransform.scale.v;
    // Döndürme (dereceyi radyana çevir)
    double rotation_radians = textureTransform.rotation_degrees * M_PI / 180.0;
    double cosTheta = std::cos(rotation_radians);
    double sinTheta = std::sin(rotation_radians);
    double newU = u * cosTheta - v * sinTheta;
    double newV = u * sinTheta + v * cosTheta;
    u = newU;
    v = newV;
    // Merkezi geri taþý
    u += 0.5;
    v += 0.5;
    // Öteleme uygula
    u += textureTransform.translation.u;
    v += textureTransform.translation.v;
    // Tiling uygula
    u = std::fmod(u * textureTransform.tilingFactor.u, 1.0);
    v = std::fmod(v * textureTransform.tilingFactor.v, 1.0);
    // Wrap mode uygula

    WrapMode::Planar;
    return Vec2(u, v);
}

UVData Lambertian::transformUV(double u, double v) const {
    UVData uvData;
    uvData.original = Vec2(u, v);

    // Tüm dönüþümleri uygula
    u -= 0.5;
    v -= 0.5;

    u *= textureTransform.scale.u;
    v *= textureTransform.scale.v;

    double rotation_radians = textureTransform.rotation_degrees * M_PI / 180.0;
    double cosTheta = std::cos(rotation_radians);
    double sinTheta = std::sin(rotation_radians);
    double newU = u * cosTheta - v * sinTheta;
    double newV = u * sinTheta + v * cosTheta;
    u = newU;
    v = newV;

    u += 0.5;
    v += 0.5;

    u += textureTransform.translation.u;
    v += textureTransform.translation.v;

    u *= textureTransform.tilingFactor.u;
    v *= textureTransform.tilingFactor.v;

    uvData.transformed = Vec2(u, v);
    return uvData;
}


Vec2 Lambertian::applyWrapMode(const UVData& uvData) const {
    switch (textureTransform.wrapMode) {
    case WrapMode::Repeat:
        return applyRepeatWrapping(uvData.transformed);
    case WrapMode::Mirror:
        return applyMirrorWrapping(uvData.transformed);
    case WrapMode::Clamp:
        return applyClampWrapping(uvData.transformed);
    case WrapMode::Planar:
        return applyPlanarWrapping(uvData.original);
    case WrapMode::Cubic:
        return applyCubicWrapping(uvData.transformed);
    }
    return uvData.transformed;
}
Vec2 Lambertian::applyRepeatWrapping(const Vec2& uv) const {
    double u = std::fmod(uv.u, 1.0);
    double v = std::fmod(uv.v, 1.0);
    if (u < 0) u += 1.0;
    if (v < 0) v += 1.0;
    return Vec2(u, v);
}

Vec2 Lambertian::applyMirrorWrapping(const Vec2& uv) const {
    double u = std::fmod(uv.u, 2.0);
    double v = std::fmod(uv.v, 2.0);
    if (u < 0) u += 2.0;
    if (v < 0) v += 2.0;
    if (u > 1.0) u = 2.0 - u;
    if (v > 1.0) v = 2.0 - v;
    return Vec2(u, v);
}

Vec2 Lambertian::applyClampWrapping(const Vec2& uv) const {
    double u = (uv.u < 0.0) ? 0.0 : ((uv.u > 1.0) ? 1.0 : uv.u);
    double v = (uv.v < 0.0) ? 0.0 : ((uv.v > 1.0) ? 1.0 : uv.v);
    return Vec2(u, v);
}

Vec2 Lambertian::applyPlanarWrapping(const Vec2& uv) const {
    // Planer sarma için orijinal UV'leri kullan
    return uv;
}

Vec2 Lambertian::applyCubicWrapping(const Vec2& uv) const {
    // UV koordinatlarýný 0-3 aralýðýna geniþlet
    double u_scaled = uv.u * 3.0;
    double v_scaled = uv.v * 3.0;

    // Hangi yüzeyde olduðumuzu belirle
    int face = static_cast<int>(u_scaled) + 3 * static_cast<int>(v_scaled);

    // Yüzey içindeki lokal koordinatlarý hesapla
    double u_local = std::fmod(u_scaled, 1.0);
    double v_local = std::fmod(v_scaled, 1.0);

    // Yüzeye göre koordinatlarý ayarla
    switch (face % 6) {  // 6'ya göre mod alarak taþmalarý önlüyoruz
    case 0: // Ön yüz
        return Vec2(u_local, v_local);
    case 1: // Sað yüz
        return Vec2(v_local, 1.0 - u_local);
    case 2: // Arka yüz
        return Vec2(1.0 - u_local, v_local);
    case 3: // Sol yüz
        return Vec2(1.0 - v_local, 1.0 - u_local);
    case 4: // Üst yüz
        return Vec2(u_local, 1.0 - v_local);
    case 5: // Alt yüz
        return Vec2(u_local, v_local);
    }

    // Bu noktaya asla ulaþýlmamalý, ama güvenlik için ekliyoruz
    return uv;
}

bool Lambertian::scatter(const Ray& r_in, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const {
    Vec2 transformedUV = applyTextureTransform(rec.u, rec.v);
    Vec3 albedoValue = getPropertyValue(albedoProperty, transformedUV);
    float roughness = getPropertyValue(roughnessProperty, transformedUV).x;
    float metallic = getPropertyValue(metallicProperty, transformedUV).x;
    Vec3 specularValue = getPropertyValue(specularProperty, transformedUV);

    Vec3 N = rec.normal;
   
    if (has_normal_map()) {
        Vec3 normalFromMap = get_normal_from_map(transformedUV.u, transformedUV.v);
        normalFromMap = normalFromMap * 2.0 - Vec3(1, 1, 1);
        float normalStrength = get_normal_strength();
        normalFromMap = normalFromMap * normalStrength;
        Vec3 T, B;
        createCoordinateSystem(N, T, B);
        N = (T * normalFromMap.x + B * normalFromMap.y + N * normalFromMap.z).normalize();
    }

    Vec3 V = -r_in.direction.normalize();
    Vec3 R = reflect(r_in.direction, N);

    Vec3 scatter_direction;
    if (anisotropic > 0) {
        Vec3 T, B;
        createCoordinateSystem(N, T, B);
        T = anisotropicDirection;
        B = Vec3::cross(N, T);
        scatter_direction = computeAnisotropicDirection(N, T, B, roughness, anisotropic);
    }
    else {
        // Metalik ve difüz yönleri hesapla
        Vec3 metal_dir = R + roughness * random_in_unit_sphere();
        Vec3 diffuse_dir = N + random_in_unit_sphere();

        // Metalik deðerine göre interpolasyon yap
        scatter_direction = lerp(diffuse_dir, metal_dir, metallic);
    }

    if (scatter_direction.near_zero()) {
        scatter_direction = N;
    }

    scattered = Ray(rec.point, scatter_direction.normalize());

    Vec3 F0 = lerp(Vec3(0.04f, 0.04f, 0.04f), albedoValue, metallic);
    float cosTheta = std::max(static_cast<float>(Vec3::dot(N, V)), 0.0f);
    Vec3 F = computeFresnel(F0, cosTheta);

    // Metalik deðerine göre difüz ve specular katkýlarý hesapla
    Vec3 specularContribution = specularValue * F;
    Vec3 diffuseContribution = albedoValue * (Vec3(1.0f, 1.0f, 1.0f) - F) * (1.0f - metallic);

    // Metalik deðerine göre attenuation'ý hesapla
    attenuation = lerp(diffuseContribution, specularContribution, metallic);

    if (clearcoat > 0) {
        Vec3 clearcoatF = computeFresnel(Vec3(0.04f, 0.04f, 0.04f), cosTheta);
        attenuation = lerp(attenuation, clearcoatF, clearcoat);
    }

    if (subsurfaceRadius > 0) {
        // Basit bir kalýnlýk tahmini. Gerçek uygulamada bu deðer daha karmaþýk hesaplanabilir.
        float estimatedThickness = 0.1f;
        Vec3 subsurfaceContribution = computeSubsurfaceScattering(N, V, estimatedThickness);
        attenuation += subsurfaceContribution * (1.0f - metallic);
        attenuation.clamp(0.0f, 1.0f);
    }

    Vec3 emissionValue = getPropertyValue(emissionProperty, transformedUV);
    attenuation += emissionValue;

    float max_component = std::max({ attenuation.x, attenuation.y, attenuation.z });
    if (max_component > 1.0f) {
        attenuation = attenuation / max_component;
    }
    // Attenuation deðerini sýnýrla
    attenuation.clamp(0.0f, 1.0f);

    return true;
}

Vec3 Lambertian::computeAnisotropicDirection(const Vec3& N, const Vec3& T, const Vec3& B, float roughness, float anisotropy) const {
    float r1 = random_double();
    float r2 = random_double();

    float phi = 2 * M_PI * r1;
    float cosTheta = std::sqrt((1 - r2) / (1 + (anisotropy * anisotropy - 1) * r2));
    float sinTheta = std::sqrt(1 - cosTheta * cosTheta);

    float x = sinTheta * std::cos(phi);
    float y = sinTheta * std::sin(phi);
    float z = cosTheta;

    return (x * T + y * B + z * N).normalize();
}

Vec3 Lambertian::computeSubsurfaceScattering(const Vec3& N, const Vec3& V, float thickness) const {
    float cosTheta = std::max(static_cast<float>(Vec3::dot(N, V)), 0.0f);

    // Iþýðýn malzeme içinden geçiþ mesafesini tahmin et
    float transmittance = std::exp(-thickness / subsurfaceRadius);

    // Malzemenin kenarlarýnda ve ince kýsýmlarýnda daha fazla ýþýk geçiþi
    float fresnel = std::pow(1.0f - cosTheta, 5.0f);
    float thinness = 0.5f - thickness;

    // SSS yoðunluðunu hesapla
    float sssIntensity = transmittance * (fresnel + thinness);

    return subsurfaceColor * sssIntensity;
}
void Lambertian::setTextureTransform(const TextureTransform& transform) {
    textureTransform = transform;
}

Vec3 Lambertian::computeScatterDirection(const Vec3& N, const Vec3& T, const Vec3& B, float roughness) const {
    float r1 = 2 * M_PI * random_double();
    float r2 = random_double();
    float r2s = std::sqrt(r2);

    float tx = r2s * std::cos(r1);
    float ty = r2s * std::sin(r1);
    float tz = std::sqrt(1 - r2);

    // Apply roughness
    tx *= roughness;
    ty *= roughness;

    return (tx * T + ty * B + tz * N).normalize();
}


void Lambertian::createCoordinateSystem(const Vec3& N, Vec3& T, Vec3& B) const {
    if (std::fabs(N.x) > std::fabs(N.y)) {
        T = Vec3(N.z, 0, -N.x).normalize();
    }
    else {
        T = Vec3(0, -N.z, N.y).normalize();
    }
    B = Vec3::cross(N, T);
}
// Reflection helper function
Vec3 Lambertian::reflect(const Vec3& v, const Vec3& n) const {
    return v - 2 * Vec3::dot(v, n) * n;
}

// Helper function to generate a random point in a unit sphere
Vec3 Lambertian::random_in_unit_sphere() const {
    while (true) {
        Vec3 p = Vec3(random_double(-1, 1), random_double(-1, 1), random_double(-1, 1));
        if (p.length_squared() < 1) return p;
    }
}
Vec3 Lambertian::computeFresnel(const Vec3& F0, float cosTheta) const {
    float p = std::pow(1.0f - cosTheta, 5.0f);
    return F0 + (Vec3(1.0f, 1.0f, 1.0f) - F0) * p;
}

Vec3 Lambertian::getPropertyValue(const MaterialProperty& prop, const Vec2& uv) const {
    if (prop.texture) {
        return prop.texture->get_color(uv.u, uv.v) * prop.intensity;
    }
    return prop.color * prop.intensity;
}

Vec2 Lambertian::applyTiling(double u, double v) const {
    return Vec2(fmod(u * tilingFactor.u, 1.0),
        fmod(v * tilingFactor.v, 1.0));
}

Vec3 Lambertian::lerp(const Vec3& a, const Vec3& b, float t) const {
    return a + t * (b - a);
}

