#pragma once
#include <cstdint>

class Color {
    public:
        uint16_t blue;
        uint16_t green;
        uint16_t red;

        Color() : blue(0), green(0), red(0) {} // Default constructor initializes to black (0x00000000)
    
        // Optionally, you can add constructors or member functions if needed.
        Color(uint16_t b, uint16_t g, uint16_t r) {
            blue = b;
            green = g;
            red = r;
        }

        uint32_t toScreen() {
            return 0xff000000 | red << 16 | green << 8 | blue; // BGRA format
        }

        uint32_t toScreenShifted() {
            return 0xff000000 | (red >> 8) << 16 | (green >> 8) << 8 | blue >> 8; // BGRA format
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