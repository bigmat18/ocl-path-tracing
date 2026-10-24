#pragma once

#include <memory>
#include <utility>
#include "math/vector.hpp"

class Material;
struct Ray;

struct HitRecord {
    bmath::Vec3d point;  // impact point
    bmath::Vec3d normal; // normal on impact point
    double distance;     // distance between camera and impact
    bool front_face;     // if true the normal is positive
    std::shared_ptr<Material> material;
};

class Hittable {
public:
    virtual ~Hittable() = default;

    virtual bool hit(
        const Ray& ray, 
        std::pair<double, double> limit, 
        HitRecord& record) const = 0;
};
