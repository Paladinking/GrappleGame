#ifndef GAME_00_H
#define GAME_00_H
#include <SDL.h>
#include "Texture.h"


class Game {
	public:
		Game(unsigned window_width, unsigned window_height, std::string title) 
			: window_width(window_width), window_height(window_height), title(title) {}
		
		~Game();
		
		void create();
		
		void run();
		
		virtual void render() {};
		
		virtual void tick(Uint64 delta) {};
		
		virtual void handle_input() {};
		
		void exit_game();
	
		
	private:
		bool running = false;
		bool exited = false;
		
		const unsigned window_width = 100, window_height = 100;
		
		const std::string title;
		
		SDL_Window* window;
		
		SDL_Renderer* renderer;
	
	
};



#endif