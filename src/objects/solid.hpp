#pragma once

#include <vector>
#include "../space3d.hpp"
#include "../slib.hpp"
#include "../constants.hpp"

struct VertexData {
    slib::vec3 vertex;
    slib::vec3 normal;
};

struct FaceData {
    Face face;
    slib::vec3 faceNormal;
};

class Solid {
public:
    std::vector<VertexData> vertexData;
    std::vector<FaceData> faceData;
    Shading shading;
    Position position;
    slib::mat4 fullTransformMat;
    slib::mat4 normalTransformMat;
    int numVertices;
    int numFaces;
 
public:
    // Base constructor that initializes common data members.
    Solid() : fullTransformMat(smath::identity()),
            normalTransformMat(smath::identity())
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


