#include "ObjLoaderAdapter.h"

// Yardýmcý dönüþüm fonksiyonlarý
inline Vec3 toVec3(const ObjLoader::ObjVec3& v) {
    return Vec3(v.x, v.y, v.z);
}

inline Vec2 toVec2(const ObjLoader::ObjVec2& v) {
    return Vec2(v.u, v.v);
}

std::vector<std::unique_ptr<Triangle>> ObjLoaderAdapter::loadObjToTriangles(const std::string& filename) {
    ObjLoader::ObjModel model;
    std::vector<std::unique_ptr<Triangle>> triangles;

    if (!ObjLoader::Loader::loadObj(filename, model)) {
        throw std::runtime_error("Failed to load OBJ file: " + filename);
    }

    for (const auto& mesh : model.meshes) {
        std::shared_ptr<Material> material = nullptr;
        if (!mesh->materialName.empty()) {
            auto it = model.materials.find(mesh->materialName);
            if (it != model.materials.end()) {
                material = createMaterialFromObjMaterial(it->second.get());
            }
        }

        for (const auto& face : mesh->faces) {
            if (face.vertexIndices.size() >= 3) {
                processFace(mesh.get(), face, material, triangles);
            }
        }
    }

    return triangles;
}

void ObjLoaderAdapter::processFace(const ObjLoader::ObjMesh* mesh, const ObjLoader::ObjFace& face,
    const std::shared_ptr<Material>& material,
    std::vector<std::unique_ptr<Triangle>>& triangles) {

    if (!mesh || mesh->vertices.empty() || face.vertexIndices.size() < 3) {
        // Hata iþleme
        return;
    }

    size_t vertexCount = face.vertexIndices.size();
    for (size_t i = 0; i < vertexCount - 2; ++i) {
        // Vertex kontrolü
        if (face.vertexIndices[0] >= mesh->vertices.size() ||
            face.vertexIndices[i + 1] >= mesh->vertices.size() ||
            face.vertexIndices[i + 2] >= mesh->vertices.size()) {
            // Hata iþleme
            return;
        }

        Vec3 v0 = toVec3(mesh->vertices[face.vertexIndices[0]]);
        Vec3 v1 = toVec3(mesh->vertices[face.vertexIndices[i + 1]]);
        Vec3 v2 = toVec3(mesh->vertices[face.vertexIndices[i + 2]]);

        Vec3 n0, n1, n2;
        if (!face.normalIndices.empty() && face.normalIndices.size() > i + 1) {
            if (face.normalIndices[0] < mesh->normals.size() && face.normalIndices[i + 1] < mesh->normals.size() && face.normalIndices[i + 2] < mesh->normals.size()) {
                n0 = toVec3(mesh->normals[face.normalIndices[0]]);
                n1 = toVec3(mesh->normals[face.normalIndices[i + 1]]);
                n2 = toVec3(mesh->normals[face.normalIndices[i + 2]]);
            }
            else {
                // Hata iþleme
                return;
            }
        }
        else {
            Vec3 normal = Vec3::cross(v1 - v0, v2 - v0).normalize();
            n0 = n1 = n2 = normal;
        }

        Vec2 t0 = getTextureCoords(mesh, face, 0);
        Vec2 t1 = getTextureCoords(mesh, face, i + 1);
        Vec2 t2 = getTextureCoords(mesh, face, i + 2);

        triangles.push_back(std::make_unique<Triangle>(
            v0, v1, v2, n0, n1, n2, t0, t1, t2, material, face.smoothGroup
        ));
    }
}
Vec2 ObjLoaderAdapter::getTextureCoords(const ObjLoader::ObjMesh* mesh, const ObjLoader::ObjFace& face, size_t index) {
    if (!face.textureIndices.empty() && index < face.textureIndices.size() &&
        mesh->texCoords.size() > face.textureIndices[index]) {
        return toVec2(mesh->texCoords[face.textureIndices[index]]);
    }
   // std::cerr << "UV coordinates not found for face index " << index << ". Returning default (0,0)" << std::endl;
    return Vec2(0, 0); // Varsayýlan deðer
}

std::shared_ptr<Material> ObjLoaderAdapter::createMaterialFromObjMaterial(const ObjLoader::ObjMaterial* objMaterial) {
    Vec3 albedo(objMaterial->diffuse.x, objMaterial->diffuse.y, objMaterial->diffuse.z);
    float metallic = (objMaterial->specular.x + objMaterial->specular.y + objMaterial->specular.z) / 3.0f;
    float shininess = (objMaterial->shininess < 1.0f) ? 10.0f : objMaterial->shininess;

    return std::make_shared<Lambertian>(albedo, metallic, shininess);
}