// ObjLoader.h

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#ifdef OBJLOADER_EXPORTS
#define OBJLOADER_API __declspec(dllexport)
#else
#define OBJLOADER_API __declspec(dllimport)
#endif

namespace ObjLoader {
    struct ObjVec2 {
        float u, v;
    };

    struct ObjVec3 {
        float x, y, z;
    };

    struct ObjTriangle {
        ObjVec3 v1, v2, v3;
    };

    struct ObjFace {
        std::vector<int> vertexIndices;
        std::vector<int> textureIndices;
        std::vector<int> normalIndices;
    };

    struct ObjMaterial {
        std::string name;
        ObjVec3 ambient;
        ObjVec3 diffuse;
        ObjVec3 specular;
        float shininess;
        std::string diffuseTexture;
    };

    struct ObjMesh {
        std::string name;
        std::vector<ObjVec3> vertices;
        std::vector<ObjVec2> texCoords;
        std::vector<ObjVec3> normals;
        std::vector<ObjFace> faces;
        std::string materialName;
    };

    struct ObjModel {
        std::string name;
        std::vector<std::unique_ptr<ObjMesh>> meshes;
        std::unordered_map<std::string, std::unique_ptr<ObjMaterial>> materials;
    };

    class OBJLOADER_API Loader {
    public:
        static bool loadObj(const std::string& filename, std::vector<ObjTriangle>& triangles);
        static bool loadEnhancedObj(const std::string& filename, ObjModel& model);

    private:
        static bool parseFace(const std::string& line, ObjFace& face);
        static bool parseMaterialLibrary(const std::string& mtlFilename,
            std::unordered_map<std::string, std::unique_ptr<ObjMaterial>>& materials);
    };
} // namespace ObjLoader