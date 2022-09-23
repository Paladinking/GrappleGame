#ifndef LEVEL_MAKER_00_H
#define LEVEL_MAKER_00_H
#include <memory>
#include "engine/texture.h"
#include "engine/game.h"
#include "globals.h"
#include "engine/input.h"
#include "util/utilities.h"


class LevelMaker : public State {
	public:
		LevelMaker() : State(SCREEN_WIDTH * 2, SCREEN_HEIGHT, "Level maker") {}

		virtual void handle_down(const SDL_Keycode key, const Uint8 mouse) override;

		virtual void init() override;

		virtual void render() override;

	private:

		void tile_press(bool put);
		
		std::unique_ptr<SDL_Surface, SurfaceDeleter> tiles;
		
		std::unique_ptr<SDL_Surface, SurfaceDeleter> marker;

		// window_surface is owned by the gWindow instance, and will be freed when gWindow is freed.
		SDL_Surface* window_surface = nullptr;
		
		std::unique_ptr<PressInput> zoom_in_input, zoom_out_input, put_tile_input, clear_tile_input,
			left_input, right_input, up_input, down_input;

		int selected = 0;

		int width = 80, height = 160;

		int x_start = 0, x_end = 80;
		int y_start = 0, y_end = 80;
		int zoom_level = 0;

		int tile_size = 8;

		int editor_width = 640;

		std::unique_ptr<Uint16[]> data;
};

#endif
