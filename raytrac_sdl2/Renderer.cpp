#include "renderer.h"
#include <SDL_image.h>


Renderer::Renderer(int image_width, int image_height, int samples_per_pixel, int max_depth)
    : image_width(image_width), image_height(image_height), aspect_ratio(static_cast<double>(image_width) / image_height) {}

Renderer::~Renderer() {}
void Renderer::set_window(SDL_Window* win) {
    window = win;
}
void Renderer::draw_progress_bar(SDL_Surface* surface, float progress) {
    const int bar_width = surface->w - 40;  // Kenarlarda 20 piksel boþluk býrakýyoruz
    const int bar_height = 20;
    const int bar_y = surface->h - 40;  // Alt kenardan 40 piksel yukarýda

    // Arka plan çubuðunu çiz
   // SDL_Rect bg_rect = { 20, bar_y, bar_width, bar_height };
    //SDL_FillRect(surface, &bg_rect, SDL_MapRGB(surface->format, 100, 100, 100));

    // Ýlerleme çubuðunu çiz
   // int progress_width = static_cast<int>(progress * bar_width);
    //SDL_Rect progress_rect = { 20, bar_y, progress_width, bar_height };
    //SDL_FillRect(surface, &progress_rect, SDL_MapRGB(surface->format, 0, 255, 0));

    // Yüzde metnini hazýrla
    char percent_text[10];
    snprintf(percent_text, sizeof(percent_text), "%.1f%%", progress * 100);
   
}

void Renderer::render_image(SDL_Surface* surface, SDL_Window* window, const int total_samples_per_pixel, const int samples_per_pass) {
    unsigned int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
   // std::cout << "Starting render with " << num_threads << " threads" << std::endl;
    auto start_time = std::chrono::steady_clock::now();
    Vec3SIMD background_color;
  
    std::vector<std::shared_ptr<Light>> lights;
    auto [world, bvh] = create_scene(lights, background_color);
    auto create_scene_end_time = std::chrono::steady_clock::now();
    auto create_scene_duration = std::chrono::duration<double, std::milli>(create_scene_end_time - start_time);
    std::cout << "Create Scene Duration: " << create_scene_duration.count() / 1000 << " seconds" << std::endl;

    std::thread display_thread(&Renderer::update_display, this, window, surface);

    const int num_passes = (total_samples_per_pixel + samples_per_pass - 1) / samples_per_pass;

    for (int pass = 0; pass < num_passes; ++pass) {
        std::cout << "Starting pass " << pass + 1 << " of " << num_passes << std::endl;

        next_row.store(0);  // Reset next_row for each pass

        for (unsigned int t = 0; t < num_threads; ++t) {
            threads.emplace_back(&Renderer::render_worker, this, image_height, surface, std::ref(world),
                std::ref(lights), background_color, bvh.get(), samples_per_pass, pass * samples_per_pass);
        }

        for (auto& thread : threads) {
            thread.join();
        }

        threads.clear();

        // Her geçiþten sonra ilerleme çubuðunu güncelle
        float progress = static_cast<float>(pass + 1) / num_passes;
       // draw_progress_bar(surface, progress);
        
        std::cout << "\rRendering progress: " << std::fixed << std::setprecision(2) << progress << "%" << std::flush;
        // Pencere baþlýðýný güncelle
        char title[100];
        snprintf(title, sizeof(title), "Rendering... %.1f%% Complete", progress * 100);
        SDL_SetWindowTitle(window, title);

        SDL_UpdateWindowSurface(window);
        std::cout << "Pass " << pass + 1 << " completed. Progress: " << (progress * 100) << "%" << std::endl;
    }

    rendering_complete = true;
    display_thread.join();

    std::cout << "\nRender completed." << std::endl;
    auto render_end_time = std::chrono::steady_clock::now();

    auto render_duration = std::chrono::duration<double, std::milli>(render_end_time - create_scene_end_time);
    auto total_duration = std::chrono::duration<double, std::milli>(render_end_time - start_time);

    std::cout << "Render Duration: " << render_duration.count() / 1000 << " seconds" << std::endl;
    std::cout << "Total Duration: " << total_duration.count() / 1000 << " seconds" << std::endl;

    // Render tamamlandýðýnda pencere baþlýðýný güncelle
    SDL_SetWindowTitle(window, "Render Completed");
}

// Diðer fonksiyonlar (render_worker, render_chunk) ayný kalacak
Vec3 smooth_blend(const Vec3& normal1, const Vec3& normal2, float angle_degrees) {
    float angle_radians = angle_degrees * (M_PI / 180.0f);
    float blend_factor = std::cos(angle_radians);

    // Interpolate between normal1 and normal2
    Vec3 smoothed_normal = normal1 * blend_factor + normal2 * (1.0f - blend_factor);
    smoothed_normal = smoothed_normal.normalize(); // Ensure the result is normalized

    return smoothed_normal;
}
Vec3 calculateNormal(const Vec3& v1, const Vec3& v2, const Vec3& v3) {
    Vec3 edge1 = v2 - v1;
    Vec3 edge2 = v3 - v1;
    return (edge1.cross(edge2)).normalize();
}
// OBJ dosyalarýnýn listesi ve her biri için bir materyal

// Texture yükleme fonksiyonu

struct ObjFile {
    std::string filename;
    std::shared_ptr<Material> material;
    std::string texturePath;  // Objeye ait texture dosya yolu
    
};
std::shared_ptr<Material> loadMaterialFromMtl(const std::string& materialName) {
    // MTL dosyasýndan materyal bilgilerini yükleme
    // Örneðin, bir dosya okuyucu ve analizci ile
    if (materialName == "ground.mtl") {
        return std::make_shared<Lambertian>(Vec3(0.8f, 0.7f, 0.5f),0,0); // Toprak rengi
    }
    else if (materialName == "kure.mtl") {
        return std::make_shared<Metal>(Vec3(0.98, 0.98, 1.0), 0.13, 1, 0.0, 1); // Kýrmýzý metal
    }
    // Geçerli bir materyal yoksa varsayýlan bir materyal döndür
    return std::make_shared<Lambertian>(Vec3(0.5f, 0.5f, 0.5f),0,0); // Gri
}

std::pair<HittableList, std::shared_ptr<ParallelBVHNode>> Renderer::create_scene(std::vector<std::shared_ptr<Light>>& lights, Vec3SIMD& background_color) {
    HittableList world;
    Vec3 v0, v1, v2;
   
    atmosphericEffects.use_background_texture = true;
    atmosphericEffects.setBackgroundTexture("Texture/kure.jpg");
    background_color = { 0.3, 0.4, 0.5 };   
    atmosphericEffects.enable = true;
    AtmosphericEffects atmosphericEffects(
        100.0f,  // fog_start_distance
        0.01f,   // fog_base_density
        0.0001f, // fog_distance_factor
        0.0002f  // haze_density
    );
    
// Albedo, roughness, ve metallic için texture oluþturma
    auto albedo_texture = std::make_shared<Texture>("Texture/granidalbedo.jpg");
    auto roughness_texture = std::make_shared<Texture>("Texture/granidroug.jpg");
    auto metallic_texture = std::make_shared<Texture>("Texture/granidroug.jpg");
    auto Normal_texture = std::make_shared<Texture>("Texture/granidnormal.jpg");

    Lambertian::TextureTransform customTransform(
        Vec2(1.0, 1.0),  // scale
        0.0,             // rotation
        Vec2(0.0, 0.0),  // translation (orijinal örnekte belirtilmemiþ)
        Vec2(2.0, 2.0),  // tiling
        WrapMode::Planar // wrap mode (varsayýlan olarak Repeat kullanýldý)
    );
    auto ground_material = std::make_shared<Lambertian>(albedo_texture, 0.05,0.9, Normal_texture, customTransform);
    //TextureTransform customTransform(Vec2(1.0, 1.0), 0.0, Vec2(1.0, 1.0));
    // Yeni PBR özelliklerini ayarlama
    ground_material->setSpecular(Vec3(1, 1, 1), 1.0f);
    ground_material->setEmission(Vec3(0.8, 0.1, 0.1), 0.0f); // Hafif bir emission ekleyelim
    ground_material->setClearcoat(0.5f, 0.1f); // %50 clearcoat, 0.1 roughness ile
   // ground_material->setAnisotropic(0.0f, Vec3(1, 0, 0)); // X yönünde %50 anizotropik
    //ground_material->setMetallic(1.0, 0.0);
    //ground_material->setSubsurfaceScattering(Vec3(0.8, 0.2, 0.2), 0.1f); // Kýrmýzýmsý subsurface scattering

    // Opsiyonel: Wrap mode'u ayarlama
    //customTransform.wrapMode = WrapMode::Mirror;
    ground_material->setTextureTransform(customTransform);
   

   // customTransform.wrapMode= WrapMode::Clamp;
    // Lambertian materyali oluþturma
   // auto ground_material = std::make_shared<Lambertian>(albedo_texture, roughness_texture, 0.95, Normal_texture, customTransform);

    auto kaput_albedo = std::make_shared<Texture>("Texture/lambert/3dif.jpg");
   
    auto kaput_rough = std::make_shared<Texture>("Texture/lambert/3roug.jpg");
    auto kaput_normal = std::make_shared<Texture>("Texture/lambert/3nor.jpg");
    auto Home_texture1 = std::make_shared<Texture>("Texture/lambert/3dif.jpg");

    auto car1_Material = std::make_shared<Metal>(Vec3(0.97f, 0.96f, 0.91f), 0.2, 1, 0.0, 1);
    car1_Material->setSpecular(Vec3(1, 1, 1), 1.0f);
    car1_Material->setClearcoat(1, 0.0, Vec3(0.97f, 0.96f, 0.91f));
    car1_Material->setMetallic(1.0f, 1.0f, Vec3(0.97f, 0.96f, 0.91f));
    car1_Material->setAnisotropic(0.0, Vec3(0, 0, 0.0));
    car1_Material->setEmission(Vec3(0.1, 0.1, 0.1), 0.0);
   
   // car1_Material->setSubsurfaceScattering(Vec3(0.8,0.6, 0.4), 1.0f);
    //Home_Material->setTilingFactor(Vec2(0.1, 0.1));  //  tiling
   

    auto lastik_Material = std::make_shared<Metal>(Vec3(0.1, 0.1, 0.1), 0.012, 0.1, 0.2, 0.1);
    lastik_Material->setEmission(Vec3(0.1, 0.1, 0.1), 0.0f); // Hafif bir emission ekleyelim
    lastik_Material->setClearcoat(0.2, 0.2, Vec3(0.91, 0.92, 0.92));
    lastik_Material->setSpecular(Vec3(1, 1, 1), 1.0f);
    lastik_Material->setAnisotropic(0.0f, Vec3(1, 0, 1)); // X yönünde %50 anizotropik
    //lastik_Material->setSubsurfaceScattering(Vec3(0.2, 0.2, 0.9), 0.2f); //  subsurface scattering
    //Home_Material1->setTilingFactor(Vec2(0.01,0.01));  //  tiling
    auto jant_Material = std::make_shared<Metal>(Vec3(0.97f, 0.96f, 0.91f), 0.2, 1, 0.0, 1);
    jant_Material->setSpecular(Vec3(1, 1, 1), 1.0f);
    jant_Material->setClearcoat(1, 0, Vec3(0.97f, 0.96f, 0.91f));
    jant_Material->setMetallic(1, 1, Vec3(0.97f, 0.96f, 0.91f));
    jant_Material->setEmission(Vec3(0.1, 0.1, 0.1), 0.0);

    auto dielectric_material = std::make_shared<Dielectric>(1.5,Vec3(1,1,1), 0,0.006,0.2,0.001);
    auto clean_windshield = std::make_shared<Dielectric>(1.0, Vec3(0.88, 0.88, 0.9), 0.1, 0.006, 0.2, 0.001);
    auto decorative_glass = std::make_shared<Dielectric>(1.5, Vec3(0.9, 0.9, 0.92), 0.1, 0.01, 0.5, 0.001);
    auto old_windshield = std::make_shared<Dielectric>(1.5, Vec3(0.95, 0.95, 0.97), 0.08, 0.006, 0.15, 0.01);
    auto glass_block = std::make_shared<Dielectric>(1.5, Vec3(0.9, 0.95, 1.0), 0.2, 0.05, 0.008, 0.5);

    auto kure_texture = std::make_shared<Texture>("Texture/kure.jpg");
    Lambertian::TextureTransform kureTransform(
        Vec2(1.0, 1.0),  // scale
        30.0,             // rotation
        Vec2(0.0, 0.0),  // translation (orijinal örnekte belirtilmemiþ)
        Vec2(1.0, 1.0),  // tiling
        WrapMode::Planar // wrap mode (varsayýlan olarak Repeat kullanýldý)
    );
    Lambertian::TextureTransform kure(Vec2(1.0, 1.0), 0.0, Vec2(1.0, 1.0), Vec2(1.0, 1.0));
    auto kure_material = std::make_shared<Lambertian>(kure_texture, 0, 0.0, kureTransform);
    kure_material->setEmission(Vec3(0.1, 0.2, 0.2), 1);
    kure_material->setSpecular(Vec3(0.5, 0.5, 0.5), 1);
   // kure_material->setTextureTransform(kure);
    auto ic_Material = std::make_shared<Lambertian>(Vec3(0.8, 0.4, 0.2), 0.1, 0.0);
    ic_Material->setEmission(Vec3(0.1, 0.1, 0.1), 0.1f); // Hafif bir emission ekleyelim
    //ic_Material->setClearcoat(0.0, 0.0);
    ic_Material->setSpecular(Vec3(1, 1, 1), 1.0f);
    //ic_Material->setAnisotropic(0.0f, Vec3(1, 0, 1)); // X yönünde %50 anizotropik
   // ic_Material->setSubsurfaceScattering(Vec3(0.8, 0.7, 0.2), 0.5f); //  subsurface scattering
//    auto kaput_Material = std::make_shared<Metal>(Vec3(0, 0, 0), 1);
    
    // Transform triangles and add to world
   // OBJ dosyalarýný ve materyallerini tanýmlayalým
    std::vector<ObjFile> obj_files = {
        {"obj/ground.obj", ground_material},
        {"car/kaput.obj", car1_Material},
        {"car/camlar.obj", clean_windshield},
        {"car/far.obj", decorative_glass},
        {"car/farici.obj", jant_Material},
        {"car/lastik.obj", lastik_Material},
        {"car/jant.obj", jant_Material},
        {"car/ic.obj", ic_Material},
        {"obj/kure.obj", kure_material},
   
       // {"obj/ground.obj", loadMaterialFromMtl("ground.mtl")},
       //{"obj/kure.obj", loadMaterialFromMtl("kure.mtl")}
        // Diðer obj dosyalarý ve materyalleri buraya eklenebilir
    };
    ObjLoaderAdapter objAdapter;
    for (const auto& obj_file : obj_files) {
        std::vector<std::unique_ptr<Triangle>> triangles = objAdapter.loadObjToTriangles(obj_file.filename);
        if (!triangles.empty()) {
            // Tüm üçgenler için ayný materyali kullan
            std::shared_ptr<Material> materialToUse = obj_file.material;

            for (auto& triangle : triangles) {
                // Orijinal UV koordinatlarýný koru
                world.add(std::make_unique<Triangle>(
                    triangle->v0, triangle->v1, triangle->v2,
                    triangle->n0, triangle->n1, triangle->n2,
                    triangle->t0, triangle->t1, triangle->t2,
                    materialToUse,
                    triangle->smoothGroup
                ));
            }
            std::cout << obj_file.filename << " yüklendi. Üçgen sayýsý: " << triangles.size() << std::endl;
        }
        else {
            std::cerr << obj_file.filename << " yüklenemedi. Diðer nesnelerle devam ediliyor." << std::endl;
        }
    }
   // Add boxes and other objects to the world and their bounds to object_bounds
   std::vector<std::pair<Vec3, double>> object_bounds;
    auto add_object = [&](std::shared_ptr<Hittable> obj, const Vec3& center, double size) {
        world.add(obj);
        object_bounds.emplace_back(center, size);
        };
    
      // Iþýk ekle
    //lights.push_back(std::make_shared<PointLight>(Vec3(6, 5, 6), Vec3(42, 41, 40),0));
   // lights.push_back(std::make_shared<PointLight>(Vec3(-3, 5, 6), Vec3(50, 50, 50), 0));
    //lights.push_back(std::make_shared<PointLight>(Vec3(0, 5, 1), Vec3(5, 2, 2),10));
    lights.push_back(std::make_shared<DirectionalLight>(Vec3(5, -8, 2), Vec3(1.55, 1.5, 1.45)));
    //lights.push_back(std::make_shared<AreaLight>(Vec3(-5, 50, -1), Vec3(-5, -1, 2), Vec3(-5, -1, 2), 1, 1, Vec3(4, 3, 3)));

    std::cout << "Total objects in the scene: " << world.size() << std::endl;

    // BVH'yi oluþtur
   
    //auto bvh = std::make_shared<BVHNode>(world.objects, 0, world.objects.size(), 0.0, 1.0);
    auto bvh = std::make_shared<ParallelBVHNode>(world.objects, 0, world.objects.size(), 0.0, 1.0);

    return std::make_pair(world, bvh);


}

void Renderer::render_chunk(int start_row, int end_row, SDL_Surface* surface, const HittableList& world,
    const std::vector<std::shared_ptr<Light>>& lights, const Vec3& background_color,
    const ParallelBVHNode* bvh, const int samples_per_pass, const int current_sample) {
    // Define camera and world
    Vec3 lookfrom(2.2, 1.2, 3.9);
    Vec3 lookat(-2.0, 0.0, -1);
    Vec3 vup(0, 1, 0);
    double vfov = 40.0;
    double aperture = 0.0;
    double dist_to_focus = 2.3;// (lookfrom - lookat).length();
    Camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus);
    ThreadLocalRNG rng;

    // Iterate over pixels in chunk
    for (int j = end_row; j >= start_row; --j) {
        for (int i = 0; i < image_width; ++i) {
            Vec3 new_color(0, 0, 0);
            // Accumulate colors from multiple samples
            for (int s = 0; s < samples_per_pass; ++s) {
                // Generate ray
                auto u = (i + rng.get()) / (image_width - 1);
                auto v = (j + rng.get()) / (image_height - 1);
                Ray r = cam.get_ray(u, v);
                // Calculate ray color
                new_color += ray_color(r, bvh, lights, background_color, MAX_DEPTH);
            }

            // Get existing color (linear space)
            std::lock_guard<std::mutex> lock(mtx);
            Uint32* pixel = static_cast<Uint32*>(surface->pixels) + (image_height - 1 - j) * surface->pitch / 4 + i;
            Uint8 r, g, b;
            SDL_GetRGB(*pixel, surface->format, &r, &g, &b);
            Vec3 existing_color(r / 255.0, g / 255.0, b / 255.0);
            existing_color = Vec3(existing_color.x * existing_color.x, existing_color.y * existing_color.y, existing_color.z * existing_color.z); // Gamma decode

            // Combine existing and new colors in linear space
            int total_samples = current_sample + samples_per_pass;
            Vec3 combined_color = (existing_color * current_sample + new_color) / total_samples;

            // Gamma correction
            combined_color = Vec3(sqrt(combined_color.x), sqrt(combined_color.y), sqrt(combined_color.z));

            // Convert color to SDL pixel format
            int ir = static_cast<int>(256 * clamp(combined_color.x, 0.0, 0.999));
            int ig = static_cast<int>(256 * clamp(combined_color.y, 0.0, 0.999));
            int ib = static_cast<int>(256 * clamp(combined_color.z, 0.0, 0.999));

            // Set pixel color in SDL surface
            *pixel = SDL_MapRGB(surface->format, ir, ig, ib);
        }
    }
}

void Renderer::render_worker(int image_height, SDL_Surface* surface, const HittableList& world,
    const std::vector<std::shared_ptr<Light>>& lights, const Vec3& background_color,
    const ParallelBVHNode* bvh, const int samples_per_pass, const int current_sample) {
    while (true) {
        int start_row = next_row.fetch_add(16); // 16 satýrlýk chunk'lar
        if (start_row >= image_height) {
            break;
        }
        int end_row = std::min(start_row + 15, image_height - 1);
        render_chunk(start_row, end_row, surface, world, lights, background_color, bvh, samples_per_pass, current_sample);
    }
}


void Renderer::update_display(SDL_Window* window, SDL_Surface* surface) {
    while (!rendering_complete) {
        SDL_UpdateWindowSurface(window);

        float progress = static_cast<float>(completed_pixels) / (image_width * image_height) * 100.0f;
       

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                rendering_complete = true;
                return;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 100ms aralýklarla güncelle
    }
}
// Yeni metod: Normal map uygulama
Vec3SIMD Renderer::apply_normal_map(const HitRecord& rec) {
    if (rec.material->has_normal_map()) {
        Vec3 tangent, bitangent;
        create_coordinate_system(rec.normal, tangent, bitangent);

        Vec3 normal_from_map = rec.material->get_normal_from_map(rec.u, rec.v);
        normal_from_map = normal_from_map * 2.0 - Vec3(1, 1, 1);
        normal_from_map *= rec.material->get_normal_strength();

        Vec3SIMD transformed_normal(
            tangent.x * normal_from_map.x + bitangent.x * normal_from_map.y + rec.normal.x * normal_from_map.z,
            tangent.y * normal_from_map.x + bitangent.y * normal_from_map.y + rec.normal.y * normal_from_map.z,
            tangent.z * normal_from_map.x + bitangent.z * normal_from_map.y + rec.normal.z * normal_from_map.z
        );

        return transformed_normal.normalize();
    }
    return Vec3SIMD(rec.normal);
}
// Yeni metod: Koordinat sistemi oluþturma
void Renderer::create_coordinate_system(const Vec3& N, Vec3& T, Vec3& B) {
    if (std::fabs(N.x) > std::fabs(N.y)) {
        T = Vec3(N.z, 0, -N.x).normalize();
    }
    else {
        T = Vec3(0, -N.z, N.y).normalize();
    }
    B = Vec3::cross(N, T);
}


Vec3SIMD Renderer::ray_color(const Ray& r, const ParallelBVHNode* bvh, const std::vector<std::shared_ptr<Light>>& lights, const Vec3SIMD& background_color, int depth) {
    Vec3SIMD final_color(0, 0, 0);
    Vec3SIMD throughput(1, 1, 1);
    Ray current_ray = r;
    float total_distance = 0.0f;
    Vec3SIMD sky_color;
    for (int bounce = 0; bounce < MAX_DEPTH; ++bounce) {
        HitRecord rec;
        if (!bvh->hit(current_ray, EPSILON, std::numeric_limits<float>::infinity(), rec)) {
            if (atmosphericEffects.enable) {
                if (background_texture) {
                    // Eðer background texture varsa, onu kullan
                    float u = 0.5f + std::atan2(current_ray.direction.z, current_ray.direction.x) / (2 * M_PI);
                    float v = 0.5f - std::asin(current_ray.direction.y) / M_PI;
                    sky_color = background_texture->get_color(u, v);
                }
                else if (!background_texture) {
                    // Yoksa background_color'ý kullan
                    sky_color = background_color;
                    final_color += throughput * atmosphericEffects.applyAtmosphericEffects(sky_color, total_distance);
                }
                else {
                    final_color += throughput * background_color;
                }
                break;
            }
        }
        // Normal map uygulamasý
        Vec3SIMD original_normal(rec.normal);
        Vec3SIMD transformed_normal = apply_normal_map(rec);
        rec.normal = static_cast<Vec3>(transformed_normal);

        float segment_distance = rec.t;
        total_distance += segment_distance;

        if (atmosphericEffects.enable) {
            // Atmosferik efektleri uygula
            throughput = atmosphericEffects.attenuateSegment(throughput, total_distance - segment_distance, total_distance);
            Vec3SIMD segment_contribution = atmosphericEffects.calculateSegmentContribution(total_distance - segment_distance, total_distance);
            final_color += throughput * segment_contribution;
        }
        else {
            final_color += throughput * sky_color;
        }

        // Malzeme iþlemleri...
        Vec3SIMD emitted = Vec3SIMD(rec.material->emitted(rec.u, rec.v, rec.point));
        final_color += throughput * emitted;

       
         if (rec.material->type() == MaterialType::Volumetric) {
            // Volumetric malzeme iþlemleri...
        }
        else {
            Vec3 attenuation;
            Ray scattered;
            if (!rec.material->scatter(current_ray, rec, attenuation, scattered)) {
                break;
            }

            if (rec.material->type() != MaterialType::Dielectric && rec.material->type() != MaterialType::Volumetric) {
                Vec3SIMD direct_light = calculate_direct_lighting(bvh, lights, rec, rec.normal);
                final_color += throughput * Vec3SIMD(attenuation) * direct_light;
            }

            // Normal'i orijinal haline geri döndür (gerekirse)
            rec.normal = static_cast<Vec3>(original_normal);
            throughput *= Vec3SIMD(attenuation);

            float p = std::max(0.1f, std::min(0.95f, throughput.max_component()));
            if (random_double() >= p) {
                break;
            }
            throughput /= p;
            current_ray = scattered;
        }
    }

    return final_color;
}
Vec3SIMD fresnelSchlick(float cosTheta, const Vec3SIMD& F0) {
    return F0 + (Vec3SIMD(1.0f) - F0) * pow(1.0f - cosTheta, 5.0f);
}

Vec3SIMD Renderer::calculate_light_contribution(const std::shared_ptr<Light>& light, const Vec3SIMD& point, const Vec3SIMD& geometric_normal, const Vec3SIMD& shading_normal, const Vec3SIMD& view_direction, float shininess,float metallic, bool is_global) {
    Vec3SIMD to_light;
    Vec3 intensity = light->intensity;
    float distance = 0.0f;
    bool is_directional = false;
    // View direction hesaplama
       

    if (auto directional_light = std::dynamic_pointer_cast<DirectionalLight>(light)) {
        to_light = -directional_light->direction.normalize();
        is_directional = true;
        distance = 1000.0f; // Assume a far distance for directional lights
    }
    else if (auto point_light = std::dynamic_pointer_cast<PointLight>(light)) {
        Vec3 light_pos = point_light->getPosition();
        Vec3 point_vec3 = point;
        to_light = light_pos - point_vec3;
        distance = to_light.length();
        to_light = to_light.normalize();
        is_directional = false;
        intensity = point_light->getIntensity() / (distance * distance);
    }
    else if (auto area_light = std::dynamic_pointer_cast<AreaLight>(light)) {
        Vec3 random_light_pos = area_light->random_point();
        Vec3 point_vec3 = Vec3(point);
        to_light = random_light_pos - point_vec3;
        distance = to_light.length();
        to_light = to_light.normalize();
        is_directional = false;
        double area = area_light->getWidth() * area_light->getHeight();
        double intensity_factor = area_light->getIntensity().length() / area;
        double attenuation = 1.0 / (distance * distance);
        intensity = area_light->getIntensity() * attenuation;
    }

    // Light atmospheric effect
    float fog_density = 0.0f;
    float haze_density = 0.0f;
    float atmospheric_attenuation = expf(-distance * (fog_density + haze_density));
    intensity = intensity * atmospheric_attenuation;

    if (!is_directional && !is_global) {
        // Additional attenuation
        float additional_attenuation = 1.0f / (1.0f + distance * 0.001f);
        intensity = intensity * additional_attenuation;
    }

    // Use the shading normal for light calculations
    __m128 cos_theta_vec = _mm_max_ps(_mm_setzero_ps(), dot(shading_normal, to_light).data);
    float cos_theta = _mm_cvtss_f32(cos_theta_vec);

    // Global illumination calculations
    if (is_global) {
        intensity = intensity * 0.1f;
    }

    // Check if the light is on the correct side of the surface
    float geometric_dot = Vec3::dot(geometric_normal, to_light);
    if (geometric_dot <= 0) {
        // Light is on the wrong side of the surface, no contribution
        return Vec3SIMD(0, 0, 0);
    }

    // Specular hesaplama
    Vec3SIMD half_vector = (to_light + view_direction).normalize();
    float NdotH = max(Vec3::dot(shading_normal, half_vector), 0.0f);
    float NdotV = max(Vec3::dot(shading_normal, view_direction), 0.0f);
    float specular_factor = pow(max(0.0f, Vec3::dot(shading_normal, half_vector)), shininess);
    Vec3SIMD specular = Vec3SIMD(intensity) * specular_factor;
    Vec3SIMD F0 = Vec3SIMD(0.04f); // Dielektrik için varsayýlan deðer
   
    Vec3SIMD F = fresnelSchlick(NdotV, F0);

   
    
    // Diffuse term
    Vec3SIMD diffuse = (Vec3SIMD(1.0f) - F) * (1.0f - metallic) * Vec3SIMD(intensity) * cos_theta;

    // Final contribution
    Vec3SIMD final_contribution = diffuse + specular;

    return (Vec3SIMD(intensity) * cos_theta) + specular;
}

Vec3SIMD Renderer::calculate_direct_lighting(const ParallelBVHNode* bvh, const std::vector<std::shared_ptr<Light>>& lights, const HitRecord& rec, const Vec3SIMD& normal) {
    Vec3SIMD direct_light(0, 0, 0);
    const Vec3SIMD& hit_point = rec.point;
    const Vec3SIMD& hit_normal = normal;  // Use the provided normal instead of rec.normal
    Vec3SIMD shading_normal = apply_normal_map(rec);
    HitRecord shadow_rec;  // Create a single HitRecord object and reuse it
    Vec3SIMD view_direction = (camera_position - hit_point).normalize();
    
    float shininess = rec.material->get_shininess();
    float metallic = rec.material->get_metallic();

    for (const auto& light : lights) {
        Vec3SIMD to_light;
        float light_distance = std::numeric_limits<float>::infinity();
        Vec3SIMD light_contribution;

        if (const auto* directional_light = dynamic_cast<const DirectionalLight*>(light.get())) {
            to_light = -directional_light->direction;
            light_contribution = calculate_light_contribution(light, hit_point, hit_normal, shading_normal, view_direction, shininess, metallic);
        }
        else if (const auto* point_light = dynamic_cast<const PointLight*>(light.get())) {
            to_light = Vec3SIMD(point_light->getPosition()) - hit_point;
            light_distance = to_light.length();
            to_light = to_light.normalize();
            light_contribution = calculate_light_contribution(light, hit_point, hit_normal, shading_normal, view_direction, shininess, metallic);
        }
        else if (const auto* area_light = dynamic_cast<const AreaLight*>(light.get())) {
            constexpr int num_samples = 16;
            const float area = area_light->getWidth() * area_light->getHeight();
            const float intensity_factor = area_light->getIntensity().length() / area;
            Vec3SIMD area_light_contribution(0, 0, 0);

            for (int i = 0; i < num_samples; ++i) {
                Vec3SIMD random_light_pos = Vec3SIMD(area_light->random_point());
                Vec3SIMD to_light = random_light_pos - hit_point;
                float light_distance = to_light.length();
                to_light = to_light.normalize();
                shadow_rec = HitRecord();  // Reset the HitRecord

                if (!bvh->hit(Ray(hit_point, to_light), EPSILON, light_distance, shadow_rec)) {
                    area_light_contribution += calculate_light_contribution(light, hit_point, hit_normal, shading_normal, view_direction, shininess, metallic)
                        * (intensity_factor / num_samples);
                }
            }
            direct_light += area_light_contribution;
            continue;  // Area light calculation is complete, skip the rest of the loop
        }
        else {
            continue;  // Unknown light type, move to the next light
        }

        shadow_rec = HitRecord();  // Reset the HitRecord
        if (!bvh->hit(Ray(hit_point, to_light), EPSILON, light_distance, shadow_rec)) {
            direct_light += light_contribution;
        }
    }

    return direct_light;
}



