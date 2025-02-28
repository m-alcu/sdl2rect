#include <iostream>
#include <math.h>
#include "../poly.hpp"

// Helper functions for vector math.
inline Vertex subtract(const Vertex& a, const Vertex& b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

inline Vertex cross(const Vertex& a, const Vertex& b) {
    return { a.y * b.z - a.z * b.y,
             a.z * b.x - a.x * b.z,
             a.x * b.y - a.y * b.x };
}

inline Vertex normalize(const Vertex& v) {
    float mag = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return { v.x / mag, v.y / mag, v.z / mag };
}

void Solid::calculateNormals() {
    for (int i = 0; i < numFaces; i++) {
        const Face &face = Solid::faces[i];
        Vertex v1 = Solid::vertices[face.vertex1];
        Vertex v2 = Solid::vertices[face.vertex2];
        Vertex v3 = Solid::vertices[face.vertex3];

        // Calculate the edge vectors.
        Vertex v21 = subtract(v2, v1);
        Vertex v32 = subtract(v3, v2);

        // Compute the face normal via the cross product and normalize it.
        Solid::faceNormals[i] = normalize(cross(v21, v32));
    }
}
