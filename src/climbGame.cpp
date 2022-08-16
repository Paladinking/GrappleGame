#include <stdlib.h>
#include "climbGame.h"

void ClimbGame::tick(Uint64 delta) {
	double dDelta = delta / 1000.0;
	handle_input(dDelta);
	for (auto e : entities) {
		Vector2D &vel = e->get_velocity();
		if (vel.y < MAX_GRAVITY_VEL && !e->on_ground(tilemap)) {
			
			e->add_velocity(0, GRAVITY_ACCELERATION * dDelta); // Apply gravity
		}
		double factor = FRICTION_FACTOR * dDelta;
		if (vel.x > 0)
		{
			vel.x -= factor > vel.x ? vel.x : factor;
		}	
		else 
		{
			vel.x -= factor > -vel.x ? vel.x : -factor;
		}
		
		e->tick(dDelta, tilemap);
	}
}


void ClimbGame::handle_input(double delta) {
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
	Vector2D &vel = player->get_velocity();
	if (currentKeyStates[SDL_SCANCODE_SPACE]) 
	{
		if (player->on_ground(tilemap)) 
		{
			player->add_velocity(0, -JUMP_VEL);
		}
	} 
	if (currentKeyStates[SDL_SCANCODE_LEFT] && vel.x > -MAX_MOVEMENT_VEL) 
	{
		player->add_velocity(-MOVEMENT_ACCELERATION * delta, 0);
	} 
	if (currentKeyStates[SDL_SCANCODE_RIGHT] && vel.x < MAX_MOVEMENT_VEL) 
	{
		player->add_velocity(MOVEMENT_ACCELERATION * delta, 0);
	}
	if (currentKeyStates[SDL_SCANCODE_ESCAPE]) {
		exit_game();
	}
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
	player->init(window_width / 2, window_height / 2 + camera_y, PLAYER_FULL_WIDTH, PLAYER_FULL_HEIGHT);
	
	entities.push_back(player);
}