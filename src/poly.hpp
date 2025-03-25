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

class Gradient {

    public:
        int32_t p_x; // for draw triangles
        float p_z; // for z-Buffer

        slib::vec3 vertexPoint;  // for distances light to surface (light near)
        slib::vec3 vertexNormal; // for normal interpolation

        int32_t ds; //shining gradient (gouraud)

        Gradient() {            
        }

        // Prime constructor
        Gradient(int32_t p_x_in, float p_z_in, slib::vec3 p, slib::vec3 n, int32_t s) {
            p_x = p_x_in;
            p_z = p_z_in;
            vertexPoint = p;
            vertexNormal = n;
            ds = s;
        } 

        // From a pixel
        Gradient(const vertex &p, slib::vec3* rotatedVertices, slib::vec3 *normals, Scene& scene, Face face) {
            p_x = ( p.p_x << 16 ) + 0x8000;
            p_z = p.p_z;
            vertexPoint = rotatedVertices[p.vtx];
            vertexNormal = normals[p.vtx];
            float diff = std::max(0.0f, smath::dot(scene.lux,vertexNormal));
            float bright = face.material.properties.k_a+face.material.properties.k_d * diff;
            ds = (int32_t) (bright * 65536 * 4);
        }        

        // Overload operator+
        Gradient operator+(const Gradient &g) const {
            slib::vec3 p = vertexPoint + g.vertexPoint; 
            slib::vec3 n = vertexNormal + g.vertexNormal;
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
            if (shining > 0x0000ffff) { 
                bgra_value|= 0x00FFFFFF;
            } else {
                rgba.red = (uint8_t) ((rgba.red * shining) >> 16);
                rgba.green = (uint8_t) ((rgba.green * shining) >> 16);
                rgba.blue = (uint8_t) ((rgba.blue * shining) >> 16);             
            }
        }

        RGBValue(uint32_t value) {
            bgra_value = value;
        }

    };

class Rasterizer {
    public:
        vertex p1, p2, p3;
        Gradient edge12, edge23, edge13;
        uint32_t *pixels;
        float *zBuffer;
        const Solid* solid;  // Pointer to the abstract Solid
    
        // Updated constructor that also accepts a Solid pointer.
        Rasterizer(const Solid* solidPtr, uint32_t *pixelsAux, float *zBufferAux)
          : solid(solidPtr), pixels(pixelsAux), zBuffer(zBufferAux) {}
    
        void draw(const Solid& solid, Scene& scene, const Face& face, slib::vec3 faceNormal, slib::vec3 *rotatedNormals);
        bool visible();
        bool outside(Scene& scene);
        bool behind();
    
    private:
        void drawTriSector(int16_t top, int16_t bottom, Gradient& left, Gradient& right, Gradient leftEdge, Gradient rightEdge, Scene& scene, const Face& face, uint32_t flatColor, uint32_t* precomputedShading);
        void orderVertices(vertex *p1, vertex *p2, vertex *p3);
        Gradient gradientDy(vertex p1, vertex p2, slib::vec3* rotatedVertices, slib::vec3 *normals, Scene& scene, Face face);
        void swapVertex(vertex *p1, vertex *p2);
        uint32_t phongShading(Gradient gRaster, Scene& scene, Face face);
        uint32_t blinnPhongShading(Gradient gRaster, Scene& scene, Face face);
        uint32_t precomputedPhongShading(Gradient gRaster, Scene& scene, Face face, uint32_t* precomputedShading);
        static Gradient gradientDx(const Gradient &left, const Gradient &right);
        void updateFromPixel(Gradient &updated, const vertex &p, slib::vec3* rotatedVertices, slib::vec3 *normals, Scene& scene, Face face);
    };
    

