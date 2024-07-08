#include "Vec2.h"

Vec2::Vec2(float u, float v) : u(u), v(v) {}

float Vec2::length() const {
    return std::sqrt(u * u + v * v);
}

void Vec2::normalize() {
    float len = length();
    if (len != 0) {
        u /= len;
        v /= len;
    }
}

Vec2 Vec2::rotate(float angle) const {
    float cosAngle = std::cos(angle);
    float sinAngle = std::sin(angle);
    return Vec2(u * cosAngle - v * sinAngle, u * sinAngle + v * cosAngle);
}

Vec2 Vec2::operator*(float scalar) const {
    return Vec2(u * scalar, v * scalar);
}

Vec2 Vec2::operator+(const Vec2& other) const {
    return Vec2(u + other.u, v + other.v);
}

Vec2 Vec2::operator-(const Vec2& other) const {
    return Vec2(u - other.u, v - other.v);
}

Vec2 Vec2::operator/(float scalar) const {
    if (scalar != 0) {
        return Vec2(u / scalar, v / scalar);
    }
    else {
        return *this; // Handle error case, currently returning itself
    }
}

float Vec2::dot(const Vec2& other) const {
    return u * other.u + v * other.v;
}

float Vec2::cross(const Vec2& other) const {
    return u * other.v - v * other.u;
}

float Vec2::angle(const Vec2& other) const {
    return std::atan2(cross(other), dot(other));
}
