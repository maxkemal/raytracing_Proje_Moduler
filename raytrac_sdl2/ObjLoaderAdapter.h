#pragma once
#include "ObjLoader.h"
#include "Material.h"
#include "Lambertian.h"
#include "Triangle.h"
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>

class ObjLoaderAdapter {
public:
    static std::vector<std::unique_ptr<Triangle>> loadObjToTriangles(const std::string& filename);

private:
    static std::shared_ptr<Material> createMaterialFromObjMaterial(const ObjLoader::ObjMaterial* objMaterial);
    static void processFace(const ObjLoader::ObjMesh* mesh, const ObjLoader::ObjFace& face,
        const std::shared_ptr<Material>& material,
        std::vector<std::unique_ptr<Triangle>>& triangles);
    static Vec2 getTextureCoords(const ObjLoader::ObjMesh* mesh, const ObjLoader::ObjFace& face, size_t index);
};