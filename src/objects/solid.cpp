#include <iostream>
#include <math.h>
#include "../poly.hpp"

void Solid::calculateNormals() {
    for (int i = 0; i < numFaces; i++) {
        const Face &face = Solid::faces[i];
        Vertex v1 = Solid::vertices[face.vertex1];
        Vertex v2 = Solid::vertices[face.vertex2];
        Vertex v3 = Solid::vertices[face.vertex3];

        // Calculate the edge vectors.
        Vertex v21 = v2 - v1;
        Vertex v32 = v3 - v2;

        // Compute the face normal via the cross product and normalize it.
        Solid::faceNormals[i] = v21.cross(v32).normalize();
    }
}

void Solid::calculateVertexNormals() {

    for (int i = 0; i < numVertices; i++) { 
        Vertex vertexNormal = { 0, 0, 0 };
        for(int j = 0; j < numFaces; j++) {
            if (Solid::faces[j].vertex1 == i || 
                Solid::faces[j].vertex2 == i || 
                Solid::faces[j].vertex3 == i) {
                    vertexNormal += Solid::faceNormals[j];
            }
        }
        Solid::vertexNormals[i] = vertexNormal.normalize();
    }
}
