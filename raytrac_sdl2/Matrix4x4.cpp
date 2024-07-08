#include "Matrix4x4.h"
#include <cmath> // cos ve sin fonksiyonlarý için

// Varsayýlan yapýcý
Matrix4x4::Matrix4x4() {
    identity();
}

// Birim matris oluþturma
void Matrix4x4::identity() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

// Matris çarpýmý operatörü
Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const {
    Matrix4x4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }
    return result;
}

// Nokta dönüþümü
Vec3 Matrix4x4::transform_point(const Vec3& p) const {
    float x = m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3];
    float y = m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3];
    float z = m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3];
    float w = m[3][0] * p.x + m[3][1] * p.y + m[3][2] * p.z + m[3][3];
    return Vec3(x / w, y / w, z / w);
}

// Vektör dönüþümü
Vec3 Matrix4x4::transform_vector(const Vec3& v) const {
    float x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
    float y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
    float z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;
    return Vec3(x, y, z);
}

// Statik matris oluþturucularý
Matrix4x4 Matrix4x4::translation(const Vec3& t) {
    Matrix4x4 mat;
    mat.m[0][3] = t.x;
    mat.m[1][3] = t.y;
    mat.m[2][3] = t.z;
    return mat;
}

Matrix4x4 Matrix4x4::scaling(const Vec3& s) {
    Matrix4x4 mat;
    mat.m[0][0] = s.x;
    mat.m[1][1] = s.y;
    mat.m[2][2] = s.z;
    return mat;
}

// X ekseni etrafýnda rotasyon matrisi oluþturma
Matrix4x4 Matrix4x4::rotation_x(float angle) {
    Matrix4x4 mat;
}
