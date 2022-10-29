#ifndef LEVEL_MAKER_00_H
#define LEVEL_MAKER_00_H
#include <memory>
#include <utility>
#include "engine/texture.h"
#include "engine/game.h"
#include "globals.h"
#include "engine/input.h"
#include "util/utilities.h"
#include "level.h"


class LevelMaker : public State {
	public:
		LevelMaker(LevelData&& data, const std::string& tiles_path) : State(SCREEN_WIDTH * 2, SCREEN_HEIGHT, "Level maker"), level_data(std::move(data)), tiles_path(tiles_path) {}

		virtual void handle_down(const SDL_Keycode key, const Uint8 mouse) override;

		virtual void init() override;

		virtual void render() override;

	private:

		void tile_press(bool put);
		
		std::string tiles_path;
		
		std::unique_ptr<SDL_Surface, SurfaceDeleter> tiles;
		std::unique_ptr<SDL_Surface, SurfaceDeleter> marker;

		// window_surface is owned by the gWindow instance, and will be freed when gWindow is freed.
		SDL_Surface* window_surface = nullptr;
		
		std::unique_ptr<PressInput> 
			zoom_in_input, zoom_out_input, 
			put_tile_input, clear_tile_input,
			left_input, right_input, up_input, down_input,
			save_input, tiles_input, colisions_input, tile_colisions_input,
			tile_scale_up_input, tile_scale_down_input;

		Uint16 selected = 0;
		
		enum {
			PLACE_TILES, PLACE_COLLISIONS
		} editor_mode = PLACE_TILES;

		LevelData level_data;
		bool tile_colisions = true;

		int x_start = 0, x_end = 80;
		int y_start = 0, y_end = 80;
		int zoom_level = 0;

		// Number of tiles of one side of the texture (1-8). Uint32 so that tile_scale << 16 fits.
		Uint32 tile_scale = 1; 

		int editor_width = 640;
};

#endif
