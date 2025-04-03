#pragma once

#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/tetrakis.hpp"
#include "objects/torus.hpp"
#include "objects/test.hpp"
#include "objects/ascLoader.hpp"
#include "objects/objLoader.hpp"
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

struct triangle
{
    vertex p1, p2, p3;
    vertex edge12, edge23, edge13;
    int16_t i;
    triangle(const triangle& _t) : p1(_t.p1), p2(_t.p2), p3(_t.p3), i(_t.i) {};
    triangle(const vertex& _p1, const vertex& _p2, const vertex& _p3, int16_t _i) : p1(_p1), p2(_p2), p3(_p3), i(_i) {};
};

class Rasterizer {
    public:
        std::vector<std::unique_ptr<triangle>> triangles;
        uint32_t *pixels;
        float *zBuffer;
        const Solid* solid;  // Pointer to the abstract Solid
    
        // Updated constructor that also accepts a Solid pointer.
        Rasterizer(const Solid* solidPtr, uint32_t *pixelsAux, float *zBufferAux)
          : solid(solidPtr), pixels(pixelsAux), zBuffer(zBufferAux) {}
    
        void draw(triangle& tri, const Solid& solid, Scene& scene);
        bool visible(const triangle& triangle);
        bool outside(Scene& scene, const triangle& triangle);
        bool behind(const triangle& triangle);

        void addTriangle(std::unique_ptr<triangle> triangle)
        {
            triangles.push_back(std::move(triangle));
        }
    
    private:
        void drawTriHalf(int32_t top, int32_t bottom, vertex& left, vertex& right, vertex leftEdge, vertex rightEdge, Scene& scene, const Face& face, uint32_t flatColor, uint32_t* precomputedShading);
        void orderVertices(vertex *p1, vertex *p2, vertex *p3);
        vertex gradientDy(vertex p1, vertex p2, slib::vec3& lux, Face face);
        uint32_t phongShadingShader(vertex gRaster, Scene& scene, Face face);
        uint32_t blinnPhongShadingShader(vertex gRaster, Scene& scene, Face face);
        uint32_t precomputedPhongShadingShader(vertex gRaster, Scene& scene, Face face, uint32_t* precomputedShading);
        static vertex gradientDx(const vertex &left, const vertex &right);
        void update2ndVertex(vertex &updated, const vertex &p, slib::vec3& lux, Face face);
    };
    

