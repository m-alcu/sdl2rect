#pragma once
#include <cstdint>
#include <vector>
#include <map>
#include "../slib.hpp"
#include "../constants.hpp"

enum class Shading {
    Flat,
    Gouraud,
    TexturedGouraud,
    BlinnPhong,
    Phong,
    TexturedPhong
};

inline std::string shadingToString(Shading s) {
    switch (s) {
        case Shading::Flat:  return "<Flat>";
        case Shading::Gouraud: return "<Gouraud>";
        case Shading::BlinnPhong: return "<BlinnPhong>";
        case Shading::TexturedGouraud: return "<TexturedGouraud>";
        case Shading::Phong: return "<Phong>";
        case Shading::TexturedPhong: return "<TexturedPhong>";
        default: return "Unknown";
    }
}

struct VertexData {
    slib::vec3 vertex;
    slib::vec3 normal;
    slib::vec2 texCoord;
};

typedef struct Face
{
    int vertex1;
    int vertex2;
    int vertex3;
    std::string materialKey;
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
struct MaterialProperties {
    float k_s; // Specular reflection coefficient
    float k_a; // Ambient reflection coefficient
    float k_d; // Diffuse reflection coefficient
    float shininess;
};

class Solid {
public:
    std::vector<VertexData> vertexData;
    std::vector<FaceData> faceData;
    Shading shading;
    Position position;
    std::map<std::string, slib::material> materials;

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

    slib::texture DecodePng(const char* filename);

protected:
    // Protected virtual methods to be implemented by derived classes.
    virtual void loadVertices() = 0;
    virtual void loadFaces() = 0;
};


