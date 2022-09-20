#ifndef LEVEL_MAKER_00_H
#define LEVEL_MAKER_00_H
#include <memory>
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
		
		virtual void handle_keydown(SDL_KeyboardEvent e) override;
		
		virtual void init() override;
		
		virtual void render() override;
		
		
	private:
		void handle_down(const SDL_Keycode key, const Uint8 mouse);
		
		void tile_press(bool put);
	
		SDL_Surface* tiles = nullptr;
		
		SDL_Surface* marker = nullptr;
		
		SDL_Surface* editor_window = nullptr;
		
		SDL_Surface* window_surface = nullptr;
		
		std::unique_ptr<PressInput> zoom_in_input, zoom_out_input, put_tile_input, clear_tile_input,
			left_input, right_input, up_input, down_input;
		
		int selected = 0;
		
		int width = 80, height = 80;
		
		int x_start = 0, x_end = 80;
		int y_start = 0, y_end = 80;
		int zoom_level = 0;
		
		int tile_size = 8;
		
		std::unique_ptr<Uint16[]> data = std::make_unique<Uint16[]>(80 * 80);
	
};

#endif