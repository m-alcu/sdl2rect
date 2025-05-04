#pragma once
#include <cstdint>
#include "slib.hpp"

class Color : public slib::vec3 {
public:
    // Default constructor: black (0, 0, 0)
    Color() : slib::vec3(0.0f, 0.0f, 0.0f) {}

    // Constructor with blue, green, red components
    Color(float b, float g, float r) : slib::vec3(b, g, r) {}

    // Constructor from glm::vec3 directly
    Color(const slib::vec3& v) : slib::vec3(v) {}

    // Convert to BGRA (assumes 0–255 range)
    uint32_t toBgra() const {
        return 0xff000000 |
               ((static_cast<int>(x) & 0xFF) << 16) |
               ((static_cast<int>(y) & 0xFF) << 8) |
               ((static_cast<int>(z) & 0xFF));
    }

    float& blue()  { return x; }
    float& green() { return y; }
    float& red()   { return z; }

    const float& blue()  const { return x; }
    const float& green() const { return y; }
    const float& red()   const { return z; }
};
