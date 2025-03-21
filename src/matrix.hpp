#pragma once
#include <cstdint>
#include <cmath>
#include "space3d.hpp"

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


