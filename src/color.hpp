#pragma once
#include <cstdint>

class Color {
    public:
        float blue;
        float green;
        float red;

        Color() : blue(0), green(0), red(0) {} // Default constructor initializes to black (0x00000000)
    
        // Optionally, you can add constructors or member functions if needed.
        Color(float b, float g, float r) {
            blue = b;
            green = g;
            red = r;
        }

        uint32_t toBgra() {
            return 0xff000000 | ((int) red) << 16 | ((int) green) << 8 | ((int) blue); // BGRA format
        }

        Color operator+(const Color &c) const {
            return Color(blue + c.blue, green + c.green, red + c.red);
        }
    
        Color operator-(const Color &c) const {
            return Color(blue - c.blue, green - c.green, red - c.red);
        }
    
        Color operator*(const float &rhs) const {
            return Color(blue * rhs, green * rhs, red * rhs);
        }
    
        Color operator/(const float &rhs) const {
            return Color(blue / rhs, green / rhs, red / rhs);
        }

        Color& operator+=(const Color &c) {
            red += c.red;
            green += c.green;
            blue += c.blue;
            return *this;
        }

    };