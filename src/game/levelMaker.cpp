#include "levelMaker.h"
#include "file/Json.h"
#include "util/exceptions.h"
#include "config.h"
#include "globals.h"
#include "nativefiledialog/nfdcpp.h"
#include <algorithm>
#include <cmath>

// Tile apperance (0xUUSSIITT)
// UU = unused
// SS = tilescale
// II = image id (0xFF means no image)
// TT = tile type (empty 0, blocked 1, spike 2)
// eg empty tile apperance (0x0000FF00)

constexpr int TILE_SELECTOR_SIZE = 64;
constexpr int TILE_SELECTOR_TW = 10;

// How big a tile is at scale_factor 1.0
constexpr double DEFAULT_TS = 16.0;
// To avoid lines appearing in the tiles, some zoom levels that works at ca 1.25 multiples apart from each other.
const double SCALE_FACTORS[] = {0.625, 1.0, 1.25, 1.5, 2.0, 3.0, 3.75, 4.75, 6.0, 7.5, 9.5, 12.0, 15.0, 18.75, 23.5, 30.0};
constexpr int SCALE_FACTORS_LEN = 16;
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
	camera_pan_input = get_hold_input(controls.get<std::string>("camera_pan"), "None");
	exit_input = get_hold_input(config::get_bindings(bindings::GENERAL.key).get<std::string>("exit_menu"), "None");

	tiles_viewport = {
		window_state->screen_width - TILE_SELECTOR_SIZE * TILE_SELECTOR_TW,
		0,
		TILE_SELECTOR_SIZE * TILE_SELECTOR_TW,
		TILE_SELECTOR_SIZE * (static_cast<int>(level_config.img_tilecount) / TILE_SELECTOR_TW)
	};

	const int editor_w = window_state->screen_width - tiles_viewport.w;
	const int editor_h = window_state->screen_height;

	editor_viewport = {
		(window_state->screen_width - tiles_viewport.w) / 2 - editor_w / 2,
		window_state->screen_height / 2 - editor_h / 2,
		editor_w,
		editor_h
	};

	while (
		SCALE_FACTORS[min_scale_factor] * DEFAULT_TS * level_data.width < editor_w || 
		SCALE_FACTORS[min_scale_factor] * DEFAULT_TS * level_data.height < editor_h
	) {
		++min_scale_factor;
	}
	if (scale_factor < min_scale_factor) scale_factor = min_scale_factor;
	
	std::cout << min_scale_factor << std::endl;
	
	objects_viewport = {
		tiles_viewport.x,
		tiles_viewport.h,
		tiles_viewport.w,
		window_state->screen_height - tiles_viewport.h
	};
	
	{
		SDL_Surface* t = IMG_Load(level_config.objects_path.c_str());
		if (t == NULL) {
			throw image_load_exception(std::string(IMG_GetError()));
		}
		objects.reset(t);
	}


	SDL_Surface* t = IMG_Load(level_config.tiles_path.c_str());
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

SDL_Rect get_tile_rect(Uint16 index, const LevelConfig& level_config) {
	return {
		static_cast<int>((index % level_config.img_tilewidth) * level_config.img_tilesize),
		static_cast<int>((index / level_config.img_tilewidth) * level_config.img_tilesize),
		static_cast<int>(level_config.img_tilesize),
		static_cast<int>(level_config.img_tilesize)
	};
}


void LevelMaker::tile_press(const bool put) {
	updated = true;
	const int mouseX = window_state->mouseX;
	const int mouseY = window_state->mouseY;
	if (
		mouseX >= tiles_viewport.x && mouseX < tiles_viewport.x + tiles_viewport.w && 
		mouseY >= tiles_viewport.y && mouseY < tiles_viewport.y + tiles_viewport.h
	) {
		int index = (mouseX - tiles_viewport.x) / TILE_SELECTOR_SIZE 
			+ ((mouseY - tiles_viewport.y) / TILE_SELECTOR_SIZE) * TILE_SELECTOR_TW;
		if (index >= 0 && index < static_cast<int>(level_config.img_tilecount)) {
			selected = static_cast<Uint16>(index);
		}
	} else if (
		mouseX >= editor_viewport.x && mouseX < editor_viewport.x + editor_viewport.w &&
		mouseY >= editor_viewport.y && mouseY < editor_viewport.y + editor_viewport.h
	) {
		const double ts = DEFAULT_TS * SCALE_FACTORS[scale_factor];
		const int x_tile = static_cast<int>((mouseX + camera_x - editor_viewport.x) / ts);
		const int y_tile = static_cast<int>((mouseY + camera_y - editor_viewport.y) / ts);

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

void LevelMaker::zoom(const bool in) {
	int new_scale_factor = in ? scale_factor + 1  : scale_factor - 1;
	if (new_scale_factor < min_scale_factor) new_scale_factor = min_scale_factor;
	else if (new_scale_factor > SCALE_FACTORS_LEN - 1) new_scale_factor = SCALE_FACTORS_LEN - 1;

	const double scale_div = SCALE_FACTORS[new_scale_factor] / SCALE_FACTORS[scale_factor];
    camera_x = ((scale_div) * (camera_x) - (1 - scale_div) * window_state->mouseX);
    camera_y = ((scale_div) * (camera_y) - (1 - scale_div) * window_state->mouseY);

	scale_factor = new_scale_factor;
	updated = true;
}

void LevelMaker::handle_wheel(const SDL_MouseWheelEvent &e) {
	zoom(e.preciseY > 0);
};
		
void LevelMaker::handle_down(const SDL_Keycode key, const Uint8 mouse) {
	if (put_tile_input->is_targeted(key, mouse)) {
		tile_press(true);
	} 
	if (clear_tile_input->is_targeted(key, mouse)) {
		tile_press(false);
	}
	if (zoom_in_input->is_targeted(key, mouse)) {
		zoom(true);
	} else if (zoom_out_input->is_targeted(key, mouse)) {
		zoom(false);
	} 
	if (left_input->is_targeted(key, mouse)) {
		camera_x -= DEFAULT_TS * SCALE_FACTORS[scale_factor] / 3.0;
		updated = true;
	} else if (right_input->is_targeted(key, mouse)) {
		camera_x += DEFAULT_TS * SCALE_FACTORS[scale_factor] / 3.0;
		updated = true;
	}
	if (up_input->is_targeted(key, mouse)) {
		camera_y -= DEFAULT_TS * SCALE_FACTORS[scale_factor] / 3.0;
		updated = true;
	} else if (down_input->is_targeted(key, mouse)) {
		camera_y += DEFAULT_TS * SCALE_FACTORS[scale_factor] / 3.0;
		updated = true;
	}
	if (save_input->is_targeted(key, mouse)) {
		std::string path;
		if (nfd::SaveDialog(path) == NFD_OKAY) {
			level_data.write_to_file(path);
		}
	}
	if (tiles_input->is_targeted(key, mouse)) {
		editor_mode = PLACE_TILES;
		updated = true;
	} else if (colisions_input->is_targeted(key, mouse)) {
		editor_mode = PLACE_COLLISIONS;
		updated = true;
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

void LevelMaker::tick(const Uint64 delta, StateStatus& res) {
	if (exit_input->is_pressed(window_state->keyboard_state, window_state->mouse_mask)) {
		res.action = StateStatus::POP;
	} else if (camera_pan_input->is_pressed(window_state->keyboard_state, window_state->mouse_mask)) {
		int mouse_dx = 0, mouse_dy = 0;
		SDL_GetRelativeMouseState(&mouse_dx, &mouse_dy);
		camera_x -= mouse_dx;
		camera_y -= mouse_dy;
		updated = true;
	} else {
		SDL_GetRelativeMouseState(NULL, NULL);
	}
}

void LevelMaker::render() {
	if (!updated) return;

	const double ts = DEFAULT_TS * SCALE_FACTORS[scale_factor];
	
	if (camera_x < 0) 
		camera_x = 0;
	
	else if (camera_x > level_data.width * ts - editor_viewport.w) 
		camera_x = level_data.width * ts - editor_viewport.w;
	
	if (camera_y < 0) 
		camera_y = 0;
	else if (camera_y > level_data.height * ts -  editor_viewport.h) 
		camera_y = level_data.height * ts - editor_viewport.h;
	
	SDL_Rect r = {0, 0, window_state->screen_width, window_state->screen_height};
	SDL_FillRect(window_surface, &r, SDL_MapRGB(window_surface->format, 235, 235, 235));
	SDL_FillRect(window_surface, &editor_viewport, SDL_MapRGB(window_surface->format, 255, 255, 255));

	const int first_tile_x = std::max(static_cast<int>(camera_x / ts) - MAX_TILE_SCALE, 0);
	const int last_tile_x = std::min(
		static_cast<int>((camera_x + editor_viewport.w) / ts + 1), static_cast<int>(level_data.width));

	const int first_tile_y = std::max(static_cast<int>(camera_y / ts) - MAX_TILE_SCALE, 0);
	const int last_tile_y = std::min(
		static_cast<int>((camera_y + editor_viewport.h) / ts + 1), static_cast<int>(level_data.height));

	SDL_SetClipRect(window_surface, &editor_viewport);
	
	for (int x = first_tile_x; x < last_tile_x; x++) {
		for (int y = first_tile_y; y < last_tile_y; y++) {
			if (editor_mode == PLACE_TILES) {
				Uint16 tile_index = (level_data.data[x + level_data.width * y] >> 8) & 0xFF;
				Uint16 scale = (level_data.data[x + level_data.width * y] >> 16) & 0xFF;
				if (tile_index != 0xFF) {
					SDL_Rect source = get_tile_rect(tile_index, level_config);
					SDL_Rect dest = {
						static_cast<int>(editor_viewport.x + x * ts - camera_x),
						static_cast<int>(editor_viewport.y + y * ts - camera_y), 
						static_cast<int>(ts * scale), static_cast<int>(ts * scale)
					};
					SDL_BlitScaled(tiles.get(), &source, window_surface, &dest);
				}
			} else {
				SDL_Rect dest = {
					static_cast<int>(editor_viewport.x + x * ts - camera_x),
					static_cast<int>(editor_viewport.y + y * ts - camera_y), 
					static_cast<int>(ts), static_cast<int>(ts)
				};
				if ((level_data.data[x + level_data.width * y] & 0xFF) != 0) {
					SDL_FillRect(window_surface, &dest, SDL_MapRGB(window_surface->format, 0xFF, 0, 0));
				}
			}
		}
	}

	SDL_SetClipRect(window_surface, NULL);
	SDL_FillRect(window_surface, &tiles_viewport, SDL_MapRGB(window_surface->format, 0xFF, 0xAA, 0));
	for (Uint16 i = 0; i < level_config.img_tilecount; ++i) {
		SDL_Rect tile_rect = get_tile_rect(i, level_config);
		SDL_Rect dest = {
			tiles_viewport.x + (i % TILE_SELECTOR_TW) * TILE_SELECTOR_SIZE,
			(i / TILE_SELECTOR_TW) * TILE_SELECTOR_SIZE, 
			TILE_SELECTOR_SIZE, TILE_SELECTOR_SIZE
		};
		SDL_BlitScaled(tiles.get(), &tile_rect, window_surface, &dest);
		if (i == selected) {
			SDL_BlitScaled(marker.get(), NULL, window_surface, &dest);
		}
	}
	
	SDL_Rect dest = {
		objects_viewport.x, objects_viewport.y, TILE_SELECTOR_SIZE, TILE_SELECTOR_SIZE
	};
	SDL_BlitScaled(objects.get(), NULL, window_surface, &dest);
	updated = false;
	
	SDL_UpdateWindowSurface(gWindow);
}
