#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>    // for std::unique_ptr
#include <algorithm> // for std::fill
#include <cstdint>   // for uint32_t

#include "objects/solid.hpp"
#include "smath.hpp"
#include "slib.hpp"
#include "ZBuffer.hpp"


struct Camera
{
    slib::vec3 pos;
    slib::vec3 rotation;    
    slib::vec3 eye;
    slib::vec3 target;
    slib::vec3 up;
    float pitch;
    float yaw;
    slib::vec3 forward;
};

typedef struct Screen
{
    int32_t height;
    int32_t width;
} Screen;

class Scene
{
public:
    // Constructor that initializes the Screen and allocates zBuffer arrays.
    Scene(const Screen& scr)
        : screen(scr),
          zBuffer( std::make_shared<ZBuffer>( scr.width,scr.height )),
          projectionMatrix(smath::identity())
    {
        sdlSurface = SDL_CreateRGBSurface(0, screen.width, screen.height, 32, 0, 0, 0, 0);
        SDL_SetSurfaceBlendMode(sdlSurface, SDL_BLENDMODE_NONE);
        camera.eye = {0.0f, 0.0f, 0.0f};          // Camera position
        camera.target = {0.0f, 0.0f, -1.0f};      // Point to look at (in -Z)
        camera.up = {0.0f, 1.0f, 0.0f};           // Up vector (typically +Y)
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
   
    slib::vec3 lux;
    slib::vec3 eye;
    slib::vec3 halfwayVector;
    slib::mat4 projectionMatrix;
    std::shared_ptr<ZBuffer> zBuffer; // Use shared_ptr for zBuffer to manage its lifetime automatically.
    SDL_Surface* sdlSurface = nullptr; // SDL surface for rendering.

    Camera camera; // Camera object to manage camera properties.
    // Store solids in a vector of unique_ptr to handle memory automatically.
    std::vector<std::unique_ptr<Solid>> solids;
};
