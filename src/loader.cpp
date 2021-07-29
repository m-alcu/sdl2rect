#include <iostream>
#include <math.h>
#include "poly.h"


void Loader::loadVertices(Vertex *vertices) {

    const float rawVertices[42] = { //14*3
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

    for (int i=0; i<14; i++) {

        vertices[i].x = rawVertices[3*i];
        vertices[i].y = rawVertices[3*i+1];
        vertices[i].z = rawVertices[3*i+2];

    }
}

void Loader::calculateNormals(Face *faces, Vertex *normals, Vertex *vertices) {

    Vertex v21;
    Vertex v32;
    Vertex normal;

     for (int i=0; i<24; i++) {
        v21.x = vertices[faces[i].vertex2].x - vertices[faces[i].vertex1].x;
        v21.y = vertices[faces[i].vertex2].y - vertices[faces[i].vertex1].y;
        v21.z = vertices[faces[i].vertex2].z - vertices[faces[i].vertex1].z;
        v32.x = vertices[faces[i].vertex3].x - vertices[faces[i].vertex2].x;
        v32.y = vertices[faces[i].vertex3].y - vertices[faces[i].vertex2].y;
        v32.z = vertices[faces[i].vertex3].z - vertices[faces[i].vertex2].z;
        normal.x = v21.y*v32.z - v32.y*v21.z;
        normal.y = v21.z*v32.x - v32.z*v21.x;
        normal.z = v21.x*v32.y - v32.x*v21.y;
        float mag = sqrt(normal.x*normal.x+normal.y*normal.y+normal.z*normal.z);
        normals[i].x = normal.x / mag;
        normals[i].y = normal.y / mag;
        normals[i].z = normal.z / mag;
     }

}

void Loader::loadFaces(Face *faces) {

    const uint16_t rawFaces[96] = { //24 faces , color + *p1 + *p2 + *p3
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

    RGBValue color;
    for (int i=0; i<24; i++) {

        if (rawFaces[4*i] == 2) {
            color.rgba.red = 0x00;
            color.rgba.green = 0x16 * 4;
            color.rgba.blue = 0x3f * 4;
        } else {
            color.rgba.red = 0xff;
            color.rgba.green = 0xff;
            color.rgba.blue = 0xff;           
        }
        color.rgba.alpha = 0x00;

        faces[i].color = color.long_value;
        faces[i].vertex1 = rawFaces[4*i+1];
        faces[i].vertex2 = rawFaces[4*i+2];
        faces[i].vertex3 = rawFaces[4*i+3];

    }
}