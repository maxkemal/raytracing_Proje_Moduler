#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "Hittable.h"
#include "ObjLoader.h"
#include "Material.h"
#include "Vec3.h"
#include "Vec2.h"

class EnhancedMesh : public Hittable {
public:
    EnhancedMesh() = default;
    explicit EnhancedMesh(const ObjLoader::ObjMesh& objMesh, const std::unordered_map<std::string, std::unique_ptr<ObjLoader::ObjMaterial>>& materials);

    bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
    bool bounding_box(double time0, double time1, AABB& output_box) const override;

    static std::vector<std::shared_ptr<EnhancedMesh>> createFromObjModel(const ObjLoader::ObjModel& model);

private:
    struct Face {
        std::vector<int> vertexIndices;
        std::vector<int> normalIndices;
        std::vector<int> texCoordIndices;
    };

    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<Vec2> texCoords;
    std::vector<Face> faces;
    std::shared_ptr<Material> material;

    void triangulate();
    bool rayTriangleIntersect(const Ray& r, const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& n0, const Vec3& n1, const Vec3& n2, const Vec2& t0, const Vec2& t1, const Vec2& t2, double t_min, double t_max, HitRecord& rec) const;
   
    
    static std::shared_ptr<Material> createMaterialFromObjMaterial(const ObjLoader::ObjMaterial* objMaterial);
};