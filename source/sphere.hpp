#pragma once

#include <memory>

#include "math/vector.hpp"
#include "common.hpp"
#include "hittable.hpp"

class Sphere : public Hittable {
    bmath::Vec3d center;
    double radius;
    std::shared_ptr<Material> material;

public:
    Sphere(const bmath::Vec3d& center, double radius, std::shared_ptr<Material> material = nullptr)
        : center(center), radius(radius), material(std::move(material)) {}

    bool hit(const Ray& ray, std::pair<double, double> limit, HitRecord& record) const override {
        bmath::Vec3d oc = center - ray.origin;
        auto a = bmath::dot(ray.direction, ray.direction);
        auto h = dot(ray.direction, oc);
        auto c = oc.length_squared() - radius*radius;
        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;   

        auto sqrtd = std::sqrt(discriminant);

        auto root = (h - sqrtd) / a;
        if (root <= limit.first || limit.second <= root) {
            root = (h + sqrtd) / a;
            if (root <= limit.first || limit.second <= root)
                return false;
        }

        record.distance = root;
        record.point = ray.at(record.distance);

        bmath::Vec3d outward_normal = (record.point - center) / radius;
        record.front_face = bmath::dot(ray.direction, outward_normal) < 0.0;
        record.normal = record.front_face ? outward_normal : -outward_normal;
        record.material = material;

       return true;
    }
};
