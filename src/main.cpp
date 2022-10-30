#include <vector>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "util/exceptions.h"
#include "engine/game.h"
#include "game/climbGame.h"
#include "game/levelMaker.h"
#include "game/menu.h"
#include "game/config.h"

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
	TTF_Quit();
    SDL_Quit();
}


/**
 * Run a game, and set the exit status in case of error.
 */
void run_game(Game &game, int& exit_status) {
	try {
		game.create();
		game.run();
	} catch (const base_exception &e) {
		std::cout << e.msg << std::endl;
		exit_status = -1;
	} catch (const std::logic_error &e) {
		std::cout << e.what() << std::endl;
		exit_status = -2;
	}
	game.destroy_game();
}

/**
 * Initialize SDL, engine and config.
 */
void init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "Could not initialize SDL, "  << SDL_GetError() << std::endl;
		exit(-1);
	}
	
	if (TTF_Init() != 0) {
		std::cout << "Could not initialize SDL_tff, " << TTF_GetError() << std::endl;
		exit(-2);
	}
	
	try {
		engine::init();
		config::init();
	} catch (const base_exception &e) {
		std::cout << e.msg << std::endl;
		exit(-3);
	}
}

int main(int argc, char* args[])
{
	atexit(cleanup);
	at_quick_exit(cleanup);

	bool level_maker = false;

	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			if (strcmp(args[i], "level_maker") == 0 || strcmp(args[i], "--level_maker") == 0 ) {
				level_maker = true;
				break;
			} 
		}
	}
	
	init();

	int exit_status = 0;
	if (level_maker) {
		return -4;
	}

	StateGame game(new MainMenu(), 100, 100, "Grapple Game");
	run_game(game, exit_status);

	return exit_status;
}