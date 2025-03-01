#include <iostream>
#include <math.h>
#include "../poly.hpp"


void Test::loadVertices() {
    const float half = 32768.f;
    const float axisDist = half * std::sqrt(3.f);  // ≈ 56755

    Test::vertices[0] = {   axisDist,  axisDist,  axisDist };
    Test::vertices[1] = {  -axisDist,  axisDist,  axisDist };
    Test::vertices[2] = {  -axisDist, -axisDist,  axisDist };
    Test::vertices[3] = {   axisDist, -axisDist,  axisDist };

    Test::vertices[4] = {   axisDist,  axisDist, -axisDist };
    Test::vertices[5] = {  -axisDist,  axisDist, -axisDist };
    Test::vertices[6] = {  -axisDist, -axisDist, -axisDist };
    Test::vertices[7] = {   axisDist, -axisDist, -axisDist };

}

void Test::loadFaces() {

    Test::faces[0].color   = 0xff0058fc;
    Test::faces[0].vertex1 = 0+4;
    Test::faces[0].vertex2 = 1+4; // wrap-around for the quad
    Test::faces[0].vertex3 = 2+4;

    Test::faces[1].color   = 0xffffffff;
    Test::faces[1].vertex1 = 0+4;
    Test::faces[1].vertex2 = 2+4; // wrap-around for the quad
    Test::faces[1].vertex3 = 3+4;

    Test::faces[2].color   = 0xff0058fc;
    Test::faces[2].vertex1 = 0;
    Test::faces[2].vertex2 = 1; // wrap-around for the quad
    Test::faces[2].vertex3 = 2;

    Test::faces[3].color   = 0xffffffff;
    Test::faces[3].vertex1 = 0;
    Test::faces[3].vertex2 = 2; // wrap-around for the quad
    Test::faces[3].vertex3 = 3;


}