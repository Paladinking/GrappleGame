#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include "globals.h"
#include "texture.h"
#include "exceptions.h"
#include "game.h"
#include "climbGame.h"
//#include "testGame.h"

/**
 * Frees still used global resources and quits SDL and SDL_image.
 * Called atexit.
 **/
void cleanup()
{
	if (gRenderer != NULL) 
	{
		printf("Destroying renderer\n");
		SDL_DestroyRenderer(gRenderer);
	}
	if (gWindow != NULL) 
	{
		printf("Destroying window\n");
		SDL_DestroyWindow(gWindow);
	}
	
	printf("Shutting down...\n");
	IMG_Quit();
    SDL_Quit();
}


int main(int argc, char* args[])
{
	atexit(cleanup);
	at_quick_exit(cleanup);
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Could not initialize SDL, %s\n", SDL_GetError());
		exit(-1);
	}
	int exit_status = 0;
	ClimbGame game;
	try {
		game.create();
		game.run();
	} catch (base_exception &e) {
		printf("%s\n", e.msg.c_str());
		exit_status = -1;
	}
	game.destroy_game();
	
	return exit_status;
}