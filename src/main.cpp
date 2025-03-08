#include <SDL2/SDL.h>
#include <sstream>
#include <iomanip>
#include "poly.hpp"
#include "render.hpp"

int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    Screen screen = {768, 1024};

    SDL_Window* window = SDL_CreateWindow("Poly3d", 
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          screen.width, screen.high, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Use streaming texture for direct pixel access.
    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screen.width, screen.high);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);    

    // Allocate pixel buffers.
    Uint32* pixels       = new Uint32[screen.width * screen.high];
    int64_t* zBufferInit = new int64_t[screen.width * screen.high];
    int64_t* zBuffer     = new int64_t[screen.width * screen.high];
    Uint32* background   = new Uint32[screen.width * screen.high];

    bool isRunning = true;
    SDL_Event event;
    Uint32 ant = SDL_GetTicks();
    Uint32 dif;

    // Object data.
    
    //Tetrakis* poly = new Tetrakis(14, 24);
    //poly->setup();
    
    //Test * poly = new Test(8, 4);
    //poly->setup();
    
    Torus* poly = new Torus(20*10, 20*10*2);
    poly->setup(20, 10, 500, 250);

    Position position;
    position.z = 2000;
    position.x = 0;
    position.y = 0;
    position.zoom = 500;
    position.xAngle = 24.79f;
    position.yAngle = 49.99f;    

    Vertex lux = {0,0,1};
    Shading shading = Shading::Flat;

    // Backgroud
    Desert().draw(background, screen);
    std::fill(zBufferInit, zBufferInit + (screen.width * screen.high), INT64_MAX);

    // Render engine
    Render render;

	// Create a texture for the background.
	SDL_Texture* backgroundTexture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, screen.width, screen.high);
	SDL_UpdateTexture(backgroundTexture, NULL, background, screen.width * sizeof(Uint32));

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
                position.y = position.y - 10;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_a) {
                position.y = position.y + 10;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_o) {
                position.x = position.x - 10;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p) {
                position.x = position.x + 10;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s) {
                position.zoom = position.zoom + 10;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_w) {
                position.zoom = position.zoom - 10;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_g) {
                shading = Shading::Gouraud;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_f) {
                shading = Shading::Flat;
            }
        }

        // Calculate frame time.
        dif = SDL_GetTicks() - ant;
        ant = SDL_GetTicks();

        std::ostringstream oss;
        oss << "xAngle: " << std::fixed << std::setprecision(2) << position.xAngle
            << " yAngle: " << std::fixed << std::setprecision(2) << position.yAngle
            << " xPos: " << std::fixed << std::setprecision(2) << position.x
            << " yPos: " << std::fixed << std::setprecision(2) << position.y
            << " zoom: " << position.zoom
            << " frames (ms): " << dif;
        std::string title = oss.str();
        SDL_SetWindowTitle(window, title.c_str());

        //draw figure into pixels memory
        memset(pixels, 0, screen.width * screen.high * sizeof(Uint32));
        std::copy(zBufferInit, zBufferInit + (screen.width * screen.high), zBuffer);
        render.drawObject(*poly, pixels, screen, zBuffer, position, lux, shading);

        // Lock the texture to update its pixel data.
        void* texturePixels = nullptr;
        int pitch = 0;
        if (SDL_LockTexture(texture, NULL, &texturePixels, &pitch) == 0) {
			memcpy(texturePixels, pixels, screen.width * screen.high * sizeof(Uint32));
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
        position.xAngle += 0.01f;
        position.yAngle += 0.02f;
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
