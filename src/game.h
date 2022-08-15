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
		
		void exit_game();
		
		void destroy_game();
		
	protected:
	
		const unsigned window_width = 100, window_height = 100;
		
		virtual void render() {};
		
		virtual void init() {};
		
		virtual void tick(Uint64 delta) {};
		
		virtual void handle_keydown(SDL_KeyboardEvent e) {};
		
		virtual void handle_keyup(SDL_KeyboardEvent e) {};
		
		
	
		
	private:
		bool running = false;
		bool destroyed = true;
		
		const std::string title = "Title";
};



#endif