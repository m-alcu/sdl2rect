#pragma once

#include <vector>
#include "../space3d.hpp"
#include "../slib.hpp"
#include "../constants.hpp"

class Solid {
public:
    std::vector<slib::vec3> vertices;
    std::vector<Face> faces;
    std::vector<slib::vec3> faceNormals;
    std::vector<slib::vec3> vertexNormals;
    Position position;
    uint32_t* precomputedShading;
    int numVertices;
    int numFaces;

public:
    // Base constructor that initializes common data members.
    Solid()
        :precomputedShading(new uint32_t[PRECOMPUTE_SIZE*PRECOMPUTE_SIZE])
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

protected:
    // Protected virtual methods to be implemented by derived classes.
    virtual void loadVertices() = 0;
    virtual void loadFaces() = 0;
};


