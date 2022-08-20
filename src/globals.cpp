#include "globals.h"
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

int TILE_SIZE = 8;
int FULL_TILE_HEIGHT = 160;
int FULL_TILE_WIDTH = 80;

bool VERBOSE = false;

std::string ASSETS_ROOT = "assets/";
std::string PLAYER_IMG = "player.png";
std::string MAP_IMG = "map.png";
std::string HOOK_IMG = "ball.png";