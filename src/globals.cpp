#include "globals.h"
#include "file/json.h"
#include <iostream>

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 640;

double MAX_GRAVITY_VEL = 700.0;
double GRAVITY_ACCELERATION = 3000.0;
double MAX_MOVEMENT_VEL = 350.0;
double MOVEMENT_ACCELERATION = 2200.0;
double FRICTION_FACTOR = 350.0;
double AIR_RES_FACTOR = 0.001;
double JUMP_VEL = 800;

int PLAYER_FULL_WIDTH = 24;
int PLAYER_FULL_HEIGHT = 40;
int PLAYER_START_X = 320;
int PLAYER_START_Y = 960;
int GRAPPLE_LENGTH = 350;
double GRAPPLE_SPEED = 800.0;
double GRAPPLE_PULL = 5000.0;
double GRAPPLE_RELEASE = 200.0;

int TILE_SIZE = 8;
int FULL_TILE_HEIGHT = 160;
int FULL_TILE_WIDTH = 80;
int CAMERA_PAN_REGION = 250;
double CAMERA_SPEED = 1000.0;

bool VERBOSE = false;

std::string ASSETS_ROOT = "assets/";
std::string PLAYER_IMG = "player.png";
std::string MAP_IMG = "map.png";
std::string HOOK_IMG = "ball.png";

std::string CONFIG_ROOT = "config/";
std::string OPTIONS_FILE = "options.json";

#define SET_IF_EXISTS(obj, T, S) if (obj.has_key_of_type<T>(#S)) S = obj.get<T>(#S)

void load_globals() {
	JsonObject obj = json::read_from_file(GLOBALS_PATH);
	
	SET_IF_EXISTS(obj, int, SCREEN_WIDTH);
	SET_IF_EXISTS(obj, int, SCREEN_HEIGHT);

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
	
	std::cout << "SCREEN_WIDTH: " 			<< SCREEN_WIDTH << '\n';
	std::cout << "SCREEN_HEIGHT: " 			<< SCREEN_HEIGHT << '\n';
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