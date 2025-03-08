#ifndef RENDER_HPP
#define RENDER_HPP

#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/solid.hpp"
#include "poly.hpp"

class Render {

    public:
        Pixel proj3to2D(Vertex vertex, Vertex vertexNormal, Screen screen, Position position, Vertex lux);
        Pixel* projectRotateAllPoints(const Solid& solid, const Screen& screen, const Matrix& matrix, Position position, Vertex lux);        
        void drawObject(const Solid& solid, uint32_t *pixels, Screen screen, int64_t *zBuffer, Position position, Vertex lux, Shading shading);
        void drawFace(Face face, Pixel *projectedPoints, Vertex faceNormal, Screen screen, uint32_t *pixels,  Matrix matrix, int64_t *zBuffer, Vertex lux, Shading shading);
        void drawAllFaces(const Solid& solid, Pixel *projectedPoints, Screen screen, uint32_t *pixels, Matrix matrix, int64_t *zBuffer, Vertex lux, Shading shading);
};

#endif
