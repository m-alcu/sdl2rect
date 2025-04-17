#pragma once
#include <cstdint>

class Color {
    public:
        union {
            struct {
                unsigned char blue;
                unsigned char green;
                unsigned char red;
                unsigned char alpha;
            } rgba;
            uint32_t bgra_value;
        };

        Color() : bgra_value(0) {} // Default constructor initializes to black (0x00000000)
    
        // Optionally, you can add constructors or member functions if needed.
        Color(unsigned char blue, unsigned char green, unsigned char red, unsigned char alpha) {
            rgba.blue = blue;
            rgba.green = green;
            rgba.red = red;
            rgba.alpha = alpha;
        }

        Color(uint32_t value) {
            bgra_value = value;
        }

    };