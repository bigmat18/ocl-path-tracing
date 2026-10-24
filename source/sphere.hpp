#pragma once

#include "math/vector.hpp"

struct Ray {
    bmath::Vec3d origin;
    bmath::Vec3d direction;

    bmath::Vec3d at(double t) const { return origin + t * direction; }
};

struct HitRecord {
    bmath::Vec3d point;  // impact point
    bmath::Vec3d normal; // normal on impact point
    double distance;     // distance between camera and impact
    bool front_face;     // if true the normal is positive
};

class Sphere {
    bmath::Vec3d center;
    double radius;

public:
    Sphere(const bmath::Vec3d& center, double radius)
        : center(center), radius(radius) {}

    bool hit(const Ray& ray, std::pair<double, double> limit, HitRecord& record) const {
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
        record.front_face = bmath::dot(ray.direction, outward_normal);
        record.normal = record.front_face ? outward_normal : -outward_normal;

        return true;
    }
};
