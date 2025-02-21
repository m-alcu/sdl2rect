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


void Desert::draw(uint32_t *pixels, Screen screen) {

    seed1 = 0x1234;
    seed2 = 0x2293;
    uint8_t* greys = new uint8_t[screen.width * screen.high];

    uint32_t* desertPalette = new uint32_t[64];
    Desert::calcPalette(desertPalette);

    const uint8_t desertBase[40] = { 
        15,15,16,16,17,19,21,23,26,29,31,31,31,35,39,42,45,43,60,57,
        55,51,47,45,44,42,39,37,34,31,29,25,21,19,16,16,16,15,15,15  
    };

    for(int point=0; point<screen.width; point++) {
        uint8_t grey = desertBase[point % sizeof(desertBase)];
        greys[point] = grey;
        pixels[point] = desertPalette[grey];
    }

    for (int point=screen.width; point<(screen.width*screen.high - screen.width); point++) {
        uint8_t h_point = greys[point - screen.width + 1];
        uint8_t l_point = greys[point - screen.width];
        uint8_t grey = (h_point >> 1) + (h_point >> 2) + (h_point >> 3) + (l_point >> 3);

        if (seed1 >= 0x8000) {
            seed1 += seed1;
            seed1 ^= seed2;
        } else {
            seed1 += seed1;
        }
        grey += (uint8_t) ((seed1 & 0x00ff) >> 6);
        greys[point] = grey;
        pixels[point] = desertPalette[grey];
    }

    delete[] desertPalette;
    delete[] greys;

}