#pragma once

#include <vector>
#include <memory>    // for std::unique_ptr
#include <algorithm> // for std::fill
#include <cstdint>   // for uint32_t

#include "space3d.hpp"
#include "objects/solid.hpp"
#include "smath.hpp"
#include "ZBuffer.hpp"

class Scene
{
public:
    // Constructor that initializes the Screen and allocates zBuffer arrays.
    Scene(const Screen& scr)
        : screen(scr),
          zBuffer( std::make_shared<ZBuffer>( scr.width,scr.height )),
          fullTransformMat(smath::identity()),
          normalTransformMat(smath::identity()),
          projectionMatrix(smath::identity())
    {
        // Allocate memory for zBufferInit and zBuffer.
        pixels      = new uint32_t[scr.width * scr.height];
    }

    // Destructor to free the allocated memory.
    ~Scene()
    {
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
    std::shared_ptr<ZBuffer> zBuffer; // Use shared_ptr for zBuffer to manage its lifetime automatically.
    uint32_t* pixels;

    // Store solids in a vector of unique_ptr to handle memory automatically.
    std::vector<std::unique_ptr<Solid>> solids;
};
