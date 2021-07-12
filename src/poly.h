#include <iostream>

typedef struct Screen
{
    uint16_t high;
    uint16_t width;
} Screen;

typedef union
{
    struct
    {
        unsigned char blue;
        unsigned char green;
        unsigned char red;
        unsigned char alpha;
    } rgba;

    uint32_t long_value;
} RGBValue;


class Rectangle {
    public:
        void draw(uint32_t *pixels, Screen screen, int y_start, int y_end, int x_start, int x_end, uint32_t color);

    public:
        void randomDraw(uint32_t *pixels, Screen screen);

};