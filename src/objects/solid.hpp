#pragma once

#include <vector>
#include "../space3d.hpp"
#include "../slib.hpp"
#include "../constants.hpp"

struct VertexData {
    std::vector<slib::vec3> vertices;
    std::vector<slib::vec3> vertexNormals;
    int numVertices;
};

struct FaceData {
    std::vector<Face> faces;
    std::vector<slib::vec3> faceNormals;
    int numFaces;
};

class Solid {
public:
    VertexData vertexData;
    FaceData faceData;
    Shading shading;
    Position position;
 
public:
    // Base constructor that initializes common data members.
    Solid()
    {
    }

    // Virtual destructor for proper cleanup in derived classes.
    virtual ~Solid() = default;

    // A common setup method that calls the helper functions.
    virtual void setup() {
        loadVertices();
        loadFaces();
        calculateNormals();
        calculateVertexNormals();
    }

    virtual void calculateNormals();

    virtual void calculateVertexNormals();

    virtual MaterialProperties getMaterialProperties(MaterialType type);

    virtual int getColorFromMaterial(const float color);

protected:
    // Protected virtual methods to be implemented by derived classes.
    virtual void loadVertices() = 0;
    virtual void loadFaces() = 0;
};


