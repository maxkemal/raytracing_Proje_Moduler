#include "Vec3SIMD.h"
//#include "Vec3.h" // Assuming Vec3 class is defined here

Vec3SIMD::Vec3SIMD() : data(_mm_setzero_ps()) {}

Vec3SIMD::Vec3SIMD(float x, float y, float z) : data(_mm_set_ps(0, z, y, x)) {}

Vec3SIMD::Vec3SIMD(__m128 v) : data(v) {}

Vec3SIMD::Vec3SIMD(const Vec3& v) : data(_mm_set_ps(0, v.z, v.y, v.x)) {}

Vec3SIMD::operator Vec3() const {
    float result[4];
    _mm_store_ps(result, data);
    return Vec3(result[0], result[1], result[2]);
}

float Vec3SIMD::x() const { return _mm_cvtss_f32(data); }

float Vec3SIMD::y() const { return _mm_cvtss_f32(_mm_shuffle_ps(data, data, _MM_SHUFFLE(1, 1, 1, 1))); }

float Vec3SIMD::z() const { return _mm_cvtss_f32(_mm_shuffle_ps(data, data, _MM_SHUFFLE(2, 2, 2, 2))); }

Vec3SIMD Vec3SIMD::operator-() const { return Vec3SIMD(_mm_xor_ps(data, _mm_set1_ps(-0.0))); }

Vec3SIMD& Vec3SIMD::operator+=(const Vec3SIMD& v) {
    data = _mm_add_ps(data, v.data);
    return *this;
}

Vec3SIMD& Vec3SIMD::operator-=(const Vec3SIMD& v) {
    data = _mm_sub_ps(data, v.data);
    return *this;
}

Vec3SIMD& Vec3SIMD::operator*=(const Vec3SIMD& v) {
    data = _mm_mul_ps(data, v.data);
    return *this;
}

Vec3SIMD& Vec3SIMD::operator/=(const Vec3SIMD& v) {
    data = _mm_div_ps(data, v.data);
    return *this;
}

Vec3SIMD& Vec3SIMD::operator*=(float t) {
    data = _mm_mul_ps(data, _mm_set1_ps(t));
    return *this;
}

Vec3SIMD& Vec3SIMD::operator/=(float t) {
    data = _mm_div_ps(data, _mm_set1_ps(t));
    return *this;
}


float Vec3SIMD::length() const {
    __m128 squared = _mm_mul_ps(data, data);
    __m128 sum = _mm_hadd_ps(squared, squared);
    sum = _mm_hadd_ps(sum, sum);
    return _mm_cvtss_f32(_mm_sqrt_ss(sum));
}

__m128 Vec3SIMD::length_vec() const {
    __m128 squared = _mm_mul_ps(data, data);
    __m128 sum = _mm_hadd_ps(squared, squared);
    sum = _mm_hadd_ps(sum, sum);
    return _mm_sqrt_ps(sum);
}

float Vec3SIMD::length_squared() const {
    __m128 squared = _mm_mul_ps(data, data);
    __m128 sum = _mm_hadd_ps(squared, squared);
    sum = _mm_hadd_ps(sum, sum);
    return _mm_cvtss_f32(sum);
}

Vec3SIMD Vec3SIMD::normalize() const {
    __m128 len = _mm_set1_ps(length());
    return Vec3SIMD(_mm_div_ps(data, len));
}

float Vec3SIMD::max_component() const {
    __m128 max1 = _mm_max_ps(data, _mm_shuffle_ps(data, data, _MM_SHUFFLE(3, 0, 0, 1)));
    __m128 max2 = _mm_max_ps(max1, _mm_shuffle_ps(max1, max1, _MM_SHUFFLE(3, 0, 0, 2)));
    return _mm_cvtss_f32(max2);
}

Vec3SIMD operator+(const Vec3SIMD& u, const Vec3SIMD& v) {
    return Vec3SIMD(_mm_add_ps(u.data, v.data));
}

Vec3SIMD operator-(const Vec3SIMD& u, const Vec3SIMD& v) {
    return Vec3SIMD(_mm_sub_ps(u.data, v.data));
}

Vec3SIMD operator*(const Vec3SIMD& u, const Vec3SIMD& v) {
    return Vec3SIMD(_mm_mul_ps(u.data, v.data));
}

Vec3SIMD operator/(const Vec3SIMD& u, const Vec3SIMD& v) {
    return Vec3SIMD(_mm_div_ps(u.data, v.data));
}

Vec3SIMD operator*(float t, const Vec3SIMD& v) {
    return Vec3SIMD(_mm_mul_ps(_mm_set1_ps(t), v.data));
}

Vec3SIMD operator*(const Vec3SIMD& v, float t) {
    return t * v;
}

Vec3SIMD operator/(const Vec3SIMD& v, float t) {
    return Vec3SIMD(_mm_div_ps(v.data, _mm_set1_ps(t)));
}

Vec3SIMD dot(const Vec3SIMD& u, const Vec3SIMD& v) {
    __m128 prod = _mm_mul_ps(u.data, v.data);
    __m128 sum = _mm_hadd_ps(prod, prod);
    sum = _mm_hadd_ps(sum, sum);
    return Vec3SIMD(sum);
}


Vec3SIMD cross(const Vec3SIMD& u, const Vec3SIMD& v) {
    __m128 a = _mm_shuffle_ps(u.data, u.data, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 b = _mm_shuffle_ps(v.data, v.data, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 c = _mm_shuffle_ps(u.data, u.data, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 d = _mm_shuffle_ps(v.data, v.data, _MM_SHUFFLE(3, 0, 2, 1));
    return Vec3SIMD(_mm_sub_ps(_mm_mul_ps(a, b), _mm_mul_ps(c, d)));
}
