#include "renderer.h"




Renderer::Renderer(int image_width, int image_height, int samples_per_pixel, int max_depth)
    : image_width(image_width), image_height(image_height), aspect_ratio(static_cast<double>(image_width) / image_height) {}

Renderer::~Renderer() {}

void Renderer::render_image(SDL_Surface* surface, SDL_Window* window) {
    unsigned int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    std::cout << "Starting render with " << num_threads << " threads" << std::endl;

    auto start_time = std::chrono::steady_clock::now();

    Vec3 background_color;
    std::vector<std::shared_ptr<Light>> lights;

    auto [world, bvh] = create_scene(lights, background_color);
    auto create_scene_end_time = std::chrono::steady_clock::now();

    std::thread display_thread(&Renderer::update_display, this, window, surface);

    for (unsigned int t = 0; t < num_threads; ++t) {
        threads.emplace_back(&Renderer::render_worker, this, image_height, surface, std::ref(world), std::ref(lights), background_color, bvh.get());
    }

    for (auto& thread : threads) {
        thread.join();
    }

    rendering_complete = true;
    display_thread.join();

    SDL_UpdateWindowSurface(window);

    std::cout << "\nRender completed." << std::endl;

    auto render_end_time = std::chrono::steady_clock::now();

    auto create_scene_duration = std::chrono::duration<double, std::milli>(create_scene_end_time - start_time);
    auto render_duration = std::chrono::duration<double, std::milli>(render_end_time - create_scene_end_time);
    auto total_duration = std::chrono::duration<double, std::milli>(render_end_time - start_time);

    std::cout << "Create Scene Duration: " << create_scene_duration.count() / 1000 << " seconds" << std::endl;
    std::cout << "Render Duration: " << render_duration.count() / 1000 << " seconds" << std::endl;
    std::cout << "Total Duration: " << total_duration.count() / 1000 << " seconds" << std::endl;
}

std::pair<HittableList, std::shared_ptr<BVHNode>> Renderer::create_scene(std::vector<std::shared_ptr<Light>>& lights, Vec3& background_color) {
    HittableList world;
    background_color = Vec3(0.1, 0.12, 0.15);
    Vec3 v0, v1, v2;
    // Load OBJ file and add triangles to world
    //std::vector<ObjLoader::ObjTriangle> triangles;
      // Create a Volumetric material instance
    auto material_volume = std::make_shared<Volumetric>
        (
            Vec3(0.8, 0.8, 0.8),//albedo deðeri
            2,              //Yoðunluk (density) deðeri
            0.5,           //emilim olasýlýk (absorption_probability) deðeri
            Vec3(-2.0, 0.5, -1.0),
            10
        );

    // Transform triangles and add to world
    std::vector<ObjLoader::ObjTriangle> triangles;
    bool obj_loaded = ObjLoader::Loader::loadObj("111.obj", triangles);

    if (obj_loaded) {
        for (const auto& objTriangle : triangles) {
            auto triangle_material = std::make_shared<Lambertian>(Vec3(0.8, 0.8, 0.8));
            // Apply transformation matrix
            // ...
            Vec3 v0(objTriangle.v1.x, objTriangle.v1.y, objTriangle.v1.z);
            Vec3 v1(objTriangle.v2.x, objTriangle.v2.y, objTriangle.v2.z);
            Vec3 v2(objTriangle.v3.x, objTriangle.v3.y, objTriangle.v3.z);

            auto tri = std::make_shared<Triangle>(v0, v1, v2, triangle_material);
            Matrix4x4 scale = Matrix4x4::scaling(Vec3(4.2, 4.2, 4.2));
            Matrix4x4 translate = Matrix4x4::translation(Vec3(0, -0.5, -3));
            tri->set_transform(translate * scale);
            world.add(tri);
        }
        std::cout << "OBJ file loaded successfully. Number of triangles: " << triangles.size() << std::endl;
    }
    else {
        std::cerr << "Failed to load OBJ file. Continuing with other objects." << std::endl;
    }

    // Add other objects to world   
  

    //auto material_ground = std::make_shared<Lambertian>(Vec3(0.4, 0.4, 0.4));
    //auto material_center = std::make_shared<Lambertian>(Vec3(0.1, 0.2, 0.5));
    //auto material_left = std::make_shared<Dielectric>(1.5);
    //auto material_right = std::make_shared<Metal>(Vec3(0.8, 0.6, 0.2), 0.0);
    //auto material_back = std::make_shared<Metal>(Vec3(1.0, 1.0, 0.0), 0.0);
    // auto material_volume = std::make_shared<Volumetric>(Vec3(1.0, 1.0, 1.0), 0.1);
   // auto material_box1 = std::make_shared<Lambertian>(Vec3(0.4, 0.6, 0.6));

    // Add boxes and other objects to the world and their bounds to object_bounds
    std::vector<std::pair<Vec3, double>> object_bounds;

    auto add_object = [&](std::shared_ptr<Hittable> obj, const Vec3& center, double size) {
        world.add(obj);
        object_bounds.emplace_back(center, size);
        };
    // Nesneleri ekleyin
   // add_object(std::make_shared<Box>(Vec3(-2.0, 0.0, -2.0), 1.0, material_volume), Vec3(-2.0, 0.0, -1.0), 1.0);
    //add_object(std::make_shared<Box>(Vec3(2.0, 0.0, -1.0), 1.0, material_box1), Vec3(2.0, 0.0, -1.0), 1.0);
    //add_object(std::make_shared<Box>(Vec3(2.0, 0.0, -5.0), 1.0, material_box1), Vec3(2.0, 0.0, -5.0), 1.0);
    //add_object(std::make_shared<Box>(Vec3(-2.0, 0.0, -5.0), 1.0, material_box1), Vec3(-2.0, 0.0, -5.0), 1.0);
    //add_object(std::make_shared<Sphere>(Vec3(0.0, 0.0, -1.0), 0.5, material_center), Vec3(0.0, 0.0, -1.0), 0.5);
    //add_object(std::make_shared<Sphere>(Vec3(0.0, 0.4, -5.0), 1, material_back), Vec3(0.0, 0.4, -5.0), 1.0);
    //add_object(std::make_shared<Sphere>(Vec3(1.5, 0.0, -1.0), 0.5, material_left), Vec3(-1.0, 0.0, -1.0), 0.5);
    //add_object(std::make_shared<Sphere>(Vec3(2.5, 0.0, -1.0), 0.5, material_right), Vec3(1.0, 0.0, -1.0), 0.5);

      // Iþýk ekle
    //lights.push_back(std::make_shared<PointLight>(Vec3(-3, 2, -3), Vec3(1, 1, 0.5),0));
    //lights.push_back(std::make_shared<PointLight>(Vec3(3, 2, 1), Vec3(1, 1, 3.5), 0));
    //lights.push_back(std::make_shared<PointLight>(Vec3(0, 5, 1), Vec3(5, 2, 2),10));
    lights.push_back(std::make_shared<DirectionalLight>(Vec3(-6, -5, 0), Vec3(1.1, 1.1, 1)));
    //lights.push_back(std::make_shared<AreaLight>(Vec3(5, 5, -1), Vec3(1, -1, 2), Vec3(0, -1, 2), 5, 5, Vec3(40, 20, 20)));

    std::cout << "Total objects in the scene: " << world.size() << std::endl;

    // BVH'yi oluþtur
    auto bvh = std::make_shared<BVHNode>(world.objects, 0, world.objects.size(), 0.0, 1.0);

    return { world, bvh };


}

void Renderer::render_chunk(int start_row, int end_row, SDL_Surface* surface, const HittableList& world, const std::vector<std::shared_ptr<Light>>& lights, const Vec3& background_color, const BVHNode* bvh) {

    //auto bvh = build_bvh(world.objects, 0.0, 1.0);

    // Set up rendering parameters
    const int samples_per_pixel = 5;

    //Vec3 background_color(0.1, 0.1, 0.12);
    // Define camera and world
    Vec3 lookfrom(0, 0.1, 3.8);
    Vec3 lookat(0, 0.5, -3);
    Vec3 vup(0, 1, 0);
    double vfov = 40.0;
    double aperture = 0.05;
    double dist_to_focus = (lookfrom - lookat).length();
    Camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus);
    ThreadLocalRNG rng;

    // Iterate over pixels in chunk
    for (int j = end_row; j >= start_row; --j) {
        for (int i = 0; i < image_width; ++i) {
            Vec3 color(0, 0, 0);

            // Accumulate colors from multiple samples
            for (int s = 0; s < samples_per_pixel; ++s) {
                // Generate ray
                auto u = (i + rng.get()) / (image_width - 1);
                auto v = (j + rng.get()) / (image_height - 1);
                Ray r = cam.get_ray(u, v);

                // Calculate ray color
                color += ray_color(r, bvh, lights, background_color, MAX_DEPTH);
            }

            // Average color and gamma correction
            color /= samples_per_pixel;
            color = Vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));

            // Convert color to SDL pixel format
            int ir = static_cast<int>(256 * clamp(color.x, 0.0, 0.999));
            int ig = static_cast<int>(256 * clamp(color.y, 0.0, 0.999));
            int ib = static_cast<int>(256 * clamp(color.z, 0.0, 0.999));

            // Set pixel color in SDL surface
            std::lock_guard<std::mutex> lock(mtx);
            Uint32* pixel = static_cast<Uint32*>(surface->pixels) + (image_height - 1 - j) * surface->pitch / 4 + i;
            *pixel = SDL_MapRGB(surface->format, ir, ig, ib);
        }
    }
}

void Renderer::render_worker(int image_height, SDL_Surface* surface, const HittableList& world, const std::vector<std::shared_ptr<Light>>& lights, const Vec3& background_color, const BVHNode* bvh) {

    while (true) {
        int start_row = next_row.fetch_add(16); // 16 satýrlýk chunk'lar
        if (start_row >= image_height) {
            break;
        }
        int end_row = std::min(start_row + 15, image_height - 1);
        render_chunk(start_row, end_row, surface, world, lights, background_color, bvh);
    }
}

void Renderer::update_display(SDL_Window* window, SDL_Surface* surface) {
    while (!rendering_complete) {
        SDL_UpdateWindowSurface(window);

        float progress = static_cast<float>(completed_pixels) / (image_width * image_height) * 100.0f;
        std::cout << "\rRendering progress: " << std::fixed << std::setprecision(2) << progress << "%" << std::flush;

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

Vec3SIMD Renderer::ray_color(const Ray& r, const BVHNode* bvh, const std::vector<std::shared_ptr<Light>>& lights, const Vec3SIMD& background_color, int depth) {
    // Implementation of ray_color function
    Vec3SIMD final_color(0, 0, 0);
    Vec3SIMD throughput(1, 1, 1);
    Ray current_ray = r;

    for (int bounce = 0; bounce < MAX_DEPTH; ++bounce) {
        HitRecord rec;
        if (!bvh->hit(current_ray, EPSILON, std::numeric_limits<float>::infinity(), rec)) {
            final_color += throughput * background_color;
            break;
        }

        Vec3SIMD emitted = Vec3SIMD(rec.material->emitted(rec.u, rec.v, rec.point));
        final_color += throughput * emitted;

        if (rec.material->type() == MaterialType::Volumetric) {
            auto volumetric = std::dynamic_pointer_cast<Volumetric>(rec.material);
            if (!volumetric) {
                std::cerr << "Failed to cast to Volumetric material" << std::endl;
                break;
            }

            Vec3 attenuation;
            Ray scattered;
            if (volumetric->scatter(current_ray, rec, attenuation, scattered)) {
                throughput *= Vec3SIMD(attenuation);
                current_ray = scattered;
                continue;
            }
            else {
                break;
            }
        }

        Vec3 attenuation;
        Ray scattered;
        if (!rec.material->scatter(current_ray, rec, attenuation, scattered)) {
            break;
        }

        if (rec.material->type() != MaterialType::Dielectric && rec.material->type() != MaterialType::Volumetric) {
            Vec3SIMD direct_light = calculate_direct_lighting(bvh, lights, rec);
            final_color += throughput * Vec3SIMD(attenuation) * direct_light;
        }

        throughput *= Vec3SIMD(attenuation);

        float p = std::max(0.1f, std::min(0.95f, throughput.max_component()));
        if (random_double() >= p) {
            break;
        }
        throughput /= p;

        current_ray = scattered;
    }

    return final_color;
}

Vec3SIMD Renderer::calculate_light_contribution(const std::shared_ptr<Light>& light, const Vec3SIMD& point, const Vec3SIMD& normal, bool is_global ) {
    Vec3SIMD to_light;
    Vec3 intensity = light->intensity;
    float distance = 0.0f;
    bool is_directional = false;

    if (auto directional_light = std::dynamic_pointer_cast<DirectionalLight>(light)) {
        to_light = -directional_light->direction.normalize();
        is_directional = true;
        distance = 1000.0f; // Bu deðeri projenize göre ayarlayabilirsiniz
    }

 else if (auto point_light = std::dynamic_pointer_cast<PointLight>(light)) {
     Vec3 light_pos = point_light->getPosition();  // Vec3SIMD'den Vec3'e dönüþüm
     Vec3 point_vec3 = point;  // Vec3SIMD'den Vec3'e dönüþüm
     to_light = light_pos - point_vec3;

     distance = to_light.length();
     to_light = to_light.normalize();
     is_directional = false;

     // Intensity hesaplamasý
     intensity = point_light->getIntensity() / (distance * distance);
     }
 else if (auto area_light = std::dynamic_pointer_cast<AreaLight>(light)) {
        Vec3 random_light_pos = area_light->random_point();  // Alan ýþýðýndan rastgele bir nokta al
        Vec3 point_vec3 = Vec3(point);  // Vec3SIMD'den Vec3'e dönüþüm
        to_light = random_light_pos - point_vec3;

        distance = to_light.length();
        to_light = to_light.normalize();
        is_directional = false;

        // Area light için ek hesaplamalar
        double area = area_light->getWidth() * area_light->getHeight();
        double intensity_factor = area_light->getIntensity().length() / area;
        double attenuation = 1.0 / (distance * distance);
        intensity = area_light->getIntensity() * attenuation;
    }

    // Atmosferik etki (hem directional hem de diðer ýþýklar için)
    float atmospheric_attenuation = expf(-distance * 0.00001f);
    intensity = intensity * atmospheric_attenuation;

    if (!is_directional && !is_global) {
        // Buraya ek zayýflama ekleyebilirsiniz, eðer global aydýnlatma için kullanýlmýyorsa
    }

    __m128 cos_theta_vec = _mm_max_ps(_mm_setzero_ps(), dot(normal, to_light).data);
    float cos_theta = _mm_cvtss_f32(cos_theta_vec);

    // Global aydýnlatma için ek hesaplamalar
    if (is_global) {
        // Burada global aydýnlatma için ek hesaplamalar yapabilirsiniz
        // Örneðin, yansýma faktörü ekleyebilir veya yoðunluðu azaltabilirsiniz
        intensity = intensity * 0.5f; // Global aydýnlatma için yoðunluðu azaltma örneði
    }

    return intensity * cos_theta;
}



Vec3SIMD Renderer::calculate_direct_lighting(const BVHNode* bvh, const std::vector<std::shared_ptr<Light>>& lights, const HitRecord& rec) {
    Vec3SIMD direct_light(0, 0, 0);
    const Vec3SIMD& hit_point = rec.point;
    const Vec3SIMD& hit_normal = rec.normal;

    HitRecord shadow_rec;  // Tek bir HitRecord nesnesi oluþtur ve yeniden kullan

    for (const auto& light : lights) {
        Vec3SIMD to_light;
        float light_distance = std::numeric_limits<float>::infinity();
        Vec3SIMD light_contribution;

        if (const auto* directional_light = dynamic_cast<const DirectionalLight*>(light.get())) {
            to_light = -directional_light->direction;
            light_contribution = calculate_light_contribution(light, hit_point, hit_normal);
        }
        else if (const auto* point_light = dynamic_cast<const PointLight*>(light.get())) {
            to_light = Vec3SIMD(point_light->getPosition()) - hit_point;
            light_distance = to_light.length();
            to_light = to_light.normalize();
            light_contribution = calculate_light_contribution(light, hit_point, hit_normal);
        }
        else if (const auto* area_light = dynamic_cast<const AreaLight*>(light.get())) {
            constexpr int num_samples = 16;
            const float area = area_light->getWidth() * area_light->getHeight();
            const float intensity_factor = area_light->getIntensity().length() / area;
            Vec3SIMD area_light_contribution(0, 0, 0);

            for (int i = 0; i < num_samples; ++i) {
                // Rastgele bir nokta seç ve bu noktaya doðru ýþýn gönder
                Vec3SIMD random_light_pos = Vec3SIMD(area_light->random_point());
                Vec3SIMD to_light = random_light_pos - hit_point;
                float light_distance = to_light.length();
                to_light = to_light.normalize();

                shadow_rec = HitRecord();  // HitRecord'u sýfýrla
                if (!bvh->hit(Ray(hit_point, to_light), EPSILON, light_distance, shadow_rec)) {
                    // Iþýk katkýsýný hesapla ve ortalama al
                    area_light_contribution += calculate_light_contribution(light, hit_point, hit_normal)
                        * (intensity_factor / num_samples);
                }
            }
            direct_light += area_light_contribution;
            continue;  // Area light hesaplamasý tamamlandý, döngünün geri kalanýný atla
        }
        else {
            continue;  // Bilinmeyen ýþýk tipi, bir sonraki ýþýða geç
        }

        shadow_rec = HitRecord();  // HitRecord'u sýfýrla
        if (!bvh->hit(Ray(hit_point, to_light), EPSILON, light_distance, shadow_rec)) {
            direct_light += light_contribution;
        }
    }
    return direct_light;
}



