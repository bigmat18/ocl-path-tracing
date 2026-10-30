#include <memory>
#include <vector>
#include "common.hpp"
#include "hittable.hpp"
#include "materials.hpp"
#include "sphere.hpp"

bmath::Vec3d ray_color(const Ray& r, const std::vector<std::shared_ptr<Hittable>>& world, int depth = max_depth) {
    if (depth <= 0)
        return {0, 0, 0};

    HitRecord record;
    HitRecord closest_rec;
    bool hit_anything = false;
    double closest_so_far = std::numeric_limits<double>::infinity();

    for (const auto& obj : world) {
        if (obj->hit(r, {0.001, closest_so_far}, record)) {
            hit_anything = true;
            closest_so_far = record.distance;
            closest_rec = record;
        }
    }

    if (hit_anything) {
        Ray scattered;
        bmath::Vec3d attenuation;
        if (closest_rec.material && closest_rec.material->scatter(r, closest_rec, attenuation, scattered)) {
            return attenuation ^ ray_color(scattered, world, depth - 1);
        }
        return {0, 0, 0};
    }

    bmath::Vec3d unit_direction = r.direction.normalized();
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * bmath::Vec3d(1.0, 1.0, 1.0) + a * bmath::Vec3d(0.5, 0.7, 1.0);
}

int main() {
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    auto material_ground = std::make_shared<Lambertian>(bmath::Vec3d(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<Lambertian>(bmath::Vec3d(0.1, 0.2, 0.5));
    auto material_left   = std::make_shared<Dieletric>(1.0 / 1.33);
    auto material_right  = std::make_shared<Metal>(bmath::Vec3d(0.8, 0.6, 0.2), 1.0);

    std::vector<std::shared_ptr<Hittable>> world = {
        std::make_shared<Sphere>(bmath::Vec3d(-1.0,    0.0, -1.0),   0.5, material_left),
        std::make_shared<Sphere>(bmath::Vec3d( 1.0,    0.0, -1.0),   0.5, material_right),
        std::make_shared<Sphere>(bmath::Vec3d( 0.0,    0.0, -1.2),   0.5, material_center),
        std::make_shared<Sphere>(bmath::Vec3d( 0.0, -100.5, -1.0), 100.0, material_ground)
    };

    for (int j = 0; j < image_height; j++) {
        for (int i = 0; i < image_width; i++) {
            bmath::Vec3d pixel_color(0, 0, 0);
            for (int sample = 0; sample < samples_per_pixel; sample++) {
                Ray ray = get_ray(i, j);
                pixel_color += ray_color(ray, world);
            }

            pixel_color *= pixel_samples_scale;
            write_color(std::cout, pixel_color);
        }
    }
}
