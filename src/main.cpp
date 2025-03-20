#include <SDL2/SDL.h>
#include <sstream>
#include <iomanip>
#include "poly.hpp"
#include "renderer.hpp"
#include "backgrounds/background.hpp"
#include "backgrounds/backgroundFactory.hpp"
#include "scene.hpp"

int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    Scene scene({768, 1024});
    scene.lux = {0, 0, 1};
    scene.eye = {0, 0, 1};
    scene.shading = Shading::Flat;
    scene.setup();

    SDL_Window* window = SDL_CreateWindow("Poly3d", 
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          scene.screen.width, scene.screen.high, 0);
    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1, 
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Use streaming texture for direct pixel access.
    SDL_Texture* texture = SDL_CreateTexture(sdlRenderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, scene.screen.width, scene.screen.high);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);    

    // Allocate pixel buffers.
    
    bool isRunning = true;
    SDL_Event event;
    Uint32 from;
    Uint32 to;

    // Backgroud
    Uint32* back = new Uint32[scene.screen.width * scene.screen.high];
    auto background = BackgroundFactory::createBackground(BackgroundType::IMAGE_PNG);
    background->draw(back, scene.screen.high, scene.screen.width);

    // Renderer engine
    Renderer renderer;

	// Create a texture for the background.
	SDL_Texture* backgroundTexture = SDL_CreateTexture(sdlRenderer,
    SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, scene.screen.width, scene.screen.high);
	SDL_UpdateTexture(backgroundTexture, NULL, back, scene.screen.width * sizeof(Uint32));

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
            /*} else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q) {
                poly->position.y = poly->position.y - 10;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_a) {
                poly->position.y = poly->position.y + 10;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_o) {
                poly->position.x = poly->position.x - 10;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p) {
                poly->position.x = poly->position.x + 10;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s) {
                poly->position.zoom = poly->position.zoom + 10;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_w) {
                poly->position.zoom = poly->position.zoom - 10;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_e) {
                poly->position.z = poly->position.z + 10;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_d) {
                poly->position.z = poly->position.z - 10;  */               
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_g) {
                scene.shading = Shading::Gouraud;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_f) {
                scene.shading = Shading::Flat;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_h) {
                scene.shading = Shading::BlinnPhong;                 
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_j) {
                scene.shading = Shading::Phong;     
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_k) {
                scene.shading = Shading::Precomputed;                              
            }
        }

        // Calculate frame time.
        from = SDL_GetTicks();
        //draw figure into pixels memory

        renderer.drawScene(scene);
        to = SDL_GetTicks();

        std::ostringstream oss;
/*        oss << "xAngle: " << std::fixed << std::setprecision(2) << poly->position.xAngle
            << " yAngle: " << std::fixed << std::setprecision(2) << poly->position.yAngle
            << " xPos: " << std::fixed << std::setprecision(2) << poly->position.x
            << " yPos: " << std::fixed << std::setprecision(2) << poly->position.y
            << " zoom: " << poly->position.zoom
            << " frames (ms): " << (to - from);*/
        std::string title = oss.str();
        SDL_SetWindowTitle(window, title.c_str());        

        // Lock the texture to update its pixel data.
        void* texturePixels = nullptr;
        int pitch = 0;
        if (SDL_LockTexture(texture, NULL, &texturePixels, &pitch) == 0) {
			memcpy(texturePixels, scene.pixels, scene.screen.width * scene.screen.high * sizeof(Uint32));
			SDL_UnlockTexture(texture);
        } else {
            std::cerr << "SDL_LockTexture error: " << SDL_GetError() << std::endl;
        }

        // Renderer the updated texture.
        SDL_RenderClear(sdlRenderer);
        SDL_RenderCopy(sdlRenderer, backgroundTexture, NULL, NULL);
        SDL_RenderCopy(sdlRenderer, texture, NULL, NULL);
        SDL_RenderPresent(sdlRenderer);

        // Update rotation angles.
        /*poly->position.xAngle += 0.005f;
        poly->position.yAngle += 0.010f;*/
    }

    // Free resources.
    delete[] back;

    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
