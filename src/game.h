#ifndef GAME_00_H
#define GAME_00_H
#include <SDL.h>
#include "Texture.h"

/**
 * Bases Game class, to be extended by a more specific game. On it's own is only a white window with a title that can be closed.
 */
class Game {
	public:
		Game(unsigned window_width, unsigned window_height, std::string title) 
			: window_width(window_width), window_height(window_height), title(title) {}
		
		~Game();
		
		/**
		 * Creates a new game, opening a window and creating a renderer. Throws SDL_exception if creating window or renderer fails.
		 * Throws a Game_exception if a game is already running.
		 */
		void create();
		
		/**
		 * Starts and runs the game loop, calling tick and render every frame and handle_keydown / handle_keyup when a keypress happens.
		 * Will return after exit_game has been called, or immedietly if the game has not been created or has been destroyed.
		 * Closing the window will call exit_game and cause run to return.
		 */
		void run();
		
		/**
		 * Exits an ongoing game.
		 */
		void exit_game();
		
		/**
		 * Frees all resources used by the game.
		 */
		void destroy_game();
		
	protected:
		// Window dimensions
		const unsigned window_width = 100, window_height = 100;
		
		// Mouse position, updated every frame.
		int mouseX, mouseY;
		
		Uint8 mouseButton;
		
		/**
		 * Called once per frame, after tick has been called. 
		 * In the future the renderer might be given as a parameter here instead of being global.
		 */
		virtual void render() {};
		
		/**
		 * Initializes a game, called at the end of create. If init trows an exception the game will not be successfully created.
		 */
		virtual void init() {};
		
		/**
		 * Tick function, called once every frame, before render. Delta is the passed time in milliseconds.
		 */
		virtual void tick(Uint64 delta) {};
		
		/**
		 * Called every time a KEYDOWN-event happens.
		 */
		virtual void handle_keydown(SDL_KeyboardEvent e) {};
		
		/**
		 * Called every time a KEYUP-event happens.
		 */
		virtual void handle_keyup(SDL_KeyboardEvent e) {};
		
		/**
		 * Called every time a MOUSEDOWN-event happens.
		 */
		virtual void handle_mousedown(SDL_MouseButtonEvent e) {};
		
		/**
		 * Called every time a MOUSEUP-event happens.
		 */
		virtual void handle_mouseup(SDL_MouseButtonEvent e) {};
	
		
	private:
		bool running = false;
		bool destroyed = true;
		
		const std::string title = "Title";
};



#endif