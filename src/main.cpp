#include <stdio.h>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include "globals.h"
#include "texture.h"
#include "exceptions.h"


bool init(SDL_Window**);

void loadData(std::vector<Texture>&);

void close();

//The window we'll be rendering to
SDL_Window* gWindow;
SDL_Renderer* gRenderer;

std::vector<Texture> textures;

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

	//The image we will load and show on the screen
	
	if (!init(&gWindow)) {
		return 1;
	}
	
	loadData(textures);

	bool running = true;
	
	int currentTexture = DIR_NONE;
	
	Texture ballTexture;
	
	try {
		ballTexture.load_from_file("assets/ball.png");
	} catch(image_load_exception &e) {
		close();
		exit(-1);
	}
	
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
							currentTexture = DIR_UP;
							break;
						case SDLK_DOWN:
							currentTexture = DIR_DOWN;
							break;
						case SDLK_LEFT:
							currentTexture = DIR_LEFT;
							break;
						case SDLK_RIGHT:
							currentTexture = DIR_RIGHT;
							break;
						case SDLK_ESCAPE:
							running = false;
							break;
						default:
							currentTexture = DIR_NONE;
							break;
						
					}
					break;
			}
		}
		
		SDL_RenderClear(gRenderer);
		
		textures[currentTexture].render(0, 0);
		
		ballTexture.render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 );
		
		SDL_RenderPresent(gRenderer);
		
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
	
	gRenderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == NULL) {
		printf("Could not create Renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(*window);
		return false;
	}
	
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
  
	return true;
}
/*
void loadImage(SDL_Texture* dest_array[], const unsigned index, const char* path) {	
	SDL_Texture* texture = IMG_LoadTexture(renderer, path);
	if (texture == NULL) {
		output_error(true, "Could not load texture: %s, %s\n", path, IMG_GetError());
	}
	dest_array[index] = texture;
	list_add(&textures, texture);
}*/

void loadData(std::vector<Texture> &textures)   
{
	textures.push_back(Texture());
	textures.push_back(Texture());
	textures.push_back(Texture());
	textures.push_back(Texture());
	textures.push_back(Texture());
	
	try {
		textures[DIR_UP].load_from_file("assets/up.png");
		textures[DIR_DOWN].load_from_file("assets/down.png");
		textures[DIR_LEFT].load_from_file("assets/left.png");
		textures[DIR_RIGHT].load_from_file("assets/right.png");
		textures[DIR_NONE].load_from_file("assets/center.png");
	} catch (image_load_exception &e) {
		printf("Failed to load assets: ");
		printf("%s\n", e.msg.c_str());
		close();
		exit(-1);
	}
}


void close() 
{
	textures.clear();
	
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	
	IMG_Quit();
    //Quit SDL subsystems
    SDL_Quit();
}