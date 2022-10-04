#include "globals.h"
#include "file/json.h"
#include <iostream>

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 640;

bool VERBOSE = false;

std::string ASSETS_ROOT = "assets/";
std::string PLAYER_IMG = "player.png";
std::string MAP_IMG = "map.png";
std::string HOOK_IMG = "ball.png";

std::string CONFIG_ROOT = "config/";
std::string LEVELS_ROOT = "assets/levels/";
std::string OPTION_FILE = "options.json";
std::string LEVELS_FILE = "levels.json";

#define SET_IF_EXISTS(obj, T, S) if (obj.has_key_of_type<T>(#S)) S = obj.get<T>(#S)

void load_globals() {
	try {
		JsonObject obj = json::read_from_file(GLOBALS_PATH);
		SET_IF_EXISTS(obj, int, SCREEN_WIDTH);
		SET_IF_EXISTS(obj, int, SCREEN_HEIGHT);
		SET_IF_EXISTS(obj, bool, VERBOSE);
		SET_IF_EXISTS(obj, std::string, ASSETS_ROOT);
		SET_IF_EXISTS(obj, std::string, PLAYER_IMG);
		SET_IF_EXISTS(obj, std::string, MAP_IMG);
		SET_IF_EXISTS(obj, std::string, HOOK_IMG);
		SET_IF_EXISTS(obj, std::string, CONFIG_ROOT);
		SET_IF_EXISTS(obj, std::string, LEVELS_ROOT);
		SET_IF_EXISTS(obj, std::string, OPTION_FILE);
		SET_IF_EXISTS(obj, std::string, LEVELS_FILE);
	} catch(const base_exception& e) {
		std::cout << e.msg << '\n' << "Using default globals" << std::endl;
	}
	ASSETS_ROOT = PROJECT_ROOT + ASSETS_ROOT;
	CONFIG_ROOT = PROJECT_ROOT + CONFIG_ROOT;
	LEVELS_ROOT = PROJECT_ROOT + LEVELS_ROOT;

	if (!VERBOSE) return;

	std::cout << "SCREEN_WIDTH: " 			<< SCREEN_WIDTH << '\n';
	std::cout << "SCREEN_HEIGHT: " 			<< SCREEN_HEIGHT << '\n';
	std::cout << "ASSETS_ROOT: " 			<< ASSETS_ROOT << '\n';
	std::cout << "PLAYER_IMG: " 			<< PLAYER_IMG << '\n';
	std::cout << "MAP_IMG: " 				<< MAP_IMG << '\n';
	std::cout << "HOOK_IMG: " 				<< HOOK_IMG << '\n';
	std::cout << "CONFIG_ROOT: " 			<< CONFIG_ROOT << '\n';
	std::cout << "LEVELS_ROOT: " 			<< LEVELS_ROOT << '\n';
	std::cout << "OPTION_FILE: " 			<< OPTION_FILE << '\n';
	std::cout << "LEVELS_FILE: " 			<< LEVELS_FILE << '\n';
	std::cout << std::flush;
}