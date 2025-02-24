#include <SDL2/SDL.h>
#include "poly.h"

int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    Screen screen = {600, 800};

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
    Uint32* pixels     = new Uint32[screen.width * screen.high];
    Uint32* background = new Uint32[screen.width * screen.high];

    bool isRunning = true;
    SDL_Event event;
    Uint32 ant = SDL_GetTicks();
    Uint32 dif;

    // Object data.
    Tetrakis* tetrakis = new Tetrakis(14, 24);
    tetrakis->setup();

    // Backgroud
    Desert().draw(background, screen);

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
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            {
                isRunning = false;
            }
        }

        // Calculate frame time.
        dif = SDL_GetTicks() - ant;
        ant = SDL_GetTicks();
        std::string title = "frames (ms): " + std::to_string(dif);
        SDL_SetWindowTitle(window, title.c_str());

        //draw figure into pixels memory
        memset(pixels, 0, screen.width * screen.high * sizeof(Uint32));
        render.drawObject(*tetrakis, pixels, screen);

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
        tetrakis->xAngle += 0.01f;
        tetrakis->yAngle += 0.02f;
    }

    // Free resources.
    delete[] pixels;
    delete[] background;

    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
