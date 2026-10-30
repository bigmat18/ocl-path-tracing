#pragma once

#include "common.hpp"
#include "math/vector.hpp"
#include "hittable.hpp"
#include "random.h"

template<typename T, std::size_t N>
inline bmath::Vector<T, N> reflect(const bmath::Vector<T, N>& v, const bmath::Vector<T, N>& n) {
    return bmath::Vector<T, N>(v - n * (static_cast<T>(2) * bmath::dot(v, n)));
}

namespace bmath {
    using ::reflect;
}

class Material {
public:
    virtual ~Material() = default;

    virtual bool scatter(
        const Ray& ray, const HitRecord& record, 
        bmath::Vec3d& attenuation, Ray& scattered
    ) const = 0;
};

class Labertian : public Material {
    bmath::Vec3d albedo;

public:
    Labertian(const bmath::Vec3d& albedo) : albedo(albedo) {}

    virtual bool scatter(
        const Ray& ray, const HitRecord& record, 
        bmath::Vec3d& attenuation, Ray& scattered
    ) const override {
        auto direction = record.normal + random_unit_vector();
        if (direction.near_zero())
            direction = record.normal;

        scattered = Ray(record.point, direction);
        attenuation = albedo;
        return true;
    }
};

using Lambertian = Labertian;

class Metal : public Material {
    bmath::Vec3d albedo;
    double fuzz;

public:
    Metal(const bmath::Vec3d& albedo, double fuzz = 0.0)
        : albedo(albedo), fuzz(fuzz < 1.0 ? fuzz : 1.0) {}

    virtual bool scatter(
        const Ray& ray, const HitRecord& record, 
        bmath::Vec3d& attenuation, Ray& scattered
    ) const override {
        bmath::Vec3d reflected = reflect(ray.direction.normalized(), record.normal);
        scattered = Ray(record.point, reflected + (fuzz * random_unit_vector()));
        attenuation = albedo;
        return (bmath::dot(scattered.direction, record.normal) > 0.0);
    }
};

class Dieletric : public Material {
    double refraction_index;

public:
    Dieletric(double refraction_index) : refraction_index(refraction_index) {};

    virtual bool scatter(
        const Ray& ray, const HitRecord& record, 
        bmath::Vec3d& attenuation, Ray& scattered
    ) const override {
        attenuation = bmath::Vec3d(1.0, 1.0, 1.0);
        double ri = record.front_face ? (1.0/refraction_index) : refraction_index;
        auto unit_direction = ray.direction.normalized();
        double cos_theta = std::fmin(bmath::dot(-unit_direction, record.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        bmath::Vec3d direction;

        if (cannot_refract) {
            direction = reflect(unit_direction, record.normal);
        } else {
            direction = refract(unit_direction, record.normal, ri);
        }


        scattered = Ray(record.point, direction);
        return true;

    }

private:
    inline bmath::Vec3d refract(const bmath::Vec3d& uv, const bmath::Vec3d& n, double etai_over_etat) const {
        auto cos_theta = std::fmin(bmath::dot(-uv, n), 1.0);
        bmath::Vec3d r_out_perp =  etai_over_etat * (uv + cos_theta*n);
        bmath::Vec3d r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
        return r_out_perp + r_out_parallel;
    }

};
