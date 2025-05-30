#include <SDL2/SDL.h>
#include <sstream>
#include <iomanip>
#include <chrono>
#include "rasterizer.hpp"
#include "renderer.hpp"
#include "backgrounds/background.hpp"
#include "backgrounds/backgroundFactory.hpp"
#include "scene.hpp"
#include "average.hpp"

int main(int argc, char** argv)
{

    bool isRunning = true;
    SDL_Event event;
    Uint32 from;
    Uint32 to;
    static FrameTimeAverager frameTimeAvg;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    int width = 800;
    int height = 600;

    // Renderer engine
    Renderer renderer;

    SDL_Window* window = SDL_CreateWindow("Poly3d", 
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          width, height, 0);

    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1, 0);
    if (sdlRenderer == nullptr)
    {
        std::cout << "Could not initialise SDL renderer. Exiting..." << std::endl;
        exit(1);
    }

    Scene scene({height, width});
    scene.lux = smath::normalize(slib::vec3{0, 1, 1});;
    scene.eye = {0, 0, 1};
    scene.camera.pos = {0, 0, 0};
    scene.camera.pitch = 0;
    scene.camera.yaw = 0;
    scene.setup();

    float zNear = 100.0f; // Near plane distance
    float zFar  = 10000.0f; // Far plane distance
    float viewAngle = 45.0f; // Field of view angle in degrees

    //SDL_SetRelativeMouseMode(SDL_TRUE);

    // Backgroud
    Uint32* back = new Uint32[scene.screen.width * scene.screen.height];
    auto background = BackgroundFactory::createBackground(BackgroundType::DESERT);
    background->draw(back, scene.screen.height, scene.screen.width);

    float mouseSensitivity = 0.1f;
    float cameraSpeed = 100.0f;
    // Main loop.
    while (isRunning)
    {
        // Process events.
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                isRunning = false;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LEFT) {
                scene.camera.yaw = scene.camera.yaw + 1;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT) {
                scene.camera.yaw = scene.camera.yaw - 1;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP) {
                scene.camera.pitch = scene.camera.pitch - 1;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN) {
                scene.camera.pitch = scene.camera.pitch + 1;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q) {
                scene.camera.pos = scene.camera.pos - scene.camera.forward * cameraSpeed; 
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_a) {
                scene.camera.pos = scene.camera.pos + scene.camera.forward * cameraSpeed; 
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_f) {
                scene.solids[0]->shading = Shading::Flat;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
                scene.solids[0]->shading = Shading::TexturedFlat;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_g) {
                scene.solids[0]->shading = Shading::Gouraud;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_t) {
                scene.solids[0]->shading = Shading::TexturedGouraud;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_h) {
                scene.solids[0]->shading = Shading::BlinnPhong; 
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_y) {
                scene.solids[0]->shading = Shading::TexturedBlinnPhong;                                 
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_j) {
                scene.solids[0]->shading = Shading::Phong;   
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_u) {
                scene.solids[0]->shading = Shading::TexturedPhong;                                                 
            }
        }



        from = SDL_GetTicks();
        renderer.drawScene(scene, zNear, zFar, viewAngle, back);
        to = SDL_GetTicks();

        auto durationMs = std::chrono::duration<double, std::milli>(to - from).count();
        double smoothedMs = frameTimeAvg.update(durationMs);

        std::ostringstream oss;
        oss << "pos: (" << std::fixed << std::setprecision(2) << scene.camera.pos.x
            << "," << std::fixed << std::setprecision(2) << scene.camera.pos.y
            << "," << std::fixed << std::setprecision(2) << scene.camera.pos.z
            << ") " << shadingToString(scene.solids[0]->shading)
            << " frames/s: " << std::fixed << std::setprecision(2) << 1000/smoothedMs;
        std::string title = oss.str();
        SDL_SetWindowTitle(window, title.c_str());        

        SDL_Texture* tex = SDL_CreateTextureFromSurface(sdlRenderer, scene.sdlSurface);
        SDL_RenderCopy(sdlRenderer, tex, nullptr, nullptr);
        SDL_DestroyTexture(tex);
        SDL_RenderPresent(sdlRenderer);

        // Update rotation angles.
        scene.solids[0]->position.xAngle += 0.5f;
        scene.solids[0]->position.yAngle += 1.0f;
    }

    // Free resources.
    delete[] back;

    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
