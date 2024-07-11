#ifndef VEC2_H
#define VEC2_H

#include <iostream>

class Vec2 {
public:
    float u, v;

    // Constructors
    Vec2(float u = 0.0f, float v = 0.0f);
    Vec2(const Vec2& other);
   
    // Common vector operations
    float length() const;
    float lengthSquared() const;
    void normalize();
    Vec2 normalized() const;
    Vec2 rotate(float angle) const;

    // Dot and cross products
    float dot(const Vec2& other) const;
    float cross(const Vec2& other) const;

    // Angle between vectors
    float angle(const Vec2& other) const;

    // Operator overloads
    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator*(float scalar) const;
    Vec2 operator/(float scalar) const;
    Vec2& operator+=(const Vec2& other);
    Vec2& operator-=(const Vec2& other);
    Vec2& operator*=(float scalar);
    Vec2& operator/=(float scalar);
    bool operator==(const Vec2& other) const;
    bool operator!=(const Vec2& other) const;
    Vec2 operator-() const;
    float& operator[](int i);
    const float& operator[](int i) const;

    // Static methods for common vectors
    static Vec2 zero();
    static Vec2 one();
    static Vec2 up();
    static Vec2 down();
    static Vec2 left();
    static Vec2 right();

    // Utility functions
    void clamp(float minVal, float maxVal);
    Vec2 lerp(const Vec2& other, float t) const;

    // Stream insertion operator for easy printing
    friend std::ostream& operator<<(std::ostream& os, const Vec2& vec);
};

// Non-member operator overloads
Vec2 operator*(float scalar, const Vec2& vec);

#endif // VEC2_H