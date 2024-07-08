#include "Mesh.h"

std::vector<std::shared_ptr<Triangle>> Mesh::generateTriangles(std::shared_ptr<Material> material) const {
    std::vector<std::shared_ptr<Triangle>> triangles;

    for (const auto& face : faces) {
        if (face.vertexIndices.size() >= 3) {
            for (size_t i = 0; i < face.vertexIndices.size() - 2; ++i) {
                Vec3 v0 = vertices[face.vertexIndices[0]];
                Vec3 v1 = vertices[face.vertexIndices[i + 1]];
                Vec3 v2 = vertices[face.vertexIndices[i + 2]];

                auto triangle = std::make_shared<Triangle>(v0, v1, v2, material);
                triangles.push_back(triangle);
            }
        }
    }

    return triangles;
}
