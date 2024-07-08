#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include "Vec3.h"  // Vec3 sýnýfý için gerekli baþlýk dosyasý

class Matrix4x4 {
public:
    float m[4][4];

    Matrix4x4(); // Varsayýlan yapýcý

    void identity(); // Birim matris oluþturma

    // Matris çarpýmý operatörü
    Matrix4x4 operator*(const Matrix4x4& other) const;

    Vec3 transform_point(const Vec3& p) const; // Nokta dönüþümü
    Vec3 transform_vector(const Vec3& v) const; // Vektör dönüþümü

    // Statik matris oluþturucularý
    static Matrix4x4 translation(const Vec3& t);
    static Matrix4x4 scaling(const Vec3& s);
    static Matrix4x4 rotation_x(float angle);
    // Y ve Z eksenleri için benzer rotasyon fonksiyonlarý eklenebilir
};

#endif // MATRIX4X4_H
