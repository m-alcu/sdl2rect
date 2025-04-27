#pragma once
#include <cstdint>
#include <cmath>
#include "smath.hpp"
#include "slib.hpp"
#include "color.hpp"

typedef struct Screen
{
    int32_t height;
    int32_t width;
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

typedef struct Face
{
    int16_t vertex1;
    int16_t vertex2;
    int16_t vertex3;
    slib::material material;
} Face;

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
    Phong
};
