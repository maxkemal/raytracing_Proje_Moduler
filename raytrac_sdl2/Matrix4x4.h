#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include "Vec3.h"  // Vec3 s�n�f� i�in gerekli ba�l�k dosyas�

class Matrix4x4 {
public:
    float m[4][4];

    Matrix4x4(); // Varsay�lan yap�c�

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
