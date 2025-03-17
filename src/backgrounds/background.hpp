#pragma once

#include <cstdint>

class Background {

    protected:
        // Protected virtual methods to be implemented by derived classes.
        virtual void draw(uint32_t *pixels, uint16_t high, uint16_t width) = 0;

};
