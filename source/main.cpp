#include <iostream>
#include "math/color.hpp"

int main() {
    constexpr int image_width = 256;
    constexpr int image_height = 256;

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        for (int i = 0; i < image_width; i++) {
            auto c = bmath::Color(
                float(i) / (image_width-1), 
                float(j) / (image_height-1), 
                0.0
            );

            std::cout << c.r() << ' ' << c.g() << ' ' << c.b() << '\n';
        }
    }
}
