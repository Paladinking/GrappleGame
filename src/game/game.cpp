#include "game.h"
#include "util/exceptions.h"

SDL_Renderer* gRenderer;
SDL_Window* gWindow;

void Game::create() {
	if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
    {
		throw SDL_exception("STL is not initialized!");
    }
	if (gWindow != NULL || gRenderer != NULL) {
		throw game_exception("Previous game still alive!");
	}
	
	gWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN );
    if (gWindow == NULL )
    {
		throw SDL_exception("Window could not be created, " + std::string(SDL_GetError()));
    }
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == NULL) 
	{
		SDL_DestroyWindow(gWindow);
		throw SDL_exception("Renderer could not be created, " + std::string(SDL_GetError()));
	}
	
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	
	destroyed = false;
	init();
}

void Game::run() {
	if (destroyed)
	{
		return;
	}
	running = true;
	Uint64 last_time = SDL_GetTicks64();
	
	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) 
		{
			switch (e.type) {
				case SDL_QUIT:
					exit_game();
					break; 
				case SDL_KEYDOWN:
					handle_keydown(e.key);
					break;
				case SDL_KEYUP:
					handle_keyup(e.key);
					break;
				case SDL_MOUSEBUTTONDOWN:
					handle_mousedown(e.button);
					break;
				case SDL_MOUSEBUTTONUP:
					handle_mouseup(e.button);
					break;
			}
		}
		mouseButton = SDL_GetMouseState(&mouseX, &mouseY);
		Uint64 cur_time = SDL_GetTicks64();
		this->tick(cur_time - last_time);
		last_time = cur_time;
		
		render();
	}
}

void Game::destroy_game() {
	if (!destroyed) {
		destroyed = true;
		SDL_DestroyRenderer(gRenderer);
		gRenderer = NULL;
	
		SDL_DestroyWindow(gWindow);
		gWindow = NULL;
	}
}

void Game::exit_game() {
	running = false;
}

Game::~Game() {
	destroy_game();
}