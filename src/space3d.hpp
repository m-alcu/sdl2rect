#pragma once
#include <cstdint>
#include <cmath>
#include "smath.hpp"
#include "slib.hpp"

typedef struct Screen
{
    uint16_t height;
    uint16_t width;
} Screen;

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

typedef struct Material
{
    uint32_t Ambient;
    uint32_t Diffuse;
    uint32_t Specular;
    MaterialProperties properties;
} Material;

typedef struct Face
{
    int16_t vertex1;
    int16_t vertex2;
    int16_t vertex3;
    Material material;
} Face;

struct vertex {
    int32_t p_x;
    int32_t p_y;
    float p_z; 
    int16_t vtx;
    slib::vec3 normal;
    slib::vec3 vertexPoint;
    int32_t ds;
    slib::zvec2 tex; // Texture coordinates

    vertex() {}

    vertex(int32_t px, int32_t py, float pz, int16_t vt, slib::vec3 n, slib::vec3 vp, int32_t _ds, slib::zvec2 _tex) :
    p_x(px), p_y(py), p_z(pz), vtx(vt), normal(n), vertexPoint(vp), ds(_ds), tex(_tex) {}

    vertex(const vertex &v, slib::vec3 lux, Face face) {
        p_x = ( v.p_x << 16 ) + 0x8000;
        p_z = v.p_z;
        vertexPoint = v.vertexPoint;
        normal = v.normal;
        vtx = v.vtx;
        float diff = std::max(0.0f, smath::dot(lux,v.normal));
        float bright = face.material.properties.k_a+face.material.properties.k_d * diff;
        ds = (int32_t) (bright * 65536 * 4);
        tex = v.tex;
    }     

    vertex operator+(const vertex &v) const {
        return vertex(p_x + v.p_x, p_y + v.p_y, p_z + v.p_z, vtx, normal + v.normal, vertexPoint + v.vertexPoint, ds + v.ds, tex + v.tex);
    }
    

    vertex& operator+=(const vertex &v) {
        p_x += v.p_x;
        p_y += v.p_y;
        p_z += v.p_z;
        normal += v.normal;
        vertexPoint += v.vertexPoint;
        ds += v.ds;
        tex += v.tex;
        return *this;
    }
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

enum class Shading {
    Flat,
    Gouraud,
    BlinnPhong,
    Phong,
    Precomputed
};
