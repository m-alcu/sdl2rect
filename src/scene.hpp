#pragma once

#include <vector>
#include <memory>    // for std::unique_ptr
#include <algorithm> // for std::fill
#include <cstdint>   // for uint32_t

#include "space3d.hpp"
#include "objects/solid.hpp"

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
        zBufferInit = new float[scr.high * scr.width];
        zBuffer     = new float[scr.high * scr.width];
        pixels      = new uint32_t[scr.width * scr.high];
        std::fill(zBufferInit, zBufferInit + (scr.width * scr.high), 3.40282e+38f);
    }

    // Destructor to free the allocated memory.
    ~Scene()
    {
        delete[] zBufferInit;
        delete[] zBuffer;
        delete[] pixels;
    }

    // Called to set up the Scene, including creation of Solids, etc.
    void setup();
    void calculatePrecomputedShading(Solid& solid);

    // Add a solid to the scene's list of solids.
    // Using std::unique_ptr is a good practice for ownership.
    void addSolid(std::unique_ptr<Solid> solid)
    {
        solids.push_back(std::move(solid));
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

    // Store solids in a vector of unique_ptr to handle memory automatically.
    std::vector<std::unique_ptr<Solid>> solids;
};
