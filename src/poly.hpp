#ifndef POLY_HPP
#define POLY_HPP

#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/tetrakis.hpp"
#include "objects/torus.hpp"
#include "objects/test.hpp"

typedef struct Screen
{
    uint16_t high;
    uint16_t width;
} Screen;

typedef struct Pixel
{
    int16_t x;
    int16_t y; 
    int64_t z; 
    float s; //shining
    int32_t u; //phong normal     16.16
    int32_t v; //phong normal     16.16
} Pixel;

class Gradient {

    public:
        int32_t dx;
        int64_t dz;
        int32_t ds; //shining gradient (gouraud)
        int32_t du; //phong normal
        int32_t dv; //phong normal

        Gradient() {            
        }

        // Prime constructor
        Gradient(int32_t x, int64_t z, int32_t s, int32_t u, int32_t v) {
            dx = x;
            dz = z;
            ds = s;
            du = u;
            dv = v;
        } 

        // From a pixel
        Gradient(const Pixel &p) {
            dx = ( p.x << 16 ) + 0x8000;
            dz = ( p.z << 32 ) + 0x80000000;
            ds = (int32_t) (p.s * 65536); //is float
        }        

        // Overload operator+
        Gradient operator+(const Gradient &g) const {
            return { dx + g.dx, dz + g.dz, ds + g.ds, du + g.du, dv + g.dv };
        }

    public:
        static Gradient computePixelStep(const Gradient &left, const Gradient &right);
};

class RGBValue {
    public:
        union {
            struct {
                unsigned char blue;
                unsigned char green;
                unsigned char red;
                unsigned char alpha;
            } rgba;
            uint32_t long_value;
        };
    
        // Optionally, you can add constructors or member functions if needed.
        RGBValue(unsigned char blue, unsigned char green, unsigned char red, unsigned char alpha) {
            rgba.blue = blue;
            rgba.green = green;
            rgba.red = red;
            rgba.alpha = alpha;
        }

        RGBValue(uint32_t value, int32_t shining) {
            long_value = value;
            rgba.red = (uint8_t) ((rgba.red * shining) >> 16);
            rgba.green = (uint8_t) ((rgba.green * shining) >> 16);
            rgba.blue = (uint8_t) ((rgba.blue * shining) >> 16);             
        }

        RGBValue(uint32_t value) {
            long_value = value;
        }

    };

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
        Shading shading;
        Gradient edge12; // dx 16.16 dz 16.16
        Gradient edge23; // dx 16.16 dz 16.16
        Gradient edge13; // dx 16.16 dz 16.16
        Screen screen;
        uint32_t *pixels;
        int64_t *zBuffer;
        Triangle(uint32_t *pixelsAux, int64_t *zBufferAux, Screen screenAux) {
            screen = screenAux;
            pixels = pixelsAux;
            zBuffer = zBufferAux;
        }

    public:
        void draw();
        bool visible();
        bool outside();
        bool behind();

    private:
        void drawTriSector(Pixel top, Pixel bottom, Gradient& left, Gradient& right, uint32_t *pixels, Screen screen, Gradient leftEdge, Gradient rightEdge);
        void orderPixels(Pixel *p1, Pixel *p2, Pixel *p3);
        Gradient calculateEdge(Pixel p1, Pixel p2);
        void calculateEdges(Pixel p1, Pixel p2, Pixel p3);
        void swapPixel(Pixel *p1, Pixel *p2);

};

#endif