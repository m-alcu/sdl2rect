#include <math.h>
#include "poly.hpp"


Matrix Matrix::init(float xAngle, float yAngle, float zAngle) {
    // Create individual rotation matrices
    Matrix Rx = {
        1,         0,          0,
        0, cos(xAngle), -sin(xAngle),
        0, sin(xAngle),  cos(xAngle)
    };

    Matrix Ry = {
         cos(yAngle), 0, sin(yAngle),
                 0, 1,         0,
        -sin(yAngle), 0, cos(yAngle)
    };

    Matrix Rz = {
        cos(zAngle), -sin(zAngle), 0,
        sin(zAngle),  cos(zAngle), 0,
                0,          0, 1
    };

    // Multiply matrices in the proper order (for example: R = Rz * Ry * Rx)
    Matrix R = Rz * Ry * Rx;  // Assuming you've overloaded the operator*

    return R;
}

Matrix Matrix::initInverse(float xAngle, float yAngle, float zAngle) {
    // Inverse of Rx: rotation by -xAngle
    Matrix Rx_inv = {
        1, 0, 0,
        0, cos(xAngle), sin(xAngle),
        0, -sin(xAngle), cos(xAngle)
    };

    // Inverse of Ry: rotation by -yAngle
    Matrix Ry_inv = {
         cos(yAngle), 0, -sin(yAngle),
         0, 1, 0,
         sin(yAngle), 0, cos(yAngle)
    };

    // Inverse of Rz: rotation by -zAngle
    Matrix Rz_inv = {
        cos(zAngle), sin(zAngle), 0,
        -sin(zAngle), cos(zAngle), 0,
        0, 0, 1
    };

    // Since the original rotation is R = Rz * Ry * Rx,
    // the inverse rotation is R_inv = (Rz * Ry * Rx)^(-1) = Rx_inv * Ry_inv * Rz_inv.
    Matrix R_inv = Rx_inv * Ry_inv * Rz_inv;  // Assuming operator* is overloaded

    return R_inv;
}