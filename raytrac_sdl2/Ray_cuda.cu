//#include "raytrac_sdl2.h"

/*

// Minimum ve maksimum ýþýk uzaklýklarý
const double min_distance = 0.001;
const double max_distance = 1000.0;

__device__ Vec3SIMD calculate_light_contribution(const Light* light, const Vec3SIMD& point, const Vec3SIMD& normal) {
    Vec3SIMD to_light = light->position - point;
    double distance = to_light.length();

    if (distance < min_distance) distance = min_distance;
    if (distance > max_distance) return Vec3SIMD(0, 0, 0);

    to_light = to_light.normalize();
    double cos_theta = max(0.0, Vec3::dot(normal, to_light));

    Vec3SIMD intensity = light->intensity;

    if (auto point_light = dynamic_cast<const PointLight*>(light)) {
        // Inverse square law for point lights
        intensity = intensity / (distance * distance);
    }
    else if (auto directional_light = dynamic_cast<const DirectionalLight*>(light)) {
        // Constant intensity for directional lights
        // We do nothing here as intensity is already constant
    }
    else {
        // Default to inverse square law for other light types
        intensity = intensity / (distance * distance);
    }

    // Atmospheric effect (optional)
    double atmospheric_attenuation = exp(-distance * 0.0001);
    intensity = intensity * atmospheric_attenuation;

    return intensity * cos_theta;
}

__global__ void cuda_ray_color_kernel(const Ray* rays, const Hittable** world, const Light** lights, const Vec3SIMD* background_color, Vec3SIMD* final_colors, int max_depth, int num_rays, int num_lights) {
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    if (index >= num_rays) return;

    Vec3SIMD final_color(0, 0, 0);
    Vec3SIMD throughput(1, 1, 1);
    Ray current_ray = rays[index];
    Ray scattered;
    Vec3 attenuation;

    for (int depth = 0; depth < max_depth; ++depth) {
        HitRecord rec;

        // Check for intersection with both objects and volumetric materials
        if (!world[0]->hit(current_ray, 0.001, infinity, rec)) {
            final_color += throughput * (*background_color);
            break;
        }

        if (rec.material->type() == MaterialType::Volumetric) {
            auto volumetric = std::dynamic_pointer_cast<Volumetric>(rec.material);
            if (!volumetric) {
                // Hata durumu veya iþleme devam edilemeyecek durum.
                break;
            }

            if (volumetric->scatter(current_ray, rec, attenuation, scattered)) {
                throughput *= Vec3SIMD(attenuation);
                current_ray = scattered;
                continue;
            }
            else {
                break;
            }
        }
        else {
            Vec3SIMD emitted = Vec3SIMD(rec.material->emitted(rec.u, rec.v, rec.point));
            final_color += throughput * emitted;

            if (!rec.material->scatter(current_ray, rec, attenuation, scattered)) {
                // Error condition
                break;
            }

            if (rec.material->type() != MaterialType::Dielectric && rec.material->type() != MaterialType::Volumetric) {
                Vec3SIMD direct_light(0, 0, 0);
                for (int i = 0; i < num_lights; ++i) {
                    Vec3SIMD light_contribution = calculate_light_contribution(lights[i], rec.point, rec.normal);
                    direct_light += light_contribution;
                }
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
    }

    final_colors[index] = final_color;
}

void cuda_ray_color(const Ray* rays,
    Hittable** world,  // const kaldýrýldý
    Light** lights,    // const kaldýrýldý
    const Vec3SIMD& background_color,
    Vec3SIMD* final_colors,
    int max_depth,
    int num_rays,
    int num_lights) {
    // CUDA kernel call parameters
    int threads_per_block = 256;
    int blocks_per_grid = (num_rays + threads_per_block - 1) / threads_per_block;

    // CUDA kernel function launch
    cuda_ray_color_kernel << < blocks_per_grid, threads_per_block >> > (rays, world, lights, &background_color, final_colors, max_depth, num_rays, num_lights);

    // CUDA synchronization and error checking
    cudaDeviceSynchronize();
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) {
        fprintf(stderr, "CUDA error in cuda_ray_color_kernel: %s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }
}*/
