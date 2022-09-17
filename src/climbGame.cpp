#include <cmath>
#include <iostream>
#include "climbGame.h"
#include "json.h"

void load_globals();

void ClimbGame::tick(Uint64 delta) {
	if (delta == 0) return;
	double dDelta = delta / 1000.0;

	handle_input(dDelta);
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

void ClimbGame::handle_input(double delta) {
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
		int world_mouseX = mouseX, world_mouseY = mouseY + camera_y;
		player->fire_grapple(world_mouseX, world_mouseY);
	}
	
	if (currentKeyStates[SDL_SCANCODE_ESCAPE]) {
		exit_game();
	}
}

void ClimbGame::render() {
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);
	int camera_y_tile = ((int)camera_y) / TILE_SIZE;
	//tilemapTexture.render(0, 0, 0, camera_y, window_width, window_height);
	level.render((int)camera_y);
	//render_tilemap();
	player->render((int)camera_y);
	
	SDL_RenderPresent(gRenderer);
}

void ClimbGame::render_tilemap() {
	int camera_y_tile = ((int)camera_y) / TILE_SIZE;
	for (int x = 0; x < visible_tiles_x; x++) {
		for (int y = camera_y_tile - 1; y < camera_y_tile + visible_tiles_y + 1; y++) {
			if (level.is_blocked(x, y)) {
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
	JsonObject options, controls;
	try {
		options = json::read_from_file(CONFIG_ROOT + OPTIONS_FILE);
		if (options.has_key_of_type<JsonObject>("CONTROLS")) {
			controls = options.get<JsonObject>("CONTROLS");
		} else {
			std::cout << "No controls in options\nUsing default bindings" << std::endl;
		}
	} catch (base_exception &e){
		std::cout << e.msg << '\n' << "Using default bindings" << std::endl;
	}
	left_input = get_hold_input(controls.get_default("left", input::LEFT), input::LEFT);
	right_input = get_hold_input(controls.get_default("right", input::RIGHT), input::RIGHT);
	grapple_input = get_press_input(controls.get_default("grapple", input::GRAPPLE), input::GRAPPLE);
	pull_input = get_press_input(controls.get_default("pull", input::PULL), input::PULL);
	release_input = get_press_input(controls.get_default("release", input::RELEASE), input::RELEASE);
	jump_input = get_press_input(controls.get_default("jump", input::JUMP), input::JUMP);
	return_input = get_press_input(controls.get_default("return", input::RETURN), input::RETURN);
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

void ClimbGame::handle_keydown(SDL_KeyboardEvent e) {
	handle_down(e.keysym.sym, 0);
}

void ClimbGame::handle_keyup(SDL_KeyboardEvent e) {
	handle_up(e.keysym.sym, 0);
}

void ClimbGame::handle_mousedown(SDL_MouseButtonEvent e) {
	handle_down(SDLK_UNKNOWN, e.button);
}
void ClimbGame::handle_mouseup(SDL_MouseButtonEvent e) {
	handle_up(SDLK_UNKNOWN, e.button);
}

#define SET_IF_EXISTS(obj, T, S) if (obj.has_key_of_type<T>(#S)) S = obj.get<T>(#S)

void load_globals() {
	JsonObject obj;
	try {
		obj = json::read_from_file(GLOBALS_PATH);
	} catch (base_exception &e) {
		std::cout << e.msg << '\n' << "Using default globals" << std::endl;
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
	
	SET_IF_EXISTS(obj, std::string, CONFIG_ROOT);
	SET_IF_EXISTS(obj, std::string, OPTIONS_FILE);
	
	if (!VERBOSE) return;
	
	std::cout << "MAX_GRAVITY_VEL: " 		<< MAX_GRAVITY_VEL << '\n';
	std::cout << "MAX_GRAVITY_VEL: " 		<< MAX_GRAVITY_VEL << '\n';
	std::cout << "MAX_MOVEMENT_VEL: " 		<< MAX_MOVEMENT_VEL << '\n';
	std::cout << "GRAVITY_ACCELERATION: "	<< GRAVITY_ACCELERATION << '\n';
	std::cout << "MOVEMENT_ACCELERATION: "  << MOVEMENT_ACCELERATION << '\n';
	std::cout << "FRICTION_FACTOR: " 		<< FRICTION_FACTOR << '\n';
	std::cout << "AIR_RES_FACTOR: " 		<< AIR_RES_FACTOR << '\n';
	std::cout << "JUMP_VEL: " 				<< JUMP_VEL << '\n';
	std::cout << "PLAYER_FULL_WIDTH: " 		<< PLAYER_FULL_WIDTH << '\n';
	std::cout << "PLAYER_FULL_HEIGHT: " 	<< PLAYER_FULL_HEIGHT << '\n';
	std::cout << "PLAYER_START_X: "			<< PLAYER_START_X << '\n';
	std::cout << "PLAYER_START_Y: " 		<< PLAYER_START_Y << '\n';
	std::cout << "GRAPPLE_LENGTH: " 		<< GRAPPLE_LENGTH << '\n';
	std::cout << "GRAPPLE_SPEED:" 			<< GRAPPLE_SPEED << '\n';
	std::cout << "GRAPPLE_PULL: " 			<< GRAPPLE_PULL << '\n';
	std::cout << "GRAPPLE_RELEASE: " 		<< GRAPPLE_RELEASE << '\n';
	std::cout << "TILE_SIZE: " 				<< TILE_SIZE << '\n';
	std::cout << "FULL_TILE_HEIGHT: " 		<< FULL_TILE_HEIGHT << '\n';
	std::cout << "FULL_TILE_WIDTH: " 		<< FULL_TILE_WIDTH << '\n';
	std::cout << "CAMERA_PAN_REGION: " 		<< CAMERA_PAN_REGION << '\n';
	std::cout << "CAMERA_SPEED: " 			<< CAMERA_SPEED << '\n';
	std::cout << "ASSETS_ROOT: " 			<< ASSETS_ROOT << '\n';
	std::cout << "PLAYER_IMG: " 			<< PLAYER_IMG << '\n';
	std::cout << "MAP_IMG: " 				<< MAP_IMG << '\n';
	std::cout << "HOOK_IMG: " 				<< HOOK_IMG << '\n';
	std::cout << "CONFIG_ROOT: " 			<< CONFIG_ROOT << '\n';
	std::cout << "OPTIONS_FILE: " 			<< OPTIONS_FILE << '\n';
	std::cout << std::flush;
}