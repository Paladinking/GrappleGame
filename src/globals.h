#ifndef GLOBALS_00_H
#define GLOBALS_00_H
#include <SDL.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

const double MAX_GRAVITY_VEL = 700.0;
const double GRAVITY_ACCELERATION = 2500.0;
const double MAX_MOVEMENT_VEL = 350.0;
const double MOVEMENT_ACCELERATION = 2200.0;
const double FRICTION_FACTOR = 1200.0;
const double JUMP_VEL = 700;

const int PLAYER_FULL_WIDTH = 24;
const int PLAYER_FULL_HEIGHT = 40;
const int GRAPPLE_SPEED = 800.0;
const int GRAPPLE_LENGTH = 500;

const int TILE_SIZE = 8;
const int FULL_TILE_HEIGHT = 160;
const int FULL_TILE_WIDTH = 80;

// SDL globals
extern SDL_Renderer* gRenderer;
extern SDL_Window* gWindow;

#endif