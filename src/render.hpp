#ifndef RENDER_HPP
#define RENDER_HPP

#include <iostream>
#include <cstdint>
#include "space3d.hpp"
#include "objects/tetrakis.hpp"
#include "poly.hpp"

class Render {

    public:
        Pixel proj3to2D(Vertex vertex, Screen screen);
        Pixel* projectRotateAllPoints(const Tetrakis& tetrakis, const Screen& screen, const Matrix& matrix);        
        void drawObject(const Tetrakis& tetrakis, uint32_t *pixels, Screen screen);
        void drawFace(Face face, Pixel *projectedPoints, Vertex faceNormal, Screen screen, uint32_t *pixels,  Matrix matrix);
        void drawAllFaces(const Tetrakis& tetrakis, Pixel *projectedPoints, Screen screen, uint32_t *pixels, Matrix matrix);
};

#endif
