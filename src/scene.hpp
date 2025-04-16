#pragma once

#include <vector>
#include <memory>    // for std::unique_ptr
#include <algorithm> // for std::fill
#include <cstdint>   // for uint32_t

#include "space3d.hpp"
#include "objects/solid.hpp"
#include "smath.hpp"

class Scene
{
public:
    // Constructor that initializes the Screen and allocates zBuffer arrays.
    Scene(const Screen& scr)
        : screen(scr),
          zBuffer(nullptr),
          fullTransformMat(smath::identity()),
          normalTransformMat(smath::identity()),
          projectionMatrix(smath::identity())
    {
        // Allocate memory for zBufferInit and zBuffer.
        zBuffer     = new float[scr.height * scr.width];
        pixels      = new uint32_t[scr.width * scr.height];
    }

    // Destructor to free the allocated memory.
    ~Scene()
    {
        delete[] zBuffer;
        delete[] pixels;
    }

    // Called to set up the Scene, including creation of Solids, etc.
    void setup();

    // Add a solid to the scene's list of solids.
    // Using std::unique_ptr is a good practice for ownership.
    void addSolid(std::unique_ptr<Solid> solid)
    {
        solids.push_back(std::move(solid));
    }

    Screen screen;
    Shading shading;
    slib::vec3 lux;
    slib::vec3 eye;
    slib::mat4 fullTransformMat;
    slib::mat4 normalTransformMat;
    slib::mat4 projectionMatrix;
    float* zBufferInit;
    float* zBuffer;
    uint32_t* pixels;

    // Store solids in a vector of unique_ptr to handle memory automatically.
    std::vector<std::unique_ptr<Solid>> solids;
};
