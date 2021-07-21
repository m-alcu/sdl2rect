#include <iostream>
#include "poly.h"

const double rawVertices[42] = { //14*3
    32768, 32768, 32768,
    32768, 32768,-32768,
    32768,-32768, 32768,
    32768,-32768,-32768,
   -32768, 32768, 32768,
   -32768, 32768,-32768,
   -32768,-32768, 32768,
   -32768,-32768,-32768,
    56755,     0,     0,
        0, 56755,     0,
        0,     0, 56755,
   -56755,     0,     0,
        0,-56755,     0,
        0,     0,-56755
};

const uint8_t rawFaces[96] = { //24 faces , color + *p1 + *p2 + *p3
    2, 2, 0, 8,
    3, 0, 1, 8,
    2, 1, 3, 8,
    3, 3, 2, 8,

    3, 5, 1, 9,
    2, 1, 0, 9,
    3, 0, 4, 9,
    2, 4, 5, 9,

    3, 6, 4,10,
    2, 4, 0,10,
    3, 0, 2,10,
    2, 2, 6,10,
    
    2, 4, 6,11,
    3, 6, 7,11,
    2, 7, 5,11,
    3, 5, 4,11,

    2, 7, 6,12,
    3, 6, 2,12,
    2, 2, 3,12,
    3, 3, 7,12,

    2, 1, 5,13,
    3, 5, 7,13,
    2, 7, 3,13,
    3, 3, 1,13
};

const double rawNormals[72] = { //24 normals (1 normal per face)
    38731,     0, 52887,
    38731, 52887,     0,
    38731,     0,-52887,
    38731,-52887,     0,

        0, 38731,-52887,
    52887, 38731,     0,
        0, 38731, 52887,
   -52887, 38731,     0,

   -52887,     0, 38731,
        0, 52887, 38731,
    52887,     0, 38731,
        0,-52887, 38731,

   -38731,     0, 52887,
   -38731,-52887,     0,
   -38731,     0,-52887,
   -38731, 52887,     0,

   -52887,-38731,     0,
        0,-38731, 52887,
    52887,-38731,     0,
        0,-38731,-52887,

        0, 52887,-38731,
   -52887,     0,-38731,
        0,-52887,-38731,
    52887,     0,-38731
};

void Loader::loadVertices(Vertex *vertices) {

    for (int i=0; i<14; i++) {

        vertices[i].x = rawVertices[3*i];
        vertices[i].y = rawVertices[3*i+1];
        vertices[i].z = rawVertices[3*i+2];

    }
}

void Loader::loadNormals(Vertex *normals) {

    for (int i=0; i<24; i++) {

        normals[i].x = rawNormals[3*i];
        normals[i].y = rawNormals[3*i+1];
        normals[i].z = rawNormals[3*i+2];

    }
}

void Loader::loadFaces(Face *faces) {

    RGBValue color;
    for (int i=0; i<24; i++) {

        if (rawFaces[i] == 2) {
            color.rgba.red = 0x00;
            color.rgba.green = 0x16;
            color.rgba.blue = 0x3f;
        } else {
            color.rgba.red = 0xff;
            color.rgba.green = 0xff;
            color.rgba.blue = 0xff;           
        }
        color.rgba.alpha = 0x00;

        faces[i].color = color.long_value;
        faces[i].vertex1 = (int16_t) rawFaces[4*i+1];
        faces[i].vertex2 = (int16_t) rawFaces[4*i+2];
        faces[i].vertex3 = (int16_t) rawFaces[4*i+3];

    }
}