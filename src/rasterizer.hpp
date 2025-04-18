#pragma once
#include <SDL2/SDL.h>
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

template<class V>
class Triangle
{
public:
    V p1, p2, p3;
    V edge12, edge23, edge13;
    int16_t i;
    Triangle(const Triangle& _t) : p1(_t.p1), p2(_t.p2), p3(_t.p3), i(_t.i) {};
    Triangle(const V& _p1, const V& _p2, const V& _p3, int16_t _i) : p1(_p1), p2(_p2), p3(_p3), i(_i) {};
};

class Rasterizer {
    public:
        std::vector<std::unique_ptr<vertex>> projectedPoints;
        std::vector<std::unique_ptr<Triangle<vertex>>> triangles;
        const Solid* solid;  // Pointer to the abstract Solid
    
        // Updated constructor that also accepts a Solid pointer.
        Rasterizer(const Solid* solidPtr)
          : solid(solidPtr) {}
    

        void ProcessVertex(const Scene& scene);
        void DrawFaces(const Scene& scene);

    private:
        void drawTriHalf(int32_t top, int32_t bottom, vertex& left, vertex& right, vertex leftEdge, vertex rightEdge, const Scene& scene, const Face& face, uint32_t flatColor);
        void orderVertices(vertex *p1, vertex *p2, vertex *p3);
        vertex gradientDy(vertex p1, vertex p2);
        uint32_t PhongPixelShading(vertex gRaster, const Scene& scene, Face face);
        uint32_t BlinnPhongPixelShading(vertex gRaster, const Scene& scene, Face face);
        void draw(Triangle<vertex>& tri, const Solid& solid, const Scene& scene);
        bool Visible(const Triangle<vertex>& triangle);
        void ClipCullDrawTriangle(Triangle<vertex>& triangle , const Scene& scene);
        float fastSpecular(float dotNH, const std::vector<float>& table);
        void addPoint(std::unique_ptr<vertex> point)
        {
            projectedPoints.push_back(std::move(point));
        }
        
    };
    

