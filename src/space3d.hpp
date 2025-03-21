#pragma once
#include <cstdint>
#include <cmath>

typedef struct Screen
{
    uint16_t high;
    uint16_t width;
} Screen;

typedef struct Pixel
{
    int16_t p_x;
    int16_t p_y;
    float p_z; 
    int16_t vtx;
} Pixel;

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

typedef struct Vec3
{
    float x;
    float y;
    float z;

    // Default constructor
    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}

    // Constructor with parameters
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Addition
    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    // Subtraction
    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    // Scalar multiplication
    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    // Scalar division
    Vec3 operator/(float scalar) const {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    // Dot product
    float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product
    Vec3 cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Magnitude (length) of the vector
    float magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Normalize the vector (returns unit vector)
    Vec3 normalize() const {
        float mag = magnitude();
        if (mag == 0.0f)
            return Vec3(0.0f, 0.0f, 0.0f);
        return *this / mag;
    }

    // Print the vector (for debugging)
    void print() const {
        std::cout << "(" << x << ", " << y << ", " << z << ")\n";
    }

} Vec3;

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
