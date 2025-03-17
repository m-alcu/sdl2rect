#pragma once

#include <cstdint>

class Background {

    public:
        uint16_t seed1;
        uint16_t seed2;


    protected:
        // Protected virtual methods to be implemented by derived classes.
        virtual void draw(uint32_t *pixels, uint16_t high, uint16_t width) = 0;

};
