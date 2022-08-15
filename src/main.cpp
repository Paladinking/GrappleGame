#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include "globals.h"
#include "texture.h"
#include "exceptions.h"
#include "game.h"
#include "ballGame.h"

void cleanup() {
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
    //Quit SDL subsystems
    SDL_Quit();
}


int main( int argc, char* args[] )
{
	atexit(cleanup);
	at_quick_exit(cleanup);
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Could not initialize SDL, %s\n", SDL_GetError());
		exit(-1);
	}
	
	BallGame game;
	try {
		game.create();
	} catch (SDL_exception &e) {
		printf("%s\n", e.msg.c_str());
	}
	game.run();
	game.destroy_game();
	
	return 0;
		/*
		SDL_RenderClear(gRenderer);
		
		textures[currentTexture].render(0, 0);
		
		ballTexture.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 );
		
		SDL_RenderPresent(gRenderer);
	*/

}