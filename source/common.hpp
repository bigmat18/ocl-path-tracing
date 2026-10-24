#pragma once

#include <iostream>

#include "math/vector.hpp"
#include "random.h"

constexpr double focal_length = 2.0;
constexpr double aspect_ratio = 16.0 / 9.0;

constexpr int image_width = 1200;
constexpr int image_height = int(image_width / aspect_ratio);

constexpr double viewport_height = 10.0;
constexpr double viewport_width = viewport_height * (double(image_width) / image_height);

constexpr bmath::Vec3d camera_center(0, 0, 0);

constexpr auto viewport_u = bmath::Vec3d(viewport_width, 0, 0);
constexpr auto viewport_v = bmath::Vec3d(0, -viewport_height, 0);

constexpr auto pixel_delta_u = viewport_u / image_width;
constexpr auto pixel_delta_v = viewport_v / image_height;

constexpr auto viewport_upper_lower = camera_center - bmath::Vec3d(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
constexpr auto pixel00_loc = viewport_upper_lower + 0.5 * (pixel_delta_u + pixel_delta_v);

constexpr auto samples_per_pixel = 10;
constexpr double pixel_samples_scale = 1.0 / samples_per_pixel;

constexpr int max_depth = 10;

struct Ray {
    bmath::Vec3d origin;
    bmath::Vec3d direction;

    bmath::Vec3d at(double t) const { return origin + t * direction; }
};


inline double linear_to_gamma(double linear_component)
{
    if (linear_component > 0)
        return std::sqrt(linear_component);

    return 0;
}

void write_color(std::ostream& out, const bmath::Vec3d& pixel_color) {
    double min = 0.000;
    double max = 0.999;
    auto clamp = [&](double v) {
        if (v < min) return min;
        if (v > max) return max;
        return v;
    };

    double r = linear_to_gamma(pixel_color.x());
    double g = linear_to_gamma(pixel_color.y());
    double b = linear_to_gamma(pixel_color.z());

    int rbyte = int(256 * clamp(r));
    int gbyte = int(256 * clamp(g));
    int bbyte = int(256 * clamp(b));

    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

Ray get_ray(int i, int j) {
    auto offset = bmath::Vec3d(random_double() - 0.5, random_double() - 0.5, 0);
    auto pixel_sample = pixel00_loc +
                        ((i + offset.x()) * pixel_delta_u) +
                        ((j + offset.y()) * pixel_delta_v);
    return {camera_center, pixel_sample - camera_center};
}
