#ifndef POLY_HPP
#define POLY_HPP

#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "tetrakis.hpp"

typedef struct Screen
{
    uint16_t high;
    uint16_t width;
} Screen;

typedef struct Pixel
{
    uint16_t x;
    uint16_t y;
} Pixel;

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

class Desert {

    public:
        uint16_t seed1;
        uint16_t seed2;

    public:
        void draw(uint32_t *pixels, Screen screen);

    private:
        void calcPalette(uint32_t *palette);
};

class Triangle {

    public:
        Pixel p1;
        Pixel p2;
        Pixel p3;
        uint32_t color;  // RGBA
        int32_t edge12; // 16.16
        int32_t edge23; // 16.16
        int32_t edge13; // 16.16
        Screen screen;
        uint32_t *pixels;
        Triangle(uint32_t *pixelsAux, Screen screenAux) {
            screen = screenAux;
            pixels = pixelsAux;
        }

    public:
        void draw();
        bool visible();

    private:
        void drawTriSector(uint16_t top, uint16_t bottom, int32_t *leftSide, int32_t *rightSide, uint32_t *pixels, Screen screen, int32_t leftEdge, int32_t rightEdge);
        void orderPixels(Pixel *p1, Pixel *p2, Pixel *p3);
        int32_t calculateEdge(Pixel p1, Pixel p2);
        void calculateEdges(Pixel p1, Pixel p2, Pixel p3);
        void swapPixel(Pixel *p1, Pixel *p2);

};

#endif