#pragma once
#include <SDL2/SDL.h>
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
        sdlSurface = SDL_CreateRGBSurface(0, screen.width, screen.height, 32, 0, 0, 0, 0);
        SDL_SetSurfaceBlendMode(sdlSurface, SDL_BLENDMODE_NONE);
    }

    // Destructor to free the allocated memory.
    ~Scene()
    {
        SDL_FreeSurface(sdlSurface);
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
    SDL_Surface* sdlSurface = nullptr; // SDL surface for rendering.

    // Store solids in a vector of unique_ptr to handle memory automatically.
    std::vector<std::unique_ptr<Solid>> solids;
};
