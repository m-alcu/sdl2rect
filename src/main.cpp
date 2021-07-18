#include <SDL2/SDL.h>
#include <iostream>
#include <string> 
#include "poly.h"

int main(int argv, char** args)
{
	//SDL_Init(SDL_INIT_EVERYTHING);
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

	Screen screen;
	screen.width = 320;
	screen.high = 200;

	SDL_Window *window = SDL_CreateWindow("Poly3d", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen.width, screen.high, 0);
	//SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
                                                SDL_RENDERER_ACCELERATED | 
                                                SDL_RENDERER_PRESENTVSYNC);
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
	Triangle triangle;
	Desert desert;

	Uint32 ant;
	Uint32 dif;
	

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	Uint32 * desertPalette = new Uint32[64];

	desert.calcPalette(desertPalette);
	Uint8 * greys = new Uint8[screen.width * screen.high];

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
		
		for(int i=0;i<1;i++) {
			//rectangle.randomDraw(pixels, screen);
			//triangle.randomDraw(pixels, screen);
			desert.draw(pixels,screen,desertPalette,greys);
		}

		dif = SDL_GetTicks() - ant;
		ant += dif;

		std::string s = std::to_string(dif);
		char const *num_char = s.c_str();
		SDL_SetWindowTitle(window, num_char);
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