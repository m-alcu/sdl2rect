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

struct vertex {
    slib::vec3 point;
    slib::vec4 ndc;
    int32_t p_x;
    int32_t p_y;
    float p_z; 
    slib::vec3 normal;
    slib::zvec2 tex; // Texture coordinates
    Color color;

    vertex() {}

    vertex(int32_t px, int32_t py, float pz, slib::vec3 n, slib::vec4 vp, slib::zvec2 _tex, Color _color) :
    p_x(px), p_y(py), p_z(pz), normal(n), ndc(vp), tex(_tex), color(_color) {}

    vertex operator+(const vertex &v) const {
        return vertex(p_x + v.p_x, p_y + v.p_y, p_z + v.p_z, normal + v.normal, ndc + v.ndc, tex + v.tex, color + v.color);
    }

    vertex operator-(const vertex &v) const {
        return vertex(p_x - v.p_x, p_y - v.p_y, p_z - v.p_z, normal - v.normal, ndc - v.ndc, tex - v.tex, color - v.color);
    }

    vertex operator*(const float &rhs) const {
        return vertex(p_x * rhs, p_y * rhs, p_z * rhs, normal * rhs, ndc * rhs, tex * rhs, color * rhs);
    }

    vertex operator/(const float &rhs) const {
        return vertex(p_x / rhs, p_y / rhs, p_z / rhs, normal / rhs, ndc / rhs, tex / rhs, color / rhs);
    }
    

    vertex& operator+=(const vertex &v) {
        p_x += v.p_x;
        p_y += v.p_y;
        p_z += v.p_z;
        normal += v.normal;
        ndc += v.ndc;
        tex += v.tex;
        color += v.color;
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
    Phong
};
