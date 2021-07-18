#include <iostream>
#include "poly.h"


void Desert::calcPalette(uint32_t *palette) {

    RGBValue color;
    uint16_t r_i = 0x2d80 * 4;
    uint16_t g_i = 0x2380 * 4;
    uint16_t b_i = 0x2d80 * 4;

    uint16_t r_dif = 0x0074 * 4;
    uint16_t g_dif = 0x0064 * 4;
    uint16_t b_dif = 0x0074 * 4;

    for (int p=0; p<64; p++) {
        color.rgba.red = (uint8_t) ( r_i >> 8 );
        color.rgba.green = (uint8_t) ( g_i >> 8 );
        color.rgba.blue = (uint8_t) ( b_i >> 8 );
        color.rgba.alpha = 0x00;           
        palette[p] = color.long_value;
        r_i -= r_dif;
        g_i -= g_dif;
        b_i -= b_dif;
    }
}


void Desert::draw(uint32_t *pixels, Screen screen, uint32_t *palette, uint8_t *greys) {

    seed1 = 0x1234;
    seed2 = 0x2293;

    for(int point=0; point<screen.width; point++) {
        uint8_t out = desertBase[point % sizeof(desertBase)];
        greys[point] = out;
        pixels[point] = palette[out];
    }

    for (int point=screen.width; point<(screen.width*screen.high - screen.width); point++) {
        uint8_t h_point = greys[point - screen.width + 1];
        uint8_t l_point = greys[point - screen.width];

        h_point = h_point >> 1;
        uint8_t grey = h_point;
        h_point = h_point >> 1;
        grey += h_point;
        h_point = h_point >> 1;
        grey += h_point;
        l_point = l_point >> 3;
        grey += l_point;

        if (seed1 >= 0x8000) {
            seed1 += seed1;
            seed1 = seed1 ^ seed2;
        } else {
            seed1 += seed1;
        }
        grey += (uint8_t) ((seed1 & 0x00ff) >> 6);
        greys[point] = grey;
        pixels[point] = palette[grey];
    }

}