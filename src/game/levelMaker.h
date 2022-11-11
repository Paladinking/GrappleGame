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
		LevelMaker(LevelData&& data, const LevelConfig& level_config) : State(), level_data(std::move(data)), level_config(level_config) {}

		virtual void handle_down(const SDL_Keycode key, const Uint8 mouse) override;

		virtual void init(WindowState* window_state) override;
	
		virtual void handle_wheel(const SDL_MouseWheelEvent &e) override;

		virtual void render() override;
		
		virtual void tick(const Uint64 delta, StateStatus& res) override;
	private:
	
		void zoom(const bool in);

		void tile_press(bool put);
		
		std::unique_ptr<SDL_Surface, SurfaceDeleter> tiles;
		std::unique_ptr<SDL_Surface, SurfaceDeleter> objects;
		std::unique_ptr<SDL_Surface, SurfaceDeleter> marker;

		// window_surface is owned by the gWindow instance, and will be freed when gWindow is freed.
		SDL_Surface* window_surface = nullptr;
		
		std::unique_ptr<PressInput> 
			zoom_in_input, zoom_out_input, 
			put_tile_input, clear_tile_input,
			left_input, right_input, up_input, down_input,
			save_input, tiles_input, colisions_input, tile_colisions_input,
			tile_scale_up_input, tile_scale_down_input;
		std::unique_ptr<HoldInput> camera_pan_input, exit_input;

		int selected = 0;

		enum {
			PLACE_TILES, PLACE_COLLISIONS
		} editor_mode = PLACE_TILES;

		LevelData level_data;
		LevelConfig level_config;
		bool tile_colisions = true;
		bool updated = true;

		int scale_factor = 0;
		double camera_x = 0.0, camera_y = 0.0;

		SDL_Rect editor_viewport, tiles_viewport, objects_viewport;

		// Number of tiles of one side of the texture (1-8). Uint32 so that tile_scale << 16 fits.
		Uint32 tile_scale = 1; 
};

#endif
