#include "game.h"
#include <iostream>

SDL_Renderer* gRenderer;
SDL_Window* gWindow;

/*
 * Base Game class
 *
 */
Game::Game(const int window_width, const int window_height, const std::string& title) 
	: initial_width(window_width), initial_height(window_height), initial_title(title) {
	if (initial_width <= 0 || initial_height <= 0) throw game_exception("Invalid window dimensions");
}
 
 
void Game::create() {
	if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
    {
		throw SDL_exception("STL is not initialized!");
    }
	if (gWindow != NULL || gRenderer != NULL) {
		throw game_exception("Previous game still alive!");
	}

	gWindow = SDL_CreateWindow(
		initial_title.c_str(), 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		initial_width, initial_height, 
		SDL_WINDOW_HIDDEN
	);

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
	
	SDL_RenderSetVSync(gRenderer, 1);
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	SDL_GetWindowSize(gWindow, &window_state.window_width, &window_state.window_height);
	SDL_GetRendererOutputSize(gRenderer, &window_state.screen_width, &window_state.screen_height);
	destroyed = false;

	window_state.keyboard_state = SDL_GetKeyboardState(NULL);
	init();
}

void Game::run() {
	if (destroyed)
	{
		return;
	}
	running = true;
	Uint64 last_time = SDL_GetTicks64();
	
	while (true) {
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
				case SDL_MOUSEWHEEL:
					handle_mousewheel(e.wheel);
					break;
			}
		}
		window_state.mouse_mask = SDL_GetMouseState(&window_state.mouseX, &window_state.mouseY);
		Uint64 cur_time = SDL_GetTicks64();
		this->tick(cur_time - last_time);
		if (!running) break;
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
 * State class
 *
 */ 
void State::init(WindowState* window_state) {
	this->window_state = window_state;
}

int State::get_prefered_width() const {
	return -1;
}

int State::get_prefered_height() const {
	return -1;
}



/*
 * StateGame class
 *
 */
 
StateGame::StateGame(State* initial_state, const int w, const int h, const std::string& title) : 
	Game(w, h, title) {
	states.emplace(initial_state);
}

void StateGame::init() {
	update_window(states.top().get());
	states.top()->init(&window_state);
}

void StateGame::render() {
	states.top()->render();
}

void StateGame::tick(Uint64 delta) {
	StateStatus status = {StateStatus::NONE, nullptr};
	states.top()->tick(delta, status);

	switch (status.action) {
		case StateStatus::PUSH:
			states.emplace(status.new_state);
			update_window(status.new_state);
			states.top()->init(&window_state);
			break;
		case StateStatus::SWAP:
			states.top().reset(status.new_state);
			update_window(status.new_state);
			states.top()->init(&window_state);
			break;
		case StateStatus::POP:
			states.pop();
			if (states.size() == 0) {
				exit_game();
			} else {
				update_window(states.top().get());
				states.top()->resume();
			}
			break;
		case StateStatus::EXIT:
			exit_game();
			break;
	}			
}

void StateGame::update_window(const State* const state) {
	int w = state->get_prefered_width(), h = state->get_prefered_height();
	if (w == -1) w = window_state.screen_width;
	if (h == -1) h = window_state.screen_height;
	if (w != window_state.screen_width || h != window_state.screen_height) {
		SDL_SetWindowSize(gWindow, w, h);
		window_state.screen_width = w;
		window_state.screen_height = h;
	}
}

void StateGame::handle_keydown(SDL_KeyboardEvent &e) {
	states.top()->handle_down(e.keysym.sym, 0);
}

void StateGame::handle_keyup(SDL_KeyboardEvent &e) {
	states.top()->handle_up(e.keysym.sym, 0);
}

void StateGame::handle_mousedown(SDL_MouseButtonEvent &e) {
	states.top()->handle_down(SDLK_UNKNOWN, e.button);
}

void StateGame::handle_mouseup(SDL_MouseButtonEvent &e) {
	states.top()->handle_up(SDLK_UNKNOWN, e.button);
}

void StateGame::handle_mousewheel(SDL_MouseWheelEvent &e) {
	states.top()->handle_wheel(e);
}