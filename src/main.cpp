#include <vector>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "globals.h"
#include "util/exceptions.h"
#include "game/game.h"
#include "game/climbGame.h"
#include "game/levelMaker.h"

/**
 * Frees still used global resources and quits SDL and SDL_image.
 * Called atexit.
 **/
void cleanup()
{
	if (gRenderer != NULL) 
	{
		std::cout << "Destroying renderer" << std::endl;
		SDL_DestroyRenderer(gRenderer);
	}
	if (gWindow != NULL) 
	{
		std::cout << "Destroying window" << std::endl;
		SDL_DestroyWindow(gWindow);
	}
	std::cout << "Shutting down..." << std::endl;
	IMG_Quit();
    SDL_Quit();
}


/**
 * Run a game, and set the exit status in case of error.
 */
void run_game(Game &game, int* exit_status) {
	try {
		game.create();
		game.run();
	} catch (base_exception &e) {
		std::cout << e.msg << std::endl;
		*exit_status = -1;
	}
	game.destroy_game();
}

int main(int argc, char* args[])
{
	atexit(cleanup);
	at_quick_exit(cleanup);
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "Could not initialize SDL, "  << SDL_GetError() << std::endl;
		exit(-1);
	}

	bool level_maker = false;

	if (argc > 1) {
		for (unsigned i = 1; i < argc; ++i) {
			if (strcmp(args[i], "level_maker") == 0 || strcmp(args[i], "--level_maker") == 0 ) {
				level_maker = true;
				break;
			} 
		}
	}
	
	try {
		load_globals();
	} catch (base_exception &e) {
		std::cout << e.msg << '\n' << "Using default globals" << std::endl;
	}
	
	int exit_status = 0;
	if (level_maker) {
		LevelMaker game;
		run_game(game, &exit_status);
	} else {
		State* state = new ClimbGame();
		StateGame game(state);
		run_game(game, &exit_status);
	}
	return exit_status;
}