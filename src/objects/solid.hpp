// solid.hpp
#ifndef SOLID_HPP
#define SOLID_HPP

#include "../space3d.hpp"

class Solid {
public:
    Vertex* vertices;
    Face* faces;
    Vertex* faceNormals;
    int numVertices;
    int numFaces;
    Vertex* vertexNormals;

public:
    // Base constructor that allocates memory for common data members.
    Solid(int verticesCount, int facesCount)
        : numVertices(verticesCount), numFaces(facesCount),
          vertices(new Vertex[verticesCount]),
          faces(new Face[facesCount]),
          faceNormals(new Vertex[facesCount]),
          vertexNormals(new Vertex[verticesCount]) 
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

    virtual void calculateVertexNormals();

protected:
    // Protected virtual methods to be implemented by derived classes.
    virtual void loadVertices() = 0;
    virtual void loadFaces() = 0;
};

#endif
