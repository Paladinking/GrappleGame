#include "game.h"
#include "exceptions.h"

SDL_Renderer* gRenderer;
SDL_Window* gWindow;

void Game::create() {
	if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
    {
		throw SDL_exception("STL is not initialized!");
    }
	if (gWindow != NULL || gRenderer != NULL) {
		throw Game_exception("Previous game still alive!");
	}
	
	gWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN );
    if (gWindow == NULL )
    {
		throw SDL_exception("Window could not be created, " + std::string(SDL_GetError()));
    }
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == NULL) 
	{
		throw SDL_exception("Renderer could not be created, " + std::string(SDL_GetError()));
	}
	
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	
	init();
	destroyed = false;
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
			if (e.type == SDL_QUIT) {
				exit_game();

			} 
#ifndef NO_EVENT_INPUT
			else if (e.type == SDL_KEYDOWN) {
				handle_keydown(e.key);
			} else if (e.type == SDL_KEYUP) {
				handle_keyup(e.key);
			}
#endif
		}
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
	exit_game();
}