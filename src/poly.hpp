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
    int16_t vtx;
} Pixel;

class Gradient {

    public:
        int32_t p_x; // for draw triangles
        int64_t p_z; // for z-Buffer

        Vertex vertexPoint;  // for distances light to surface (light near)
        Vertex vertexNormal; // for normal interpolation

        int32_t ds; //shining gradient (gouraud)

        Gradient() {            
        }

        // Prime constructor
        Gradient(int32_t p_x_in, int64_t p_z_in, Vertex p, Vertex n, int32_t s) {
            p_x = p_x_in;
            p_z = p_z_in;
            vertexPoint = p;
            vertexNormal = n;
            ds = s;
        } 

        // From a pixel
        Gradient(const Pixel &p, const Solid& solid, Vertex lux) {
            p_x = ( p.p_x << 16 ) + 0x8000;
            p_z = p.p_z;
            vertexPoint = solid.vertices[p.vtx];
            vertexNormal = solid.vertexNormals[p.vtx];
            ds = (int32_t) (std::max(0.0f,(lux.dot(vertexNormal))) * 65536);
        }        

        // Overload operator+
        Gradient operator+(const Gradient &g) const {
            Vertex p = vertexPoint + g.vertexPoint; 
            Vertex n = vertexNormal + g.vertexNormal;
            return { p_x + g.p_x, p_z + g.p_z, p, n, ds + g.ds };
        }

        Gradient& operator+=(const Gradient &g) {
            p_x += g.p_x;
            p_z += g.p_z;
            vertexPoint += g.vertexPoint;
            vertexNormal += g.vertexNormal;
            ds += g.ds;
            return *this;
        }        

    public:
        static Gradient gradientDx(const Gradient &left, const Gradient &right);
        void updateFromPixel(const Pixel &p, const Solid& solid, Vertex lux);
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
        Gradient gradientDy(Pixel p1, Pixel p2, const Solid& solid, Vertex lux);
        void swapPixel(Pixel *p1, Pixel *p2);
    };
    

#endif