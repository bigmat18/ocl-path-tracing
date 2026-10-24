#pragma once

#include "common.hpp"
#include "math/vector.hpp"
#include "hittable.hpp"
#include "random.h"

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
        bmath::Vec3d reflected = bmath::reflect(ray.direction.normalized(), record.normal);
        scattered = Ray(record.point, reflected + (fuzz * random_unit_vector()));
        attenuation = albedo;
        return (bmath::dot(scattered.direction, record.normal) > 0.0);
    }
};

class Dieletric : public Material {};
