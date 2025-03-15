#pragma once

#include "../space3d.hpp"

class Solid {
public:
    Vec3* vertices;
    Face* faces;
    Vec3* faceNormals;
    int numVertices;
    int numFaces;
    Vec3* vertexNormals;
    Position position;

public:
    // Base constructor that allocates memory for common data members.
    Solid(int verticesCount, int facesCount)
        : numVertices(verticesCount), numFaces(facesCount),
          vertices(new Vec3[verticesCount]),
          faces(new Face[facesCount]),
          faceNormals(new Vec3[facesCount]),
          vertexNormals(new Vec3[verticesCount]) 
    {
    }

    // Virtual destructor for proper cleanup in derived classes.
    virtual ~Solid() {
        delete[] vertices;
        delete[] faces;
        delete[] faceNormals;
        delete[] vertexNormals;
    }

    // A common setup method that calls the helper functions.
    virtual void setup() {
        loadVertices();
        loadFaces();
        calculateNormals();
    }

    virtual void calculateNormals();

    virtual void calculateVec3Normals();

    virtual MaterialProperties getMaterialProperties(MaterialType type);

protected:
    // Protected virtual methods to be implemented by derived classes.
    virtual void loadVertices() = 0;
    virtual void loadFaces() = 0;
};


