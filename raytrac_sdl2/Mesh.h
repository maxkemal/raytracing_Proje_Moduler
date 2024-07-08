#ifndef MESH_H
#define MESH_H

#include <vector>
#include <memory>
#include <string>
#include "Hittable.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Triangle.h"

class Mesh : public Hittable {
public:
    std::vector<Vec3> vertices;
    std::vector<Vec2> texCoords;
    std::vector<Vec3> normals;
    std::string materialName; // Materyal ismi, isteðe baðlý olarak kullanýlabilir

    // Yüz (face) yapýsý
    struct Face {
        std::vector<int> vertexIndices;
        std::vector<int> textureIndices;
        std::vector<int> normalIndices;
    };

    std::vector<Face> faces; // Yüzlerin listesi

    // Üçgenleri oluþturma ve dönüþtürme metodu
    std::vector<std::shared_ptr<Triangle>> generateTriangles(std::shared_ptr<Material> material) const;
};

#endif // MESH_H
