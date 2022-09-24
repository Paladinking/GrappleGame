#include <cmath>
#include <iostream>
#include "climbGame.h"
#include "file/json.h"
#include "config.h"

void ClimbGame::tick(const Uint64 delta, StateStatus& res) {
	if (delta == 0) return;
	double dDelta = delta / 1000.0;

	handle_input(dDelta, res);
	for (auto e : entities) {
		e->tick(dDelta, level);
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

void ClimbGame::handle_input(double delta, StateStatus& res) {
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
	const Vector2D &vel = player->get_velocity(); 
	if (left_input->is_pressed(currentKeyStates, mouseButton) && vel.x > -MAX_MOVEMENT_VEL) {
		player->add_velocity(-MOVEMENT_ACCELERATION * delta, 0);
	}
	if (right_input->is_pressed(currentKeyStates, mouseButton) && vel.x < MAX_MOVEMENT_VEL) 
	{
		player->add_velocity(MOVEMENT_ACCELERATION * delta, 0);
	}


	if (do_grapple) { //Grapple is a push input, but mouse_pos is updated after. (Change?)
		do_grapple = false;
		int world_mouseX = mouseX, world_mouseY = mouseY + static_cast<int>(camera_y);
		player->fire_grapple(world_mouseX, world_mouseY);
	}
	
	if (currentKeyStates[SDL_SCANCODE_ESCAPE]) {
		res.action = StateStatus::POP;
	}
}

void ClimbGame::render() {
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);
	int camera_y_tile = (static_cast<int>(camera_y)) / TILE_SIZE;
	//tilemapTexture.render(0, 0, 0, camera_y, window_width, window_height);
	level.render(static_cast<int>(camera_y));
	//render_tilemap();
	player->render(static_cast<int>(camera_y));
	
	SDL_RenderPresent(gRenderer);
}

void ClimbGame::render_tilemap() {
	int camera_y_tile = static_cast<int>(camera_y) / TILE_SIZE;
	for (int x = 0; x < visible_tiles_x; x++) {
		for (int y = camera_y_tile - 1; y < camera_y_tile + visible_tiles_y + 1; y++) {
			if (level.is_blocked(x, y)) {
				SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
			} else {
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
			}
			SDL_Rect fillRect = {x * TILE_SIZE, y * TILE_SIZE - static_cast<int>(camera_y), TILE_SIZE, TILE_SIZE};
			SDL_RenderFillRect( gRenderer, &fillRect );
		}
	}
	
	/*for (const std::shared_ptr<Corner> &corner : corners) {
		ball.render(corner->x - 2, corner->y -2 - camera_y);
	}*/
}

void ClimbGame::init() {
	State::init();
	create_inputs();
	level.set_window_size(window_width, window_height);
	level.load_from_file("config/level1");
	//tilemap.load_from_image(ASSETS_ROOT + MAP_IMG);
	
	level.set_tilesize(TILE_SIZE);

	if (level.get_width() != FULL_TILE_WIDTH || level.get_height() != FULL_TILE_HEIGHT) 
	{
		throw game_exception("Map image has wrong dimensions.");
	}
	visible_tiles_x = window_width / TILE_SIZE;
	visible_tiles_y = window_height / TILE_SIZE;
	camera_y = PLAYER_START_Y;
	camera_y_max = TILE_SIZE * (FULL_TILE_HEIGHT - visible_tiles_y);
	camera_y_min = 0;
	if (camera_y < camera_y_min) camera_y = camera_y_min;
	if (camera_y > camera_y_max) camera_y = camera_y_max;
	
	tilemapTexture.load_from_file("assets/mapImage.png", FULL_TILE_WIDTH * TILE_SIZE, FULL_TILE_HEIGHT * TILE_SIZE);
	tilemapTexture.set_dimensions(window_width, window_height);
	
	Player* p = new Player();
	
	player.reset(p);
	player->init(PLAYER_START_X, PLAYER_START_Y, PLAYER_FULL_WIDTH, PLAYER_FULL_HEIGHT);
	
	entities.push_back(player);
}

void ClimbGame::create_inputs() {
	JsonObject& options = config::get_options();
	const JsonObject& controls = options.get<JsonObject>(bindings::KEY_NAME);

	left_input = get_hold_input(controls.get<std::string>("left"), "None");
	right_input = get_hold_input(controls.get<std::string>("right"), "None");
	grapple_input = get_press_input(controls.get<std::string>("grapple"), "None");
	pull_input = get_press_input(controls.get<std::string>("pull"), "None");
	release_input = get_press_input(controls.get<std::string>("release"), "None");
	jump_input = get_press_input(controls.get<std::string>("jump"), "None");
	return_input = get_press_input(controls.get<std::string>("return_grapple"), "None");
}

void ClimbGame::handle_down(const SDL_Keycode key, const Uint8 mouse) {
	if (grapple_input->is_targeted(key, mouse)) {
		do_grapple = true;
	}
	if (pull_input->is_targeted(key, mouse)) {
		player->set_pull(true);
	} 
	if (release_input->is_targeted(key, mouse)) {
		player->set_release(true);
	}
	if (jump_input->is_targeted(key, mouse)) {
		if (player->on_ground(level)) 
		{
			const Vector2D &vel = player->get_velocity();
			player->add_velocity(0, -vel.y -JUMP_VEL);
			//vel.y = -JUMP_VEL;
		}
	}
	if (return_input->is_targeted(key, mouse)) {
		player->return_grapple();
	}
}

void ClimbGame::handle_up(const SDL_Keycode key, const Uint8 mouse) {
	if (pull_input->is_targeted(key, mouse)) {
		player->set_pull(false);
	} 
	if (release_input->is_targeted(key, mouse)) {
		player->set_release(false);
	}
}