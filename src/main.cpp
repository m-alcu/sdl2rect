#include <SDL2/SDL.h>
#include <iostream>

int main(int argv, char** args)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("Hello SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

	bool isRunning = true;
	SDL_Event event;

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
			int colr = rand() % 255;
			int colg = rand() % 255;
			int colb = rand() % 255;
			SDL_SetRenderDrawColor(renderer, colr, colg, colb, 255);

			int x_start = rand() % 800;
			int x_end = rand() % 800;
			int y_start = rand() % 600;
			int y_end = rand() % 600;
			if (x_start > x_end) {
				x_start = x_start + x_end;
				x_end = x_start - x_end;
				x_start = x_start - x_end;
			}
			if (y_start > y_end) {
				y_start = y_start + y_end;
				y_end = y_start - y_end;
				y_start = y_start - y_end;
			}

			for (int hy=y_start;hy<y_end;hy++) {
				for(int hx=x_start;hx<x_end;hx++) {
					SDL_RenderDrawPoint(renderer, hx, hy);
				}
			}
		}
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}