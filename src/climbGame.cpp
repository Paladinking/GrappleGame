#include <stdlib.h>
#include <cmath>
#include "climbGame.h"

void ClimbGame::tick(Uint64 delta) {	
	double dDelta = delta / 1000.0;
	//printf("%f\n", 1 / dDelta);
	handle_input(dDelta);
	for (auto e : entities) {
		e->tick(dDelta, tilemap, corners);
	}
}

void ClimbGame::handle_input(double delta) {
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
	const Vector2D &vel = player->get_velocity();
	if (currentKeyStates[SDL_SCANCODE_SPACE]) 
	{
		if (player->on_ground(tilemap)) 
		{
			printf("JUMP}");
			player->add_velocity(0, -vel.y -JUMP_VEL);
			//vel.y = -JUMP_VEL;
		}
	} 
	if (currentKeyStates[SDL_SCANCODE_A] && vel.x > -MAX_MOVEMENT_VEL) 
	{
		player->add_velocity(-MOVEMENT_ACCELERATION * delta, 0);
	} 
	if (currentKeyStates[SDL_SCANCODE_D] && vel.x < MAX_MOVEMENT_VEL) 
	{
		player->add_velocity(MOVEMENT_ACCELERATION * delta, 0);
	}
	if (currentKeyStates[SDL_SCANCODE_E]) {
		if (!grapple_pressed) {
			int world_mouseX = mouseX, world_mouseY = mouseY + camera_y;
			player->fire_grapple(world_mouseX, world_mouseY);
			grapple_pressed = true;
		}
	} else {
		grapple_pressed = false;
	}
	if (currentKeyStates[SDL_SCANCODE_Q]) {
		if (!pull_pressed) {
			player->toggle_pull();
			pull_pressed = true;
		}
	} else {
		pull_pressed = false;
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
	
	/*for (const std::shared_ptr<Corner> &corner : corners) {
		ball.render(corner->x - 2, corner->y -2 - camera_y);
	}*/
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
	
	ball.load_from_file("assets/ball.png");
	ball.set_dimensions(4, 4);
	
	for (int x = 0; x < FULL_TILE_WIDTH; ++x) {
		for (int y = 0; y < FULL_TILE_HEIGHT; ++y) {
			if (!tilemap.is_blocked(x, y)) continue;
			bool top_left = true, top_right = true, botton_left = true, bottom_right = true;
			if (tilemap.is_blocked(x - 1, y)) {
				top_left = false;
				botton_left = false;
			}
			if (tilemap.is_blocked(x + 1, y)) {
				top_right = false;
				bottom_right = false;
			}
			if (tilemap.is_blocked(x, y - 1)) {
				top_left = false;
				top_right = false;
			}
			if (tilemap.is_blocked(x, y + 1)) {
				botton_left = false;
				bottom_right = false;
			}
			double x_pos = (double) x, y_pos = (double) y;
			if (top_left) {
				corners.push_back(std::shared_ptr<Corner>(new Corner(x_pos * TILE_SIZE, y_pos * TILE_SIZE)));
			}
			if (top_right) {
				corners.push_back(std::shared_ptr<Corner>(new Corner((x_pos + 1) * TILE_SIZE, y_pos * TILE_SIZE)));
			}
			if (botton_left) {
				corners.push_back(std::shared_ptr<Corner>(new Corner(x_pos * TILE_SIZE, (y_pos + 1) * TILE_SIZE)));
			}
			if (bottom_right) {
				corners.push_back(std::shared_ptr<Corner>(new Corner((x_pos + 1) * TILE_SIZE, (y_pos + 1) * TILE_SIZE)));
			}
		}
	}
	
	Player* p = new Player();
	
	player.reset(p);
	player->init(window_width / 2, window_height / 2 + camera_y, PLAYER_FULL_WIDTH, PLAYER_FULL_HEIGHT);
	
	entities.push_back(player);
}