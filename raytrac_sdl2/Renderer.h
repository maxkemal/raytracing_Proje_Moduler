/* 1.07.20024
* 
  Ray Tracing Project

  This file contains an implementation of a ray tracing application written in C++,
  designed to render 3D scenes realistically using ray tracing techniques. It includes
  optimizations using SIMD and multi-threading for performance.

  Created by Kemal DEMÝRTAÞ and licensed under the MIT License.
*/


/*
  Ray Tracing Projesi

  Bu dosya, ray tracing tekniðini kullanarak 3D sahneleri foto-gerçekçi bir þekilde render etmek için
  C++ dilinde yazýlmýþ bir uygulamayý içerir. SIMD kullanýmý ve çoklu iþ parçacýðý desteði ile performans
  optimizasyonu saðlanmýþtýr.

  Proje, Kemal DEMÝRTAÞ tarafýndan oluþturulmuþtur ve MIT Lisansý altýnda lisanslanmýþtýr.
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
#include "Mesh.h"
#include "AABB.h"
#include "Ray.h"
#include "Hittable.h"
#include "EmissiveMaterial.h"
#include "DiffuseLight.h"
#include "EnhancedObjLoader.h"
#include "SSSMaterial.h"
#include "ThreadLocalRNG.h"
#include "ObjLoaderAdapter.h"
#include "AtmosphericEffects.h"
#include "ParallelBVHNode.h"

class Renderer {
public:
    Renderer(int image_width, int image_height, int max_depth, int samples_per_pixel);;
    ~Renderer();

 
    //std::pair<HittableList, std::shared_ptr<BVHNode>> create_scene(std::vector<std::shared_ptr<Light>>& lights, Vec3& background_color);
    std::pair<HittableList, std::shared_ptr<ParallelBVHNode>> create_scene(std::vector<std::shared_ptr<Light>>& lights, Vec3SIMD& background_color);
    void render_chunk(int start_row, int end_row, SDL_Surface* surface, const HittableList& world, const std::vector<std::shared_ptr<Light>>& lights, const Vec3& background_color, const ParallelBVHNode* bvh, const int samples_per_pass, const int current_sample);
    void progressive_render(SDL_Surface* surface, const Vec3& background_color);
    void set_window(SDL_Window* win);

    void draw_progress_bar(SDL_Surface* surface, float progress);

    void render_image(SDL_Surface* surface, SDL_Window* window, const int total_samples_per_pixel, const int samples_per_pass);
    void set_camera_position(const Vec3SIMD& position) {
        camera_position = position;
    }
private:
    Vec3SIMD camera_position;
    AtmosphericEffects atmosphericEffects;
    SDL_Renderer* sdlRenderer; // SDL_Renderer pointer'ý ekleyin
    std::shared_ptr<Texture> background_texture;
    Vec3SIMD sample_directional_light(const ParallelBVHNode* bvh, const DirectionalLight* light, const HitRecord& rec, const Vec3SIMD& light_contribution);
    Vec3SIMD sample_point_light(const ParallelBVHNode* bvh, const PointLight* light, const HitRecord& rec, const Vec3SIMD& light_contribution);
    Vec3SIMD sample_area_light(const ParallelBVHNode* bvh, const AreaLight* light, const HitRecord& rec, const Vec3SIMD& light_contribution, int num_samples);
    
    void render_worker(int image_height, SDL_Surface* surface, const HittableList& world, const std::vector<std::shared_ptr<Light>>& lights, const Vec3& background_color, const ParallelBVHNode* bvh, const int samples_per_pass, const int current_sample);
    
    void update_display(SDL_Window* window, SDL_Surface* surface);
    Vec3SIMD apply_normal_map(const HitRecord& rec);
    void create_coordinate_system(const Vec3& N, Vec3& T, Vec3& B);
    Vec3SIMD ray_color(const Ray& r, const ParallelBVHNode* bvh, const std::vector<std::shared_ptr<Light>>& lights, const Vec3SIMD& background_color, int depth=0);
    Vec3SIMD calculate_light_contribution(const std::shared_ptr<Light>& light, const Vec3SIMD& point, const Vec3SIMD& geometric_normal, const Vec3SIMD& shading_normal, const Vec3SIMD& view_direction, float shininess, float metallic, bool is_global=false);
    Vec3SIMD calculate_direct_lighting(const ParallelBVHNode* bvh, const std::vector<std::shared_ptr<Light>>& lights, const HitRecord& rec, const Vec3SIMD& normal);
    int image_width;
    int image_height;
    double aspect_ratio;
    int MAX_DEPTH = 30;
      SDL_Window* window;
    std::atomic<int> next_row{ 0 };
    std::atomic<bool> rendering_complete{ false };
    std::atomic<int> completed_pixels{ 0 };
    std::mutex mtx;
    float max(float a, float b) const { return a > b ? a : b; }
};

#endif // RENDERER_H