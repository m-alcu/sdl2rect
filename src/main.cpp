#include <SDL2/SDL.h>
#include <iostream>
#include "poly.h"

int main(int argv, char** args)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	Screen screen;
	screen.width = 800;
	screen.high = 600;

	SDL_Window *window = SDL_CreateWindow("Hello SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen.width, screen.high, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Texture * texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, screen.width, screen.high);
	Uint32 * pixels = new Uint32[screen.width * screen.high];
	Uint32 format = SDL_GetWindowPixelFormat( window );
	SDL_PixelFormat* mappingFormat = SDL_AllocFormat( format );

	memset(pixels, 255, screen.width * screen.high * sizeof(Uint32));

	bool isRunning = true;
	SDL_Event event;
	Uint32 color;
	Rectangle rectangle;
	

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	while (isRunning)
	{

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				isRunning = false;
				break;

			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					isRunning = false;
				}
			}
		}
		
		for(int i=0;i<100;i++) {
			rectangle.randomDraw(pixels, screen);
		}

		SDL_SetWindowTitle(window, "pongo el titulo");
		SDL_UpdateTexture(texture, NULL, pixels, screen.width * sizeof(Uint32));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);		
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}