#include "game.h"
#include "exceptions.h"


void Game::create() {
	if (SDL_Init( SDL_INIT_VIDEO ) < 0)
    {
		throw SDL_exception("Failed to initialize STL, " + std::string(SDL_GetError()));
    }
	
	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN );
    if (window == NULL )
    {
		throw SDL_exception("Window could not be created, " + std::string(SDL_GetError()));
    }
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) 
	{
		throw SDL_exception("Renderer could not be created, " + std::string(SDL_GetError()));
	}
	
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
}

void Game::run() {
	running = true;
	Uint64 last_time = SDL_GetTicks64();
	
	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) 
		{
			if (e.type == SDL_QUIT) {
				running = false;
			}
		}
		this->handle_input();
		Uint64 cur_time = SDL_GetTicks64();
		this->tick(cur_time - last_time);
		last_time = cur_time;
		
		render();
	}
}

void Game::exit_game() {
	if (!exited) {
		exited = true;
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
	
		SDL_DestroyWindow(window);
		window = NULL;
	
		IMG_Quit();
    
		SDL_Quit();
	}
	
}

Game::~Game() {
	exit_game();
}