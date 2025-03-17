#pragma once

#include <cstdint>
#include "background.hpp"

class Desert : public Background {

    public:
        void draw(uint32_t *pixels, uint16_t high, uint16_t width);

    private:
        void calcPalette(uint32_t *palette);
};
