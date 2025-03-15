#ifndef SPACE3D_HPP
#define SPACE3D_HPP

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

typedef struct Vertex
{
    float x;
    float y;
    float z;

    // Default constructor
    Vertex() : x(0.0f), y(0.0f), z(0.0f) {}

    // Constructor with parameters
    Vertex(float x, float y, float z) : x(x), y(y), z(z) {}

    // Addition
    Vertex operator+(const Vertex& other) const {
        return Vertex(x + other.x, y + other.y, z + other.z);
    }

    Vertex& operator+=(const Vertex& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    // Subtraction
    Vertex operator-(const Vertex& other) const {
        return Vertex(x - other.x, y - other.y, z - other.z);
    }

    // Scalar multiplication
    Vertex operator*(float scalar) const {
        return Vertex(x * scalar, y * scalar, z * scalar);
    }

    // Scalar division
    Vertex operator/(float scalar) const {
        return Vertex(x / scalar, y / scalar, z / scalar);
    }

    // Dot product
    float dot(const Vertex& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product
    Vertex cross(const Vertex& other) const {
        return Vertex(
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
    Vertex normalize() const {
        float mag = magnitude();
        if (mag == 0.0f)
            return Vertex(0.0f, 0.0f, 0.0f);
        return *this / mag;
    }

    // Print the vector (for debugging)
    void print() const {
        std::cout << "(" << x << ", " << y << ", " << z << ")\n";
    }

} Vertex;

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
    int32_t color;
    Material material;
} Face;

enum class Shading {
    Flat,
    Gouraud,
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

        // Overload operator* to apply the matrix transformation to a Vertex.
        Vertex operator*(const Vertex &v) const {
            Vertex result;
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

#endif