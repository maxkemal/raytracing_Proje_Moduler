// EnhancedMesh.cpp
#include "Mesh.h"
#include "AABB.h"
#include "Lambertian.h"
#include <algorithm>
#include "globals.h"

EnhancedMesh::EnhancedMesh(const ObjLoader::ObjMesh & objMesh, const std::unordered_map<std::string, std::unique_ptr<ObjLoader::ObjMaterial>>&materials) {
    // Convert vertices
    for (const auto& v : objMesh.vertices) {
        vertices.emplace_back(v.x, v.y, v.z);
    }

    // Convert normals
    for (const auto& n : objMesh.normals) {
        normals.emplace_back(n.x, n.y, n.z);
    }

    // Convert texture coordinates
    for (const auto& t : objMesh.texCoords) {
        texCoords.emplace_back(t.u, t.v);
    }

    // Convert faces
    for (const auto& f : objMesh.faces) {
        Face face;
        face.vertexIndices = f.vertexIndices;
        face.normalIndices = f.normalIndices;
        face.texCoordIndices = f.textureIndices;
        faces.push_back(face);
    }

    // Create material
    auto materialIt = materials.find(objMesh.materialName);
    if (materialIt != materials.end()) {
        material = createMaterialFromObjMaterial(materialIt->second.get());
    }
    else {
        material = std::make_shared<Lambertian>(Vec3(0.8, 0.8, 0.8), 0.1, 32);
    }

    // Triangulate faces if necessary
    triangulate();
}

bool EnhancedMesh::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    bool hit_anything = false;
    auto closest_so_far = t_max;

    for (const auto& face : faces) {
        if (rayTriangleIntersect(r, vertices[face.vertexIndices[0]], vertices[face.vertexIndices[1]], vertices[face.vertexIndices[2]],
            normals[face.normalIndices[0]], normals[face.normalIndices[1]], normals[face.normalIndices[2]],
            texCoords[face.texCoordIndices[0]], texCoords[face.texCoordIndices[1]], texCoords[face.texCoordIndices[2]],
            t_min, closest_so_far, rec)) {
            hit_anything = true;
            closest_so_far = rec.t;
            rec.material = material;
        }
    }

    return hit_anything;
}


bool EnhancedMesh::bounding_box(double time0, double time1, AABB& output_box) const {
    if (vertices.empty()) return false;

    Vec3 min(INFINITY, INFINITY, INFINITY);
    Vec3 max(-INFINITY, -INFINITY, -INFINITY);

    for (const auto& vertex : vertices) {
        for (int i = 0; i < 3; i++) {
            min[i] = fmin(min[i], vertex[i]);
            max[i] = fmax(max[i], vertex[i]);
        }
    }

    output_box = AABB(min, max);
    return true;
}

std::vector<std::shared_ptr<EnhancedMesh>> EnhancedMesh::createFromObjModel(const ObjLoader::ObjModel& model) {
    std::vector<std::shared_ptr<EnhancedMesh>> meshes;
    for (const auto& objMesh : model.meshes) {
        meshes.push_back(std::make_shared<EnhancedMesh>(*objMesh, model.materials));
    }
    return meshes;
}

void EnhancedMesh::triangulate() {
    std::vector<Face> triangulatedFaces;
    for (const auto& face : faces) {
        if (face.vertexIndices.size() == 3) {
            triangulatedFaces.push_back(face);
        }
        else {
            // Simple fan triangulation
            for (size_t i = 1; i < face.vertexIndices.size() - 1; ++i) {
                Face newFace;
                newFace.vertexIndices = { face.vertexIndices[0], face.vertexIndices[i], face.vertexIndices[i + 1] };
                newFace.normalIndices = { face.normalIndices[0], face.normalIndices[i], face.normalIndices[i + 1] };
                newFace.texCoordIndices = { face.texCoordIndices[0], face.texCoordIndices[i], face.texCoordIndices[i + 1] };
                triangulatedFaces.push_back(newFace);
            }
        }
    }
    faces = std::move(triangulatedFaces);
}

bool EnhancedMesh::rayTriangleIntersect(const Ray& r, const Vec3& v0, const Vec3& v1, const Vec3& v2,
    const Vec3& n0, const Vec3& n1, const Vec3& n2,
    const Vec2& t0, const Vec2& t1, const Vec2& t2,
    double t_min, double t_max, HitRecord& rec) const {

    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    Vec3 h = cross(r.direction, edge2);
    double a = Vec3::dot(edge1, h);

    if (a > -EPSILON && a < EPSILON) return false;

    double f = 1.0 / a;
    Vec3 s = r.origin - v0;
    double u = f * Vec3::dot(s, h);

    if (u < 0.0 || u > 1.0) return false;

    Vec3 q = cross(s, edge1);
    double v = f * Vec3::dot(r.direction, q);

    if (v < 0.0 || u + v > 1.0) return false;

    double t = f * Vec3::dot(edge2, q);

    if (t < t_min || t > t_max) return false;

    rec.t = t;
    rec.point = r.at(t);

    // Interpolate normal and texture coordinates
    double w = 1.0 - u - v;
    rec.normal = unit_vector(w * n0 + u * n1 + v * n2);
    rec.set_face_normal(r, rec.normal);
    rec.u = w * t0.u + u * t1.u + v * t2.u;
    rec.v = w * t0.v + u * t1.v + v * t2.v;

    return true;
}

std::shared_ptr<Material> EnhancedMesh::createMaterialFromObjMaterial(const ObjLoader::ObjMaterial* objMaterial) {
    if (objMaterial == nullptr) {
        return std::make_shared<Lambertian>(Vec3(0.8, 0.8, 0.8),0.1,32);
    }

    // Bu kýsmý, desteklediðiniz materyal türlerine göre geniþletebilirsiniz
    return std::make_shared<Lambertian>(Vec3(objMaterial->diffuse.x, objMaterial->diffuse.y, objMaterial->diffuse.z),0.1,32);
}