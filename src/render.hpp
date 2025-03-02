#ifndef RENDER_HPP
#define RENDER_HPP

#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/solid.hpp"
#include "poly.hpp"

class Render {

    public:
        Pixel proj3to2D(Vertex vertex, Screen screen, Position position);
        Pixel* projectRotateAllPoints(const Solid& solid, const Screen& screen, const Matrix& matrix, Position position);        
        void drawObject(const Solid& solid, uint32_t *pixels, Screen screen, int32_t *zBuffer, Position position);
        void drawFace(Face face, Pixel *projectedPoints, Vertex faceNormal, Screen screen, uint32_t *pixels,  Matrix matrix, int32_t *zBuffer);
        void drawAllFaces(const Solid& solid, Pixel *projectedPoints, Screen screen, uint32_t *pixels, Matrix matrix, int32_t *zBuffer);
};

#endif
