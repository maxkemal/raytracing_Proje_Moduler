#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <memory>
#include <string>
#include <algorithm>
#include "Hittable.h"
#include "Material.h"
#include "Vec3.h"
#include "Vec2.h"
#include "Matrix4x4.h"
#include "Vec3SIMD.h"
#include <SDL.h>
#include <SDL_image.h>

class Triangle : public Hittable {
public:
    Vec3 v0, v1, v2;  // Vertices
    Vec3 n0, n1, n2;  // Normals
    Vec2 t0, t1, t2;  // Texture coordinates
   
    std::shared_ptr<Material> material;
    Matrix4x4 transform;
    Vec3 albedo0, albedo1, albedo2;
    float metallic0, metallic1, metallic2;
    float roughness0, roughness1, roughness2;
    std::shared_ptr<Texture> texture;
    std::string materialName;
    int smoothGroup;

    // Default constructor
    Triangle();

    // Constructor with vertices and material
    Triangle(const Vec3& a, const Vec3& b, const Vec3& c, std::shared_ptr<Material> m);

    Triangle(const Vec3& a, const Vec3& b, const Vec3& c,
        const Vec3& na, const Vec3& nb, const Vec3& nc,
        const Vec2& ta, const Vec2& tb, const Vec2& tc,
        std::shared_ptr<Material> m, int sg);
        

    // Set transformation matrix
    void set_transform(const Matrix4x4& t);
    void render(SDL_Renderer* renderer, SDL_Texture* texture);
    // Set normals
    void set_normals(const Vec3& normal0, const Vec3& normal1, const Vec3& normal2);

    // Override hit function for ray-triangle intersection
    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;

    // Override bounding box function for bounding volume hierarchy (BVH)
    virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;

private:
    Vec3 min_point;
    Vec3 max_point;
    void update_bounding_box();
};

#endif // TRIANGLE_H
