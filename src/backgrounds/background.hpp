#pragma once

#include <cstdint>
#include <memory>

class Background {

    public:
        // Protected virtual methods to be implemented by derived classes.
        virtual void draw(uint32_t *pixels, uint16_t high, uint16_t width) = 0;

        virtual ~Background() {}

};
