#pragma once

#include "../space3d.hpp"
#include "../slib.hpp"
#include "../constants.hpp"

class Solid {
public:
    slib::vec3* vertices;
    Face* faces;
    slib::vec3* faceNormals;
    int numVertices;
    int numFaces;
    slib::vec3* vertexNormals;
    Position position;
    uint32_t* precomputedShading;

public:
    // Base constructor that allocates memory for common data members.
    Solid(int verticesCount, int facesCount)
        : numVertices(verticesCount), numFaces(facesCount),
          vertices(new slib::vec3[verticesCount]),
          faces(new Face[facesCount]),
          faceNormals(new slib::vec3[facesCount]),
          vertexNormals(new slib::vec3[verticesCount]),
          precomputedShading(new uint32_t[PRECOMPUTE_SIZE*PRECOMPUTE_SIZE])
    {
    }

    // Virtual destructor for proper cleanup in derived classes.
    virtual ~Solid() {
        delete[] vertices;
        delete[] faces;
        delete[] faceNormals;
        delete[] vertexNormals;
        delete[] precomputedShading;
    }

    // A common setup method that calls the helper functions.
    virtual void setup() {
        loadVertices();
        loadFaces();
        calculateNormals();
    }

    virtual void calculateNormals();

    virtual void calculateVertexNormals();

    virtual MaterialProperties getMaterialProperties(MaterialType type);

protected:
    // Protected virtual methods to be implemented by derived classes.
    virtual void loadVertices() = 0;
    virtual void loadFaces() = 0;
};


