#pragma once
#include "space3d.hpp"

// This is the C++ class corresponding to your struct Scene:
class Scene
{
public:
    // Constructor that initializes the Screen and allocates zBuffer arrays.
    Scene(const Screen& scr)
        : screen(scr),
          zBufferInit(nullptr),
          zBuffer(nullptr)
    {
        // Allocate memory for zBufferInit and zBuffer.
        zBufferInit = new float[scr.high*scr.width];
        zBuffer     = new float[scr.high*scr.width];
        pixels      = new uint32_t[scr.width * scr.high];
        std::fill(zBufferInit, zBufferInit + (scr.width * scr.high), 3.40282e+38);
    }

    // Destructor to free the allocated memory.
    ~Scene()
    {
        delete[] zBufferInit;
        delete[] zBuffer;
    }

    Screen screen;
    Shading shading;
    Vec3 lux;
    Vec3 luxInversePrecomputed;
    Vec3 eye;
    Vec3 eyeInversePrecomputed;
    Matrix rotate;
    Matrix inverseRotate;
    float* zBufferInit;
    float* zBuffer;
    uint32_t* pixels;
};
