#pragma once


#include <cstdint>
#include <cmath>

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
    Phong
};

class Matrix {

    public:
        float r00;
        float r01;
        float r02;

        float r10;
        float r11;
        float r12;

        float r20;
        float r21;
        float r22;

        // Overload operator* to apply the matrix transformation to a Vec3.
        Vec3 operator*(const Vec3 &v) const {
            Vec3 result;
            result.x = r00 * v.x + r01 * v.y + r02 * v.z;
            result.y = r10 * v.x + r11 * v.y + r12 * v.z;
            result.z = r20 * v.x + r21 * v.y + r22 * v.z;
            return result;
        }

        // Optionally, if you need matrix multiplication, you can overload operator* for matrices.
        Matrix operator*(const Matrix &other) const {
            Matrix result;
            result.r00 = r00 * other.r00 + r01 * other.r10 + r02 * other.r20;
            result.r01 = r00 * other.r01 + r01 * other.r11 + r02 * other.r21;
            result.r02 = r00 * other.r02 + r01 * other.r12 + r02 * other.r22;

            result.r10 = r10 * other.r00 + r11 * other.r10 + r12 * other.r20;
            result.r11 = r10 * other.r01 + r11 * other.r11 + r12 * other.r21;
            result.r12 = r10 * other.r02 + r11 * other.r12 + r12 * other.r22;

            result.r20 = r20 * other.r00 + r21 * other.r10 + r22 * other.r20;
            result.r21 = r20 * other.r01 + r21 * other.r11 + r22 * other.r21;
            result.r22 = r20 * other.r02 + r21 * other.r12 + r22 * other.r22;
            return result;
        }        

    public:
        Matrix init(float xAngle, float yAngle, float zAngle);
        Matrix initInverse(float xAngle, float yAngle, float zAngle);

};

