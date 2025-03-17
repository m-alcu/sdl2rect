#pragma once

#include <cstdint>
#include "background.hpp"

class Imagepng : public Background {

    public:
        void draw(uint32_t *pixels, uint16_t high, uint16_t width);

};
