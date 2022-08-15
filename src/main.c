#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include "list.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init(SDL_Window**);

void loadData(SDL_Texture* surface[]);

void close();

//The window we'll be rendering to
SDL_Window* window;
SDL_Renderer* renderer;

List textures;

void output_error (bool do_exit, const char * format, ...)
{
	va_list args;
	va_start (args, format);
	vprintf(format, args);
	va_end (args);
	
	if (do_exit) {
		close();
		exit(-1);
	}
}

enum Direction
{
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
	DIR_NONE,
	DIR_TOTAL
};


int main( int argc, char* args[] )
{
	
	list_init(&textures);

	//The image we will load and show on the screen
	
	if (!init(&window)) {
		return 1;
	}
	
	SDL_Texture* directionTextures[DIR_TOTAL];
	
	loadData(directionTextures);

	bool running = true;
	
	SDL_Texture* currentTexture = directionTextures[DIR_NONE];
	
	while (running) 
	{
		SDL_Event e;
		
		while (SDL_PollEvent(&e)) 
		{
			switch (e.type) {
				case SDL_QUIT:
					running = false;
					break;	
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym) {
						case SDLK_UP:
							currentTexture = directionTextures[DIR_UP];
							break;
						case SDLK_DOWN:
							currentTexture = directionTextures[DIR_DOWN];
							break;
						case SDLK_LEFT:
							currentTexture = directionTextures[DIR_LEFT];
							break;
						case SDLK_RIGHT:
							currentTexture = directionTextures[DIR_RIGHT];
							break;
						case SDLK_ESCAPE:
							running = false;
							break;
						default:
							currentTexture = directionTextures[DIR_NONE];
							break;
						
					}
					break;
			}
		}
		
		SDL_RenderClear(renderer);
		
		SDL_RenderCopy(renderer, currentTexture, NULL, NULL);
		
		SDL_RenderPresent(renderer);
		
	}
	
	

	close();
    return 0;
}
	
bool init(SDL_Window** window)
{
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		return false;
    }
    
    //Create window
    *window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( *window == NULL )
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		return false;
    }
	
	renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		printf("Could not create Renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(*window);
		return false;
	}
	
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  
	return true;
}

void loadImage(SDL_Texture* dest_array[], const unsigned index, const char* path) {	
	SDL_Texture* texture = IMG_LoadTexture(renderer, path);
	if (texture == NULL) {
		output_error(true, "Could not load texture: %s, %s\n", path, IMG_GetError());
	}
	dest_array[index] = texture;
	list_add(&textures, texture);
}

void loadData(SDL_Texture* dir_textures[]) 
{
	loadImage(dir_textures, DIR_UP, "assets/up.png");
	loadImage(dir_textures, DIR_DOWN, "assets/down.png");
	loadImage(dir_textures, DIR_LEFT, "assets/left.png");
	loadImage(dir_textures, DIR_RIGHT, "assets/right.png");
	loadImage(dir_textures, DIR_NONE, "assets/center.png");
}

void close() 
{
	for (unsigned i = 0; i < textures.size; i++) {
		SDL_Texture* texture = list_get(&textures, i);
		SDL_DestroyTexture(texture);
	}
	
	list_free(&textures);
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow( window );
	IMG_Quit();
    //Quit SDL subsystems
    SDL_Quit();
}