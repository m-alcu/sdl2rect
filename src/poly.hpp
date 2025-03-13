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
    int16_t p_x;
    int16_t p_y;
    int64_t p_z; 
    float s; //shining

    int16_t vtx;
} Pixel;

class Gradient {

    public:
        int32_t p_x;

        int64_t v_x;
        int64_t v_y;
        int64_t v_z;

        Vertex vertexNormal;

        int32_t ds; //shining gradient (gouraud)

        Gradient() {            
        }

        // Prime constructor
        Gradient(int32_t p_x_in, int64_t v_x_in, int64_t v_y_in, int64_t v_z_in, int32_t s) {
            p_x = p_x_in;
            v_x = v_x_in;
            v_y = v_y_in;
            v_z = v_z_in;
            ds = s;
        } 

        // From a pixel
        Gradient(const Pixel &p, const Solid& solid, Vertex lux) {
            p_x = ( p.p_x << 16 ) + 0x8000;
            v_z = p.p_z;
            ds = (int32_t) (p.s * 65536); //is float
        }        

        // Overload operator+
        Gradient operator+(const Gradient &g) const {
            return { p_x + g.p_x, v_x + g.v_x, v_y + g.v_y, v_z + g.v_z, ds + g.ds };
        }

    public:
        static Gradient computePixelStep(const Gradient &left, const Gradient &right);
        void update(const Pixel &p, const Solid& solid, Vertex lux);
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
            uint32_t bgra_value;
        };
    
        // Optionally, you can add constructors or member functions if needed.
        RGBValue(unsigned char blue, unsigned char green, unsigned char red, unsigned char alpha) {
            rgba.blue = blue;
            rgba.green = green;
            rgba.red = red;
            rgba.alpha = alpha;
        }

        RGBValue(uint32_t value, int32_t shining) {
            bgra_value = value;
            rgba.red = (uint8_t) ((rgba.red * shining) >> 16);
            rgba.green = (uint8_t) ((rgba.green * shining) >> 16);
            rgba.blue = (uint8_t) ((rgba.blue * shining) >> 16);             
        }

        RGBValue(uint32_t value) {
            bgra_value = value;
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
        Pixel p1, p2, p3;
        uint32_t color;  // RGBA
        Shading shading;
        Gradient edge12, edge23, edge13;
        Screen screen;
        uint32_t *pixels;
        int64_t *zBuffer;
        const Solid* solid;  // Pointer to the abstract Solid
    
        // Updated constructor that also accepts a Solid pointer.
        Triangle(const Solid* solidPtr, uint32_t *pixelsAux, int64_t *zBufferAux, Screen screenAux)
          : solid(solidPtr), pixels(pixelsAux), zBuffer(zBufferAux), screen(screenAux) {}
    
        void draw(const Solid& solid, Vertex lux);
        bool visible();
        bool outside();
        bool behind();
    
    private:
        void drawTriSector(Pixel top, Pixel bottom, Gradient& left, Gradient& right, uint32_t *pixels, Screen screen, Gradient leftEdge, Gradient rightEdge);
        void orderPixels(Pixel *p1, Pixel *p2, Pixel *p3);
        Gradient calculateEdge(Pixel p1, Pixel p2, const Solid& solid, Vertex lux);
        void calculateEdges(Pixel p1, Pixel p2, Pixel p3, const Solid& solid, Vertex lux);
        void swapPixel(Pixel *p1, Pixel *p2);
    };
    

#endif