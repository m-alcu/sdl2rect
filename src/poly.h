#include <iostream>

typedef struct Screen
{
    uint16_t high;
    uint16_t width;
} Screen;


class Rectangle {
    public:
        void draw(uint32_t *pixels, Screen screen, int y_start, int y_end, int x_start, int x_end, uint32_t color);
};