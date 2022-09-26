#ifndef GLOBALS_00_H
#define GLOBALS_00_H
#include <SDL.h>
#include <string>

const std::string GLOBALS_PATH = "globals.json";

//Screen dimension constants
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

extern bool VERBOSE;

extern std::string ASSETS_ROOT;
extern std::string PLAYER_IMG;
extern std::string MAP_IMG;
extern std::string HOOK_IMG;

extern std::string CONFIG_ROOT;
extern std::string OPTION_FILE;
extern std::string LEVELS_ROOT;
extern std::string LEVELS_FILE;

constexpr double PI = 3.141592654;
constexpr double PI_HALF = PI / 2;

void load_globals();

#endif
