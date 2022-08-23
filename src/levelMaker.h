#ifndef LEVEL_MAKER_00_H
#define LEVEL_MAKER_00_H
#include "texture.h"
#include "game.h"
#include "globals.h"
#include "input.h"


class LevelMaker : public Game {
	public:
		LevelMaker() : Game(640 + 640, 640, "Level maker") {}
		
		virtual void destroy_game();
	
	protected:
		virtual void handle_mousedown(SDL_MouseButtonEvent e) override;
		
		virtual void handle_mouseup(SDL_MouseButtonEvent e) override;
		
		virtual void handle_keydown(SDL_KeyboardEvent e) override;
		
		virtual void init() override;
		
		virtual void render() override;
		
		
	private:
		SDL_Surface* tiles;
		
		SDL_Surface* marker;
		
		SDL_Surface* window_surface;
		
		std::unique_ptr<PressInput> click;
		
		int selected = 0;
	
};

#endif