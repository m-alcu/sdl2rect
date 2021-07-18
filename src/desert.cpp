#include <iostream>
#include "poly.h"


void Desert::calcPalette(uint32_t *palette) {


    RGBValue color;
    uint16_t r_ini = 0x4980;
    uint16_t g_ini = 0x3080;
    uint16_t b_ini = 0x4980;

    uint16_t r_end = 0xa280;
    uint16_t g_end = 0x7980;
    uint16_t b_end = 0xa280;


    uint16_t r_dif = (r_end - r_ini) >> 6;
    uint16_t g_dif = (g_end - g_ini) >> 6;
    uint16_t b_dif = (b_end - b_ini) >> 6;

    uint16_t r_i = r_ini;
    uint16_t g_i = g_ini;
    uint16_t b_i = b_ini;

    for (int p=0; p<64; p++) {

        color.rgba.red = (uint8_t) ( r_i >> 8 );
        color.rgba.green = (uint8_t) ( g_i >> 8 );
        color.rgba.blue = (uint8_t) ( b_i >> 8 );
        color.rgba.alpha = 0x00;           
        palette[p] = color.long_value;
        r_i += r_dif;
        g_i += g_dif;
        b_i += b_dif;

    }

}


void Desert::draw(uint32_t *pixels, Screen screen, uint32_t *palette, uint8_t *greys) {

    seed1 = 0x1234;
    seed2 = 0x2293;
    RGBValue color;

     for (int hy=0; hy<1; hy++) {
        for(int hx=0; hx<screen.width; hx++) {
            int hxx = hx % 20;
            uint8_t out = desertBase[hxx];
            greys[hy * screen.width + hx] = out;
            uint32_t p = palette[out];
            color.long_value = p;
            pixels[hy * screen.width + hx] = p;
        }
    }

    for (int point=screen.width; point<(screen.width*screen.high - screen.width); point++) {

        uint8_t h_point = greys[point - screen.width + 1];
        uint8_t l_point = greys[point - screen.width];
        
        h_point = h_point >> 1;
        uint8_t al = h_point;
        h_point = h_point >> 1;
        al = al + h_point;
        h_point = h_point >> 1;
        al = al + h_point;
        l_point = l_point >> 3;
        al = al + l_point;

        /*
        h_point = h_point >> 1;
        uint8_t al = h_point;
        l_point = l_point >> 1;
        al = al + l_point;*/

        /*
        h_point = h_point >> 1;
        uint8_t al = h_point;
        h_point = h_point >> 1;
        al = al + h_point;
        h_point = h_point >> 1;
        al = al + h_point;
        l_point = l_point >> 3;*/

        
        if (seed1 >= 0x8000) {
            seed1 = seed1 + seed1;
            seed1 = seed1 ^ seed2;
        } else {
            seed1 = seed1 + seed1;
        }
        uint8_t extra = (uint8_t) (seed1 & 0x00ff);
        al = al + (extra >> 6);
        greys[point] = al;
        uint32_t p = palette[al];
        pixels[point] = p;
    }

}