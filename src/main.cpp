#include <SDL2/SDL.h>
#include <sstream>
#include <iomanip>
#include "poly.hpp"
#include "render.hpp"
#include "backgrounds/desert.hpp"

int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    Scene scene;
    scene.screen = {768, 1024};
    scene.lux = {0, 0, 1};
    scene.eye = {0, 0, 1};
    scene.shading = Shading::Flat;

    SDL_Window* window = SDL_CreateWindow("Poly3d", 
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          scene.screen.width, scene.screen.high, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Use streaming texture for direct pixel access.
    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, scene.screen.width, scene.screen.high);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);    

    // Allocate pixel buffers.
    Uint32* pixels       = new Uint32[scene.screen.width * scene.screen.high];
    int64_t* zBufferInit = new int64_t[scene.screen.width * scene.screen.high];
    int64_t* zBuffer     = new int64_t[scene.screen.width * scene.screen.high];
    Uint32* background   = new Uint32[scene.screen.width * scene.screen.high];

    bool isRunning = true;
    SDL_Event event;
    Uint32 from;
    Uint32 to;

    // Object data.
    
    //Tetrakis* poly = new Tetrakis(14, 24);
    //poly->setup();
    
    //Test * poly = new Test(8, 4);
    //poly->setup();
    
    Torus* poly = new Torus(20*10, 20*10*2);
    poly->setup(20, 10, 500, 250);

    poly->position.z = 2000;
    poly->position.x = 0;
    poly->position.y = 0;
    poly->position.zoom = 500;
    poly->position.xAngle = 24.79f;
    poly->position.yAngle = 49.99f;    

    // Backgroud
    Desert().draw(background, scene.screen.high, scene.screen.width);
    std::fill(zBufferInit, zBufferInit + (scene.screen.width * scene.screen.high), INT64_MAX);

    poly->calculatePrecomputedShading(scene);

    // Render engine
    Render render;

	// Create a texture for the background.
	SDL_Texture* backgroundTexture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, scene.screen.width, scene.screen.high);
	SDL_UpdateTexture(backgroundTexture, NULL, background, scene.screen.width * sizeof(Uint32));

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
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q) {
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
        memset(pixels, 0, scene.screen.width * scene.screen.high * sizeof(Uint32));
        std::copy(zBufferInit, zBufferInit + (scene.screen.width * scene.screen.high), zBuffer);
        render.drawObject(*poly, pixels, zBuffer, scene);
        to = SDL_GetTicks();

        std::ostringstream oss;
        oss << "xAngle: " << std::fixed << std::setprecision(2) << poly->position.xAngle
            << " yAngle: " << std::fixed << std::setprecision(2) << poly->position.yAngle
            << " xPos: " << std::fixed << std::setprecision(2) << poly->position.x
            << " yPos: " << std::fixed << std::setprecision(2) << poly->position.y
            << " zoom: " << poly->position.zoom
            << " frames (ms): " << (to - from);
        std::string title = oss.str();
        SDL_SetWindowTitle(window, title.c_str());        

        // Lock the texture to update its pixel data.
        void* texturePixels = nullptr;
        int pitch = 0;
        if (SDL_LockTexture(texture, NULL, &texturePixels, &pitch) == 0) {
			memcpy(texturePixels, pixels, scene.screen.width * scene.screen.high * sizeof(Uint32));
			SDL_UnlockTexture(texture);
        } else {
            std::cerr << "SDL_LockTexture error: " << SDL_GetError() << std::endl;
        }

        // Render the updated texture.
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // Update rotation angles.
        poly->position.xAngle += 0.005f;
        poly->position.yAngle += 0.010f;
    }

    // Free resources.
    delete[] pixels;
    delete[] zBuffer;
    delete[] zBufferInit;
    delete[] background;

    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
