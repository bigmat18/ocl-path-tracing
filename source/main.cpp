#include "common.hpp"

int main() {
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    std::vector<Sphere> world = {
        {{-1.0, 0.0, -1.0}, 0.5},
        {{1.0, 0.0, -1.0}, 0.5},
        {{0.0,-100.5,-1.0}, 100}
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
