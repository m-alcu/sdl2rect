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