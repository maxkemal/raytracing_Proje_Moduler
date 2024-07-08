/* 1.07.20024
* 
  Ray Tracing Project

  This file contains an implementation of a ray tracing application written in C++,
  designed to render 3D scenes realistically using ray tracing techniques. It includes
  optimizations using SIMD and multi-threading for performance.

  Created by Kemal DEM�RTA� and licensed under the MIT License.
*/


/*
  Ray Tracing Projesi

  Bu dosya, ray tracing tekni�ini kullanarak 3D sahneleri foto-ger�ek�i bir �ekilde render etmek i�in
  C++ dilinde yaz�lm�� bir uygulamay� i�erir. SIMD kullan�m� ve �oklu i� par�ac��� deste�i ile performans
  optimizasyonu sa�lanm��t�r.

  Proje, Kemal DEM�RTA� taraf�ndan olu�turulmu�tur ve MIT Lisans� alt�nda lisanslanm��t�r.
*/




#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <iostream>
#include <iomanip>
#include "HittableList.h"
#include "light.h"
#include "BVHNode.h"
#include "Vec3.h"
#include "Camera.h"
#include "globals.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "AreaLight.h"
#include "Volumetric.h"
#include "matrix4x4.h"
#include "DirectionalLight.h"
#include "Lambertian.h"
#include "Dielectric.h"
#include "Metal.h"
#include "Material.h"
#include "Sphere.h"
#include "Box.h"
#include "Triangle.h"
#include "ObjLoader.h"
#include "ThreadLocalRNG.h"
#include "Vec2.h"
#include "Vec3SIMD.h"
#include "AABB.h"
#include "Ray.h"
#include "Hittable.h"
#include "EmissiveMaterial.h"
#include "DiffuseLight.h"

class Renderer {
public:
    Renderer(int image_width, int image_height, int samples_per_pixel, int max_depth);;
    ~Renderer();

    void render_image(SDL_Surface* surface, SDL_Window* window);
    std::pair<HittableList, std::shared_ptr<BVHNode>> create_scene(std::vector<std::shared_ptr<Light>>& lights, Vec3& background_color);

private:
    Vec3SIMD sample_directional_light(const BVHNode* bvh, const DirectionalLight* light, const HitRecord& rec, const Vec3SIMD& light_contribution);
    Vec3SIMD sample_point_light(const BVHNode* bvh, const PointLight* light, const HitRecord& rec, const Vec3SIMD& light_contribution);
    Vec3SIMD sample_area_light(const BVHNode* bvh, const AreaLight* light, const HitRecord& rec, const Vec3SIMD& light_contribution, int num_samples);
    void render_chunk(int start_row, int end_row, SDL_Surface* surface, const HittableList& world, const std::vector<std::shared_ptr<Light>>& lights, const Vec3& background_color, const BVHNode* bvh);
    void render_worker(int image_height, SDL_Surface* surface, const HittableList& world, const std::vector<std::shared_ptr<Light>>& lights, const Vec3& background_color, const BVHNode* bvh);
    void update_display(SDL_Window* window, SDL_Surface* surface);
    Vec3SIMD ray_color(const Ray& r, const BVHNode* bvh, const std::vector<std::shared_ptr<Light>>& lights, const Vec3SIMD& background_color, int depth=0);
    Vec3SIMD calculate_direct_lighting(const BVHNode* bvh, const std::vector<std::shared_ptr<Light>>& lights, const HitRecord& rec);
    Vec3SIMD calculate_light_contribution(const std::shared_ptr<Light>& light, const Vec3SIMD& point, const Vec3SIMD& normal, bool is_global = false);
    int image_width;
    int image_height;
    double aspect_ratio;
    int MAX_DEPTH = 50;
    std::atomic<int> next_row{ 0 };
    std::atomic<bool> rendering_complete{ false };
    std::atomic<int> completed_pixels{ 0 };
    std::mutex mtx;
};

#endif // RENDERER_H