#ifndef VEC2_H
#define VEC2_H

#include <cmath>

class Vec2 {
public:
    float u, v;

    Vec2(float u = 0.0f, float v = 0.0f);

    float length() const;
    void normalize();
    Vec2 rotate(float angle) const;
    Vec2 operator*(float scalar) const;
    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator/(float scalar) const;
    float dot(const Vec2& other) const;
    float cross(const Vec2& other) const;
    float angle(const Vec2& other) const;
};

#endif // VEC2_H
