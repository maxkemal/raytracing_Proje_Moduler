#ifndef LIGHT_H
#define LIGHT_H

#include "Vec3SIMD.h"

enum class LightType {
    Point,
    Directional,
    Area
};

class Light {
public:
    Vec3SIMD position;
    Vec3SIMD intensity;
    Vec3SIMD direction;  // Alan ýþýðý için yön
    Vec3SIMD u, v;       // Alan ýþýðý için düzlem vektörleri
    double width, height;  // Alan ýþýðý için boyutlar

    // Default constructor
    Light();

    // Sanal yýkýcý eklendi
    virtual ~Light() = default;

    virtual Vec3 random_point() const = 0;
    virtual LightType type() const = 0;
};

#endif // LIGHT_H
