#include <stdlib.h>
#include <cmath>
#include "climbGame.h"
#include "json.h"

void load_globals();

void ClimbGame::tick(Uint64 delta) {	
	double dDelta = delta / 1000.0;
	//printf("%f\n", 1 / dDelta);
	handle_input(dDelta);
	for (auto e : entities) {
		e->tick(dDelta, tilemap, corners);
	}
	const Vector2D &pos = player->get_position();
	double camera_y_delta = pos.y - camera_y;
	if (camera_y_delta < CAMERA_PAN_REGION) {
		camera_y -= CAMERA_SPEED * dDelta;
		if (camera_y < camera_y_min) camera_y = camera_y_min;
	}
	else if (camera_y_delta > window_height - 250) {
		camera_y += CAMERA_SPEED * dDelta;
		if (camera_y > camera_y_max) camera_y = camera_y_max;
	}
}

void ClimbGame::handle_input(double delta) {
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
	const Vector2D &vel = player->get_velocity();
	if (currentKeyStates[SDL_SCANCODE_SPACE]) 
	{
		if (player->on_ground(tilemap)) 
		{
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
	if ((mouseButton & SDL_BUTTON_LMASK) != 0) {
		if (!grapple_pressed) {
			int world_mouseX = mouseX, world_mouseY = mouseY + camera_y;
			player->fire_grapple(world_mouseX, world_mouseY);
			grapple_pressed = true;
		}
	} else {
		grapple_pressed = false;
	}
	if (currentKeyStates[SDL_SCANCODE_LSHIFT]) {
		player->return_grapple();
	}
	if (currentKeyStates[SDL_SCANCODE_E]) {
		if (!release_pressed) {
			player->set_release(true);
			release_pressed = true;
		}
	} else {
		if (release_pressed) {
			player->set_release(false);
			release_pressed = false;
		}
	}
	
	if (currentKeyStates[SDL_SCANCODE_Q]) {
		if (!pull_pressed) {
			player->set_pull(true);
			pull_pressed = true;
		}
	} else {
		if (pull_pressed) {
			player->set_pull(false);
			pull_pressed = false;
		}
	}
	
	if (currentKeyStates[SDL_SCANCODE_ESCAPE]) {
		exit_game();
	}
}

void ClimbGame::render() {
	SDL_RenderClear(gRenderer);
	
	render_tilemap();
	player->render((int)camera_y);
	
	SDL_RenderPresent(gRenderer);
}

void ClimbGame::render_tilemap() {
	int camera_y_tile = ((int)camera_y) / TILE_SIZE;
	for (int x = 0; x < visible_tiles_x; x++) {
		for (int y = camera_y_tile - 1; y < camera_y_tile + visible_tiles_y + 1; y++) {
			if (tilemap.is_blocked(x, y)) {
				SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
			} else {
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
			}
			SDL_Rect fillRect = {x * TILE_SIZE, y * TILE_SIZE - ((int)camera_y), TILE_SIZE, TILE_SIZE};
			SDL_RenderFillRect( gRenderer, &fillRect );
		}
	}
	
	/*for (const std::shared_ptr<Corner> &corner : corners) {
		ball.render(corner->x - 2, corner->y -2 - camera_y);
	}*/
}

void ClimbGame::init() {
	load_globals();
	tilemap.load_from_image(ASSETS_ROOT + MAP_IMG);
	tilemap.set_tilesize(TILE_SIZE);
	if (tilemap.get_width() != FULL_TILE_WIDTH || tilemap.get_height() != FULL_TILE_HEIGHT) 
	{
		throw Game_exception("Map image has wrong dimensions.");
	}
	visible_tiles_x = window_width / TILE_SIZE;
	visible_tiles_y = window_height / TILE_SIZE;
	camera_y = PLAYER_START_Y;
	camera_y_max = TILE_SIZE * (FULL_TILE_HEIGHT - visible_tiles_y);
	camera_y_min = 0;
	if (camera_y < camera_y_min) camera_y = camera_y_min;
	if (camera_y > camera_y_max) camera_y = camera_y_max;
	
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
	player->init(PLAYER_START_X, PLAYER_START_Y, PLAYER_FULL_WIDTH, PLAYER_FULL_HEIGHT);
	
	entities.push_back(player);
}

#define SET_IF_EXISTS(obj, T, S) if (obj.has_key_of_type<T>(#S)) S = obj.get<T>(#S)

void load_globals() {
	JsonObject obj;
	try {
		obj = json::read_from_file(GLOBALS_PATH);
	} catch (base_exception &e) {
		printf("%s\nUsing default globals.\n", e.msg.c_str());
		return;
	}
	SET_IF_EXISTS(obj, double, MAX_GRAVITY_VEL);
	SET_IF_EXISTS(obj, double, GRAVITY_ACCELERATION);
	SET_IF_EXISTS(obj, double, MAX_MOVEMENT_VEL);
	SET_IF_EXISTS(obj, double, MOVEMENT_ACCELERATION);
	SET_IF_EXISTS(obj, double, FRICTION_FACTOR);
	SET_IF_EXISTS(obj, double, AIR_RES_FACTOR);
	SET_IF_EXISTS(obj, double, JUMP_VEL);
	

	SET_IF_EXISTS(obj, int, PLAYER_FULL_WIDTH);
	SET_IF_EXISTS(obj, int, PLAYER_FULL_HEIGHT);
	SET_IF_EXISTS(obj, int, PLAYER_START_X);
	SET_IF_EXISTS(obj, int, PLAYER_START_Y);
	SET_IF_EXISTS(obj, int, GRAPPLE_LENGTH);
	SET_IF_EXISTS(obj, double, GRAPPLE_SPEED);
	SET_IF_EXISTS(obj, double, GRAPPLE_PULL);
	SET_IF_EXISTS(obj, double, GRAPPLE_RELEASE);
	
	SET_IF_EXISTS(obj, int, TILE_SIZE);
	SET_IF_EXISTS(obj, int, FULL_TILE_HEIGHT);
	SET_IF_EXISTS(obj, int, FULL_TILE_WIDTH);
	SET_IF_EXISTS(obj, int, CAMERA_PAN_REGION);
	SET_IF_EXISTS(obj, double, CAMERA_SPEED);
	SET_IF_EXISTS(obj, bool, VERBOSE);
	
	SET_IF_EXISTS(obj, std::string, ASSETS_ROOT);
	SET_IF_EXISTS(obj, std::string, PLAYER_IMG);
	SET_IF_EXISTS(obj, std::string, MAP_IMG);
	SET_IF_EXISTS(obj, std::string, HOOK_IMG);
	
	if (!VERBOSE) return;
	
	printf("MAX_GRAVITY_VEL: %f\n", MAX_GRAVITY_VEL);
	printf("MAX_MOVEMENT_VEL: %f\n", MAX_MOVEMENT_VEL);
	printf("GRAVITY_ACCELERATION: %f\n", GRAVITY_ACCELERATION);
	printf("MOVEMENT_ACCELERATION: %f\n", MOVEMENT_ACCELERATION);
	printf("FRICTION_FACTOR: %f\n", FRICTION_FACTOR);
	printf("AIR_RES_FACTOR: %f\n", AIR_RES_FACTOR);
	printf("JUMP_VEL: %f\n", JUMP_VEL);
	printf("PLAYER_FULL_WIDTH: %d\n", PLAYER_FULL_WIDTH);
	printf("PLAYER_FULL_HEIGHT: %d\n", PLAYER_FULL_HEIGHT);
	printf("PLAYER_START_X: %d\n", PLAYER_START_X);
	printf("PLAYER_START_Y: %d\n", PLAYER_START_Y);
	printf("GRAPPLE_LENGTH: %d\n", GRAPPLE_LENGTH);
	printf("GRAPPLE_SPEED: %f\n", GRAPPLE_SPEED);
	printf("GRAPPLE_PULL: %f\n", GRAPPLE_PULL);
	printf("GRAPPLE_RELEASE: %f\n", GRAPPLE_RELEASE);
	printf("TILE_SIZE: %d\n", TILE_SIZE);
	printf("FULL_TILE_HEIGHT: %d\n", FULL_TILE_HEIGHT);
	printf("FULL_TILE_WIDTH: %d\n", FULL_TILE_WIDTH);
	printf("CAMERA_PAN_REGION: %d\n", CAMERA_PAN_REGION);
	printf("CAMERA_SPEED: %f\n", CAMERA_SPEED);
	printf("ASSETS_ROOT: %s\n", ASSETS_ROOT.c_str());
	printf("PLAYER_IMG: %s\n", PLAYER_IMG.c_str());
	printf("MAP_IMG: %s\n", MAP_IMG.c_str());
	printf("HOOK_IMG: %s\n", HOOK_IMG.c_str());
}