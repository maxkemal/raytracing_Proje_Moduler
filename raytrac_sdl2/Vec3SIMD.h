#ifndef VEC3SIMD_H
#define VEC3SIMD_H

#include <immintrin.h> // SSE header for __m128
#include "Vec3.h" // Assuming Vec3 class is defined here


class Vec3SIMD {
public:
    __m128 data;

    Vec3SIMD();
    Vec3SIMD(float x, float y, float z);
    Vec3SIMD(__m128 v);
    Vec3SIMD(const Vec3& v);
    // Vec3SIMD yazdýrma fonksiyonu
    friend std::ostream& operator<<(std::ostream& os, const Vec3SIMD& vec) {
        os << "(" << vec.x() << ", " << vec.y() << ", " << vec.z() << ")";
        return os;
    }
    operator Vec3() const;

    float x() const;
    float y() const;
    float z() const;

    Vec3SIMD operator-() const;
    Vec3SIMD& operator+=(const Vec3SIMD& v);
    Vec3SIMD& operator-=(const Vec3SIMD& v);
    Vec3SIMD& operator*=(const Vec3SIMD& v);
    Vec3SIMD& operator/=(const Vec3SIMD& v);
    Vec3SIMD& operator*=(float t);
    Vec3SIMD& operator/=(float t);

    float length() const;
    __m128 length_vec() const;
    float length_squared() const;

    Vec3SIMD normalize() const;
    float max_component() const;
    static Vec3SIMD to_vec3simd(const Vec3& vec) {
        return Vec3SIMD(_mm_set_ps(0.0f, vec.z, vec.y, vec.x));
    }
    friend Vec3SIMD operator+(const Vec3SIMD& u, const Vec3SIMD& v);
    friend Vec3SIMD operator-(const Vec3SIMD& u, const Vec3SIMD& v);
    friend Vec3SIMD operator*(const Vec3SIMD& u, const Vec3SIMD& v);
    friend Vec3SIMD operator/(const Vec3SIMD& u, const Vec3SIMD& v);
    friend Vec3SIMD operator*(float t, const Vec3SIMD& v);
    friend Vec3SIMD operator*(const Vec3SIMD& v, float t);
    friend Vec3SIMD operator/(const Vec3SIMD& v, float t);
    //Vec3SIMD(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}
};

Vec3SIMD dot(const Vec3SIMD& u, const Vec3SIMD& v);
Vec3SIMD cross(const Vec3SIMD& u, const Vec3SIMD& v);

#endif // VEC3SIMD_H
