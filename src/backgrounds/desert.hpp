#pragma once

#include <cstdint>
#include "background.hpp"

class Desert : public Background {

    public:
        uint16_t seed1;
        uint16_t seed2;

    public:
        void draw(uint32_t *pixels, uint16_t height, uint16_t width);

    private:
        void calcPalette(uint32_t *palette);
};
