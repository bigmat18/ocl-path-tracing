#pragma once

#include "math/vector.hpp"
#include <random>

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) {
    return min + (max-min)*random_double();
}

inline bmath::Vec3d random_unit_vector() {
    while(true) {
        bmath::Vec3d p = bmath::Vec3d::random(-1,1);
        auto lensq = p.length_squared();
        if (1e-160 < lensq && lensq <= 1)
            return p / sqrt(lensq);
    }
}

inline bmath::Vec3d random_on_hemisphere(const bmath::Vec3d& normal) {
    bmath::Vec3d on_unit_sphere = random_unit_vector();
    if (bmath::dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}
