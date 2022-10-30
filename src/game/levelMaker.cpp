#include "levelMaker.h"
#include "file/Json.h"
#include "util/exceptions.h"
#include "config.h"
#include "globals.h"
#include "nativefiledialog/nfdcpp.h"
#include <algorithm>

// Tile apperance (0xUUSSIITT)
// UU = unused
// SS = tilescale
// II = image id
// TT = tile type (empty 0 or solid 1)
// eg empty tile apperance (0x0000FF00)

constexpr int TILE_SELECTOR_SIZE = 64;
constexpr int TILE_SELECTOR_TW = 10;
constexpr int TILE_SELECTOR_TH = 10;

const int ZOOM_LEVELS[] = {8, 10, 16, 20, 32, 40, 64, 80, 160};
constexpr int ZOOM_LEVELS_SIZE = 9;
constexpr int MAX_TILE_SCALE = 8;

void LevelMaker::init(WindowState* ws) {
	State::init(ws);
	SDL_SetWindowTitle(gWindow, "LevelMaker");

	const JsonObject& controls = config::get_bindings(bindings::LEVELMAKER.key);
	zoom_in_input = get_press_input(controls.get<std::string>("zoom_in"), "None");
	zoom_out_input = get_press_input(controls.get<std::string>("zoom_out"), "None");
	put_tile_input = get_press_input(controls.get<std::string>("place_tile"), "None");
	clear_tile_input = get_press_input(controls.get<std::string>("clear_tile"), "None");

	left_input = get_press_input(controls.get<std::string>("navigate_left"), "None");
	right_input = get_press_input(controls.get<std::string>("navigate_right"), "None");
	up_input = get_press_input(controls.get<std::string>("navigate_up"), "None");
	down_input = get_press_input(controls.get<std::string>("navigate_down"), "None");
	
	save_input = get_press_input(controls.get<std::string>("save_level"), "None");
	tiles_input = get_press_input(controls.get<std::string>("tiles_mode"), "None");
	colisions_input = get_press_input(controls.get<std::string>("collision_mode"), "None");
	tile_colisions_input = get_press_input(controls.get<std::string>("tile_collisions"), "None");
	tile_scale_up_input = get_press_input(controls.get<std::string>("tile_scale_up"), "None");
	tile_scale_down_input = get_press_input(controls.get<std::string>("tile_scale_down"), "None");
	editor_width = SCREEN_WIDTH * 2;

	SDL_Surface* t = IMG_Load(tiles_path.c_str());
	if (t == NULL) {
		throw image_load_exception(std::string(IMG_GetError()));
	}
	tiles.reset(t);

	SDL_Surface* m = IMG_Load(config::get_asset_path("marker.png").c_str());
	if (m == NULL) {
		throw image_load_exception(std::string(IMG_GetError()));
	}
	marker.reset(m);
	
	window_surface = SDL_GetWindowSurface(gWindow);
}

SDL_Rect get_tile_rect(Uint16 index, const LevelData& level_data) {
	return {
		static_cast<int>((index % level_data.img_tilewidth) * level_data.img_tilesize),
		static_cast<int>((index / level_data.img_tilewidth) * level_data.img_tilesize),
		static_cast<int>(level_data.img_tilesize),
		static_cast<int>(level_data.img_tilesize)
	};
}

void LevelMaker::tile_press(const bool put) {
	const int mouseX = window_state->mouseX;
	const int mouseY = window_state->mouseY;
	if (mouseX >= editor_width) {
		int index = (mouseX - editor_width) / TILE_SELECTOR_SIZE + (mouseY / TILE_SELECTOR_SIZE) * TILE_SELECTOR_TW;
		if (index >= 0 && index < static_cast<int>(level_data.img_tilecount)) {
			selected = static_cast<Uint16>(index);
		}
	} else {
		const int zoom_tile_size = editor_width / (x_end - x_start);
		const int x_tile = x_start + (mouseX / zoom_tile_size);
		const int y_tile = y_start + (mouseY / zoom_tile_size);
		const int index = x_tile + y_tile * level_data.width;

		if (editor_mode == PLACE_TILES) {
			Uint32 new_tile = put ? (tile_scale << 16) | (selected << 8) | 1 : (0xFF << 8);
			if (!tile_colisions) {
				new_tile = (new_tile & 0xFFFFFF00) | level_data.data[index] & 0xFF;
			}
			const Uint32 val = put ? (0xFF << 8) | 1 : (0xFF << 8);
			for (int y  = y_tile; y < std::min(y_tile + static_cast<int>(tile_scale), static_cast<int>(level_data.height)); ++y) {
				for (int x = x_tile; x < std::min(x_tile + static_cast<int>(tile_scale), static_cast<int>(level_data.width)); ++x) {
					int i = x + level_data.width * y;
					level_data.data[i] = tile_colisions ? val : (val & 0xFFFFFF00) | level_data.data[i] & 0xFF;
				}
			}
			level_data.data[index] = new_tile;
		} else {
			level_data.data[index] = (level_data.data[index] & 0xFFFFFF00) | (put ? 1 : 0);
		}
	}
}
		
void LevelMaker::handle_down(const SDL_Keycode key, const Uint8 mouse) {
	if (put_tile_input->is_targeted(key, mouse)) {
		tile_press(true);
	} 
	if (clear_tile_input->is_targeted(key, mouse)) {
		tile_press(false);
	}
	if (zoom_in_input->is_targeted(key, mouse) && zoom_level < ZOOM_LEVELS_SIZE - 1) {
		const int prev_ts = ZOOM_LEVELS[zoom_level];
		zoom_level++;
		const int ts = ZOOM_LEVELS[zoom_level];
		const int visible_tiles_delta = editor_width / prev_ts - editor_width / ts;
		x_start += visible_tiles_delta / 2;
		x_end -= visible_tiles_delta / 2;
		y_start += visible_tiles_delta / 2;
		y_end -= visible_tiles_delta / 2;
	} else if (zoom_out_input->is_targeted(key, mouse) && zoom_level > 0) {
		const int prev_ts = ZOOM_LEVELS[zoom_level];
		zoom_level--;
		const int ts = ZOOM_LEVELS[zoom_level];
		const int visible_tiles_delta = editor_width / ts - editor_width / prev_ts;
		int delta_x_start = std::min(visible_tiles_delta / 2, x_start)
				- std::min(0, static_cast<int>(level_data.width) - x_end - visible_tiles_delta / 2);
		int delta_y_start = std::min(visible_tiles_delta / 2, y_start) 
				- std::min(0, static_cast<int>(level_data.height) - y_end - visible_tiles_delta / 2);
		x_start -= delta_x_start;
		x_end += visible_tiles_delta - delta_x_start;
		y_start -= delta_y_start;
		y_end += visible_tiles_delta - delta_y_start; 
	} 
	if (left_input->is_targeted(key, mouse) && x_start > 0) {
		x_start--;
		x_end--;
	} else if (right_input->is_targeted(key, mouse) && x_end < static_cast<int>(level_data.width)) {
		x_start++;
		x_end++;
	}
	if (up_input->is_targeted(key, mouse) && y_start > 0) {
		y_start--;
		y_end--;
	} else if (down_input->is_targeted(key, mouse) && y_end < static_cast<int>(level_data.height)) {
		y_start++;
		y_end++;
	}
	if (save_input->is_targeted(key, mouse)) {
		std::string path;
		if (nfd::SaveDialog(path) == NFD_OKAY) {
			level_data.write_to_file(path);
		}
	}
	if (tiles_input->is_targeted(key, mouse)) {
		editor_mode = PLACE_TILES;
	} else if (colisions_input->is_targeted(key, mouse)) {
		editor_mode = PLACE_COLLISIONS;
	}
	if (tile_colisions_input->is_targeted(key, mouse)) {
		tile_colisions = !tile_colisions;
	}
	if (tile_scale_up_input->is_targeted(key, mouse) && tile_scale < MAX_TILE_SCALE) {
		tile_scale++;
		
	} else if (tile_scale_down_input->is_targeted(key, mouse) && tile_scale > 1) {
		tile_scale--;
	}
}

void LevelMaker::render() {
	SDL_Rect r = {0, 0, static_cast<int>(window_state->screen_width), static_cast<int>(window_state->screen_height)};
	SDL_FillRect(window_surface, &r, SDL_MapRGB(window_surface->format, 255, 255, 255));
	
	const int ts = ZOOM_LEVELS[zoom_level];
	for (int x = x_start; x < x_end; ++x) {
		for (int y = y_start; y < y_end; ++y) {
			
			if (editor_mode == PLACE_TILES) {
				Uint16 tile_index = (level_data.data[x + level_data.width * y] >> 8) & 0xFF;
				Uint16 scale = (level_data.data[x + level_data.width * y] >> 16) & 0xFF;
				if (tile_index != 0xFF) {
					SDL_Rect source = get_tile_rect(tile_index, level_data);
					SDL_Rect dest = {(x - x_start) * ts, (y - y_start) * ts, ts * scale, ts * scale};
					SDL_BlitScaled(tiles.get(), &source, window_surface, &dest);
				}
			} else {
				SDL_Rect dest = {(x - x_start) * ts, (y - y_start) * ts, ts, ts};
				if ((level_data.data[x + level_data.width * y] & 0xFF) != 0) {
					SDL_FillRect(window_surface, &dest, SDL_MapRGB(window_surface->format, 0xFF, 0, 0));
				}
			}
		}
	}


	for (Uint16 i = 0; i < level_data.img_tilecount; ++i) {
		SDL_Rect tile_rect = get_tile_rect(i, level_data);
		SDL_Rect dest = {
			editor_width + (i % TILE_SELECTOR_TW) * TILE_SELECTOR_SIZE,
			(i / TILE_SELECTOR_TW) * TILE_SELECTOR_SIZE, 
			TILE_SELECTOR_SIZE, TILE_SELECTOR_SIZE
		};
		SDL_BlitScaled(tiles.get(), &tile_rect, window_surface, &dest);
		if (i == selected) {
			SDL_BlitScaled(marker.get(), NULL, window_surface, &dest);
		}
	}
	
	
	
	SDL_UpdateWindowSurface(gWindow);
}
