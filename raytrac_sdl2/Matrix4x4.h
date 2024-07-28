#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include "Vec3.h"  // Vec3 s�n�f� i�in gerekli ba�l�k dosyas�

class Matrix4x4 {
public:
    float m[4][4];
    Matrix4x4() {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = (i == j) ? 1.0f : 0.0f; // Identity matrix
    }
    Matrix4x4(Vec3 tangent, Vec3 bitangent, Vec3 normal);
   // Matrix4x4(); // Varsay�lan yap�c�
    // Vec3 ile Matrix4x4 �arpma operat�r�n� arkada� fonksiyon olarak tan�mlay�n
    friend Vec3 operator*(const Matrix4x4& mat, const Vec3& vec);
    void identity(); // Birim matris olu�turma

    // Matris �arp�m� operat�r�
    Matrix4x4 operator*(const Matrix4x4& other) const;

    Vec3 transform_point(const Vec3& p) const; // Nokta d�n���m�
    Vec3 transform_vector(const Vec3& v) const; // Vekt�r d�n���m�

    // Statik matris olu�turucular�
    static Matrix4x4 translation(const Vec3& t);
    static Matrix4x4 scaling(const Vec3& s);
    static Matrix4x4 rotation_x(float angle);
    // Y ve Z eksenleri i�in benzer rotasyon fonksiyonlar� eklenebilir
};

#endif // MATRIX4X4_H
