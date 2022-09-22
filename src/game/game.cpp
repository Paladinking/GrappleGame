#include "game.h"
#include "util/exceptions.h"

SDL_Renderer* gRenderer;
SDL_Window* gWindow;

/*
 * Base Game class
 *
 */
void Game::create() {
	if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
    {
		throw SDL_exception("STL is not initialized!");
    }
	if (gWindow != NULL || gRenderer != NULL) {
		throw game_exception("Previous game still alive!");
	}
	
	gWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN);
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


/*
 * StateGame class
 *
 */
 
StateGame::StateGame(State* initial_state, const unsigned w, const unsigned h, const std::string& title) : Game(w, h, title) {
	states.emplace(initial_state);
}

void StateGame::init() {
	states.top()->init();
}

void StateGame::render() {
	states.top()->render();
}

void StateGame::tick(Uint64 delta) {
	StateStatus status = {StateStatus::NONE, nullptr};
	states.top()->tick(delta, mouseX, mouseY, status);

	switch (status.action) {
		case StateStatus::PUSH:
			states.emplace(status.new_state);
			states.top()->init();
			break;
		case StateStatus::SWAP:
			states.top().reset(status.new_state);
			states.top()->init();
			break;
		case StateStatus::POP:
			states.pop();
			if (states.size() == 0) exit_game();
			break;
		case StateStatus::EXIT:
			exit_game();
			break;
	}			
}

void StateGame::handle_keydown(SDL_KeyboardEvent e) {
	states.top()->handle_down(e.keysym.sym, 0);
}

void StateGame::handle_keyup(SDL_KeyboardEvent e) {
	states.top()->handle_up(e.keysym.sym, 0);
}

void StateGame::handle_mousedown(SDL_MouseButtonEvent e) {
	states.top()->handle_down(SDLK_UNKNOWN, e.button);
}

void StateGame::handle_mouseup(SDL_MouseButtonEvent e) {
	states.top()->handle_up(SDLK_UNKNOWN, e.button);
}