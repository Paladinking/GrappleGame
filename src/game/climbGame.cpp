#include <cmath>
#include <iostream>
#include "climbGame.h"
#include "file/json.h"
#include "config.h"

constexpr double MAX_MOVEMENT_VEL = 350.0;
constexpr double MOVEMENT_ACCELERATION = 2200.0;

constexpr int PLAYER_FULL_WIDTH = 24;
constexpr int PLAYER_FULL_HEIGHT = 40;
constexpr int PLAYER_START_X = 320;
constexpr int PLAYER_START_Y = 960;

constexpr int CAMERA_PAN_REGION = 200;
constexpr double CAMERA_SPEED = 1000.0;

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
		camera_y -= std::min(CAMERA_SPEED * dDelta, CAMERA_PAN_REGION - camera_y_delta);
		if (camera_y < camera_y_min) camera_y = camera_y_min;
	}
	else if (camera_y_delta > SCREEN_HEIGHT - CAMERA_PAN_REGION) {
		camera_y += std::min(CAMERA_SPEED * dDelta, camera_y_delta - SCREEN_HEIGHT + CAMERA_PAN_REGION);
		if (camera_y > camera_y_max) camera_y = camera_y_max;
	}
}

void ClimbGame::handle_input(double delta, StateStatus& res) {
	const Vector2D &vel = player->get_velocity(); 
	if (left_input->is_pressed(window_state->keyboard_state, window_state->mouse_mask) && vel.x > -MAX_MOVEMENT_VEL) {
		player->add_acceleration(-MOVEMENT_ACCELERATION, 0);
	}
	if (right_input->is_pressed(window_state->keyboard_state, window_state->mouse_mask) && vel.x < MAX_MOVEMENT_VEL) 
	{
		player->add_acceleration(MOVEMENT_ACCELERATION, 0);
	}


	if (do_grapple) { //Grapple is a push input, but mouse_pos is updated after. (Change?)
		do_grapple = false;
		const int world_mouseX = window_state->mouseX - game_viewport.x;
		const int world_mouseY = window_state->mouseY + static_cast<int>(camera_y) - game_viewport.y;
		player->fire_grapple(world_mouseX, world_mouseY);
	}
	
	if (window_state->keyboard_state[SDL_SCANCODE_ESCAPE]) {
		res.action = StateStatus::POP;
	}
}

void ClimbGame::render() {
	SDL_RenderSetViewport(gRenderer, &game_viewport);
	SDL_SetRenderDrawColor(gRenderer, 0xAA, 0xAA, 0xAA, 0xFF);
	SDL_RenderClear(gRenderer);
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(gRenderer, NULL);
	
	int camera_y_tile = (static_cast<int>(camera_y)) / level.get_tilesize();
	level.render(static_cast<int>(camera_y));
	player->render(static_cast<int>(camera_y));
	
	SDL_RenderPresent(gRenderer);
}

void ClimbGame::init(WindowState* ws) {
	State::init(ws);
	create_inputs();
	std::pair<std::string, std::string> lvl1 = config::get_level(0);
	level.set_screen_size(SCREEN_WIDTH, SCREEN_HEIGHT);
	level.load_from_file(lvl1.first, lvl1.second);
	
	game_viewport = {
		window_state->screen_width / 2 - SCREEN_WIDTH / 2, 
		window_state->screen_height / 2 - SCREEN_HEIGHT / 2,
		SCREEN_WIDTH,
		SCREEN_HEIGHT
	};
	
	const int tile_size = level.get_tilesize();

	visible_tiles_x = SCREEN_WIDTH / tile_size;
	visible_tiles_y = SCREEN_HEIGHT / tile_size;
	camera_y = PLAYER_START_Y;
	camera_y_max = tile_size * (level.get_height() - visible_tiles_y);
	camera_y_min = 0;
	if (camera_y < camera_y_min) camera_y = camera_y_min;
	if (camera_y > camera_y_max) camera_y = camera_y_max;

	Player* p = new Player();
	
	player.reset(p);

	player_template.reset(EntityTemplate::from_json(config::get_template("Player")));
	player->init(*player_template);

	player->set_position(PLAYER_START_X, PLAYER_START_Y);
	entities.push_back(player);
	
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xAA, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);
}



void ClimbGame::create_inputs() {
	const JsonObject& controls = config::get_bindings(bindings::CLIMBGAME.key);

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
		player->jump();
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