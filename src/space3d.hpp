#pragma once
#include <cstdint>
#include <cmath>

typedef struct Screen
{
    uint16_t high;
    uint16_t width;
} Screen;

typedef struct vertex
{
    int16_t p_x;
    int16_t p_y;
    float p_z; 
    int16_t vtx;
} vertex;

typedef struct Position
{
    float x;
    float y;
    float z;
    int zoom;
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
