#include "levelMaker.h"

constexpr int TILE_IMG_SIZE = 32;
constexpr int TILE_IMG_TW = 12;
constexpr int TILE_IMG_TH = 8;
constexpr int TILE_COUNT = 96;
constexpr int TILE_SELECTOR_SIZE = 64;
constexpr int TILE_SELECTOR_TW = 10;
constexpr int TILE_SELECTOR_TH = 10;
constexpr int EDITOR_W = 640;
constexpr int EDITOR_H = 640;
constexpr int SELECTOR_W = 640;
constexpr int SELECTOR_H = EDITOR_H;

void LevelMaker::init() {
	tiles = IMG_Load("assets/tiles/tiles.png");
	
	marker = IMG_Load("assets/marker.png");
	
	window_surface = SDL_GetWindowSurface(gWindow);
	
	click = get_press_input("Mouse Left");
}


void LevelMaker::handle_keydown(SDL_KeyboardEvent e) {
	
}

void LevelMaker::handle_mouseup(SDL_MouseButtonEvent e) {
	if (click->is_targeted(SDLK_UNKNOWN, e.button)) {
		
		printf("ClickUp\n");
	}
}

void LevelMaker::handle_mousedown(SDL_MouseButtonEvent e) {
	if (click->is_targeted(SDLK_UNKNOWN, e.button)) {
		if (e.x > EDITOR_W) {
			int index = (e.x - EDITOR_W) / TILE_SELECTOR_SIZE + (e.y / TILE_SELECTOR_SIZE) * TILE_SELECTOR_TW;
			if (index >= 0 && index < TILE_COUNT) {
				selected = index;
			}
		}
		printf("ClickDown\n");
	}
}

SDL_Rect get_tile_rect(int index) {
	return {(index % TILE_IMG_TW) * TILE_IMG_SIZE, (index / TILE_IMG_TW) * TILE_IMG_SIZE,  TILE_IMG_SIZE, TILE_IMG_SIZE};
}


void LevelMaker::render() {
	SDL_Rect r = {0, 0, SELECTOR_W + EDITOR_W, EDITOR_H};
	SDL_FillRect(window_surface, &r, SDL_MapRGB(window_surface->format, 255, 255, 255));
	for (int i = 0; i < TILE_COUNT; ++i) {
		SDL_Rect tile_rect = get_tile_rect(i);
		SDL_Rect dest = {
			EDITOR_W + (i % TILE_SELECTOR_TW) * TILE_SELECTOR_SIZE, 
			(i / TILE_SELECTOR_TW) * TILE_SELECTOR_SIZE, 
			TILE_SELECTOR_SIZE, TILE_SELECTOR_SIZE
		};
		SDL_BlitScaled(tiles, &tile_rect, window_surface, &dest);
		if (i == selected) {
			SDL_BlitScaled(marker, NULL, window_surface, &dest);
		}
	}
	
	
	
	SDL_UpdateWindowSurface(gWindow);
}


void LevelMaker::destroy_game() {
	Game::destroy_game();
	SDL_FreeSurface(tiles);
}