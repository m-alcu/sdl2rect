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
#include "tri.hpp"

enum class ClipPlane {
    Left, Right, Bottom, Top, Near, Far
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
        uint32_t PhongPixelShading(const vertex& gRaster, const Scene& scene, const Face& face);
        uint32_t BlinnPhongPixelShading(const vertex& gRaster, const Scene& scene, const Face& face);
        void draw(Triangle<vertex>& tri, const Solid& solid, const Scene& scene);
        bool Visible(const Triangle<vertex>& triangle);
        std::vector<vertex> ClipAgainstPlane(const std::vector<vertex>& poly, ClipPlane plane, const Scene& scene);
        float ComputeAlpha(const vertex& a, const vertex& b, ClipPlane plane);
        bool IsInside(const vertex& v, ClipPlane plane);
        void ClipCullDrawTriangleSutherlandHodgman(const Triangle<vertex>& t, const Scene& scene);
        void addPoint(std::unique_ptr<vertex> point)
        {
            projectedPoints.push_back(std::move(point));
        }
        
    };
    

