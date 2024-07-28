#include "Matrix4x4.h"
#include <cmath> // cos ve sin fonksiyonlar� i�in

// Varsay�lan yap�c�




// Birim matris olu�turma
void Matrix4x4::identity() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}
Matrix4x4::Matrix4x4(Vec3 tangent, Vec3 bitangent, Vec3 normal) {
    // �lk �� sat�r ve s�tunu TBN matrisine g�re ayarla
    m[0][0] = tangent.x;
    m[0][1] = tangent.y;
    m[0][2] = tangent.z;
    m[0][3] = 0.0f;

    m[1][0] = bitangent.x;
    m[1][1] = bitangent.y;
    m[1][2] = bitangent.z;
    m[1][3] = 0.0f;

    m[2][0] = normal.x;
    m[2][1] = normal.y;
    m[2][2] = normal.z;
    m[2][3] = 0.0f;

    // Son sat�r� ve s�tunu birim matrise g�re ayarla
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}
// Matrix4x4 ve Vec3 �arpma operat�r� tan�m�
Vec3 operator*(const Matrix4x4& mat, const Vec3& vec) {
    Vec3 result;
    result.x = mat.m[0][0] * vec.x + mat.m[0][1] * vec.y + mat.m[0][2] * vec.z + mat.m[0][3];
    result.y = mat.m[1][0] * vec.x + mat.m[1][1] * vec.y + mat.m[1][2] * vec.z + mat.m[1][3];
    result.z = mat.m[2][0] * vec.x + mat.m[2][1] * vec.y + mat.m[2][2] * vec.z + mat.m[2][3];
    return result;
}
// Matris �arp�m� operat�r�
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

// Nokta d�n���m�
Vec3 Matrix4x4::transform_point(const Vec3& p) const {
    float x = m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3];
    float y = m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3];
    float z = m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3];
    float w = m[3][0] * p.x + m[3][1] * p.y + m[3][2] * p.z + m[3][3];
    return Vec3(x / w, y / w, z / w);
}

// Vekt�r d�n���m�
Vec3 Matrix4x4::transform_vector(const Vec3& v) const {
    float x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
    float y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
    float z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;
    return Vec3(x, y, z);
}

// Statik matris olu�turucular�
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

// X ekseni etraf�nda rotasyon matrisi olu�turma
Matrix4x4 Matrix4x4::rotation_x(float angle) {
    Matrix4x4 mat;
    return mat;
}
