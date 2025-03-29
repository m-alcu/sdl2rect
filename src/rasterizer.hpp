#pragma once

#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/tetrakis.hpp"
#include "objects/torus.hpp"
#include "objects/test.hpp"
#include "objects/star.hpp"
#include "scene.hpp"
#include "slib.hpp"
#include "smath.hpp"

class RGBAColor {
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
        RGBAColor(unsigned char blue, unsigned char green, unsigned char red, unsigned char alpha) {
            rgba.blue = blue;
            rgba.green = green;
            rgba.red = red;
            rgba.alpha = alpha;
        }

        RGBAColor(uint32_t value, int32_t shining) {
            bgra_value = value;
            if (shining > 0x0000ffff) { 
                bgra_value|= 0x00FFFFFF;
            } else {
                rgba.red = (uint8_t) ((rgba.red * shining) >> 16);
                rgba.green = (uint8_t) ((rgba.green * shining) >> 16);
                rgba.blue = (uint8_t) ((rgba.blue * shining) >> 16);             
            }
        }

        RGBAColor(uint32_t value) {
            bgra_value = value;
        }

    };

class Rasterizer {
    public:
        vertex p1, p2, p3;
        vertex edge12, edge23, edge13;
        uint32_t *pixels;
        float *zBuffer;
        const Solid* solid;  // Pointer to the abstract Solid
    
        // Updated constructor that also accepts a Solid pointer.
        Rasterizer(const Solid* solidPtr, uint32_t *pixelsAux, float *zBufferAux)
          : solid(solidPtr), pixels(pixelsAux), zBuffer(zBufferAux) {}
    
        void draw(const Solid& solid, Scene& scene, const Face& face, slib::vec3 faceNormal);
        bool visible();
        bool outside(Scene& scene);
        bool behind();
    
    private:
        void drawTriSector(int16_t top, int16_t bottom, vertex& left, vertex& right, vertex leftEdge, vertex rightEdge, Scene& scene, const Face& face, uint32_t flatColor, uint32_t* precomputedShading);
        void orderVertices(vertex *p1, vertex *p2, vertex *p3);
        vertex gradientDy(vertex p1, vertex p2, slib::vec3* rotatedVertices, Scene& scene, Face face);
        uint32_t phongShading(vertex gRaster, Scene& scene, Face face);
        uint32_t blinnPhongShading(vertex gRaster, Scene& scene, Face face);
        uint32_t precomputedPhongShading(vertex gRaster, Scene& scene, Face face, uint32_t* precomputedShading);
        static vertex gradientDx(const vertex &left, const vertex &right);
        void updateFromPixel(vertex &updated, const vertex &p, Scene& scene, Face face);
    };
    

