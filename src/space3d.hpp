#pragma once
#include <cstdint>
#include <cmath>
#include "slib.hpp"

typedef struct Screen
{
    uint16_t height;
    uint16_t width;
} Screen;

struct vertex {
    int32_t p_x;
    int32_t p_y;
    float p_z; 
    int16_t vtx;
    slib::vec3 normal;
    slib::vec3 vertexPoint;
    int32_t ds;

    vertex() {}

    vertex(int16_t px, int16_t py, float pz, int16_t vt, slib::vec3 n, slib::vec3 vp, int32_t s) :
    p_x(px), p_y(py), p_z(pz), vtx(vt), normal(n), vertexPoint(vp), ds(s) {}

    vertex operator+(const vertex &v) const {
        return vertex(p_x + v.p_x, p_y + v.p_y, p_z + v.p_z, vtx, normal + v.normal, vertexPoint + v.vertexPoint, ds + v.ds);
    }

    vertex& operator+=(const vertex &v) {
        p_x += v.p_x;
        p_y += v.p_y;
        p_z += v.p_z;
        normal += v.normal;
        vertexPoint += v.vertexPoint;
        ds += v.ds;
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

enum class Shading {
    Flat,
    Gouraud,
    BlinnPhong,
    Phong,
    Precomputed
};
