#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <memory>
#include <string>
#include <algorithm>
#include "Hittable.h"
#include "Vec3.h"
#include "Matrix4x4.h"

class Triangle : public Hittable {
public:
    const double EPSILON = 1e-7;
    std::string materialName;
    Vec3 v0, v1, v2;
    std::shared_ptr<Material> material;
    Matrix4x4 transform;

    // Varsayýlan oluþturucu
    Triangle();
    Triangle(const Vec3& a, const Vec3& b, const Vec3& c, std::shared_ptr<Material> m);

    void set_transform(const Matrix4x4& t);

    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;

private:
    Vec3 min_point;
    Vec3 max_point;
};

#endif // TRIANGLE_H
