#pragma once
#include <cstdint>
#include <vector>
#include "../slib.hpp"
#include "../constants.hpp"

enum class Shading {
    Flat,
    Gouraud,
    BlinnPhong,
    Phong
};

inline std::string shadingToString(Shading s) {
    switch (s) {
        case Shading::Flat:  return "<Flat>";
        case Shading::Gouraud: return "<Gouraud>";
        case Shading::BlinnPhong: return "<BlinnPhong>";
        case Shading::Phong: return "<Phong>";
        default: return "Unknown";
    }
}

struct VertexData {
    slib::vec3 vertex;
    slib::vec3 normal;
};

typedef struct Face
{
    int16_t vertex1;
    int16_t vertex2;
    int16_t vertex3;
    slib::material material;
} Face;

struct FaceData {
    Face face;
    slib::vec3 faceNormal;
};

typedef struct Position
{
    float x;
    float y;
    float z;
    float zoom;
    float xAngle;
    float yAngle;
    float zAngle;    
} Position;

enum class MaterialType {
    Rubber,
    Plastic,
    Wood,
    Marble,
    Glass,
    Metal,
    Mirror 
};

// Struct to hold k_s, k_a, and k_d values
typedef struct MaterialProperties {
    float k_s; // Specular reflection coefficient
    float k_a; // Ambient reflection coefficient
    float k_d; // Diffuse reflection coefficient
    float shininess;
} MaterialProperties;

class Solid {
public:
    std::vector<VertexData> vertexData;
    std::vector<FaceData> faceData;
    Shading shading;
    Position position;

    int numVertices;
    int numFaces;
 
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


