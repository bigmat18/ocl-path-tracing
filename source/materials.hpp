#pragma once

#include "math/vector.hpp"
#include "sphere.hpp"

class Material {
public:
    virtual ~Material() = default;

    virtual bool scatter(
        const Ray& ray, const HitRecord& record, 
        bmath::Vec3d& attenuation, bmath::Vec3d& scattered
    ) const {
        return false;
    }
};

class Labertian : public Material {};

class Metal : public Material {};

class Dieletric : public Material {};
