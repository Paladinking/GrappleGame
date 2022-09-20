#ifndef GLOBALS_00_H
#define GLOBALS_00_H
#include <SDL.h>
#include <string>

const std::string GLOBALS_PATH = "globals.json";

//Screen dimension constants
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

extern double MAX_GRAVITY_VEL;
extern double GRAVITY_ACCELERATION;
extern double MAX_MOVEMENT_VEL;
extern double MOVEMENT_ACCELERATION;
extern double FRICTION_FACTOR;
extern double AIR_RES_FACTOR;
extern double JUMP_VEL;

extern int PLAYER_FULL_WIDTH;
extern int PLAYER_FULL_HEIGHT;
extern int PLAYER_START_X;
extern int PLAYER_START_Y;
extern int GRAPPLE_LENGTH;
extern double GRAPPLE_SPEED;
extern double GRAPPLE_PULL;
extern double GRAPPLE_RELEASE;

extern bool VERBOSE;

extern std::string ASSETS_ROOT;
extern std::string PLAYER_IMG;
extern std::string MAP_IMG;
extern std::string HOOK_IMG;

extern std::string CONFIG_ROOT;
extern std::string OPTIONS_FILE;

constexpr double PI = 3.141592654;
constexpr double PI_HALF = PI / 2;

extern int TILE_SIZE;
extern int FULL_TILE_HEIGHT;
extern int FULL_TILE_WIDTH;
extern int CAMERA_PAN_REGION;
extern double CAMERA_SPEED;

// SDL globals
extern SDL_Renderer* gRenderer;
extern SDL_Window* gWindow;

void load_globals();

#endif