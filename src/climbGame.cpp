#include <stdlib.h>
#include "climbGame.h"

void ClimbGame::tick(Uint64 delta) {
	double dDelta = delta / 1000.0;
	handle_input(dDelta);
	for (auto e : entities) {
		e->add_velocity(0, 1500.0 * dDelta); // Apply gravity
		e->tick(dDelta, tilemap);
	}
}


void ClimbGame::handle_input(double delta) {
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
	
}

void ClimbGame::render() {
	SDL_RenderClear(gRenderer);
	
	render_tilemap();
	player->render(camera_y);
	
	SDL_RenderPresent(gRenderer);
}

void ClimbGame::render_tilemap() {
	int camera_y_tile = camera_y / TILE_SIZE;
	for (int x = 0; x < visible_tiles_x; x++) {
		for (int y = camera_y_tile - 1; y < camera_y_tile + visible_tiles_y + 1; y++) {
			if (tilemap.is_blocked(x, y)) {
				SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
			} else {
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
			}
			SDL_Rect fillRect = {x * TILE_SIZE, y * TILE_SIZE - camera_y, TILE_SIZE, TILE_SIZE};
			SDL_RenderFillRect( gRenderer, &fillRect );
		}
	}
}

void ClimbGame::init() {
	tilemap.load_from_image("assets/map.png");
	tilemap.set_tilesize(TILE_SIZE);
	if (tilemap.get_width() != FULL_TILE_WIDTH || tilemap.get_height() != FULL_TILE_HEIGHT) 
	{
		throw Game_exception("Map image has wrong dimensions.");
	}
	visible_tiles_x = window_width / TILE_SIZE;
	visible_tiles_y = window_height / TILE_SIZE;
	camera_y = TILE_SIZE * (FULL_TILE_HEIGHT - visible_tiles_y);
	
	Player* p = new Player();
	
	player.reset(p);
	player->init(window_width / 2, window_height / 2 + camera_y, 40, 60);
	
	entities.push_back(player);
}