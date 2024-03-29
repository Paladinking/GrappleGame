#include "levelMaker.h"
#include "util/exceptions.h"
#include "config.h"
#include "nativefiledialog/nfdcpp.h"
#include <algorithm>

// Tile appearance (0xUUSSIITT)
// UU = unused
// SS = tile scale
// II = image id (0xFF means no image)
// TT = tile type (empty 0, blocked 1, spike 2)
// e.g. empty tile appearance (0x0000FF00)

constexpr int TILE_SELECTOR_SIZE = 64;
constexpr int TILE_SELECTOR_TW = 10;

// How big a tile is at scale_factor 1.0
constexpr double DEFAULT_TS = 16.0;
// To avoid lines appearing in the tiles, some zoom levels that works at ca 1.25 multiples apart from each other.
const double SCALE_FACTORS[] = {0.625, 1.0, 1.25, 1.5, 2.0, 3.0, 3.75, 4.75, 6.0, 7.5, 9.5, 12.0, 15.0, 18.75, 23.5, 30.0};
constexpr int SCALE_FACTORS_LEN = 16;
constexpr int MAX_TILE_SCALE = 8;
constexpr int TOTAL_OBJECTS = 1;

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
    collisions_input = get_press_input(controls.get<std::string>("collision_mode"), "None");
    tile_collisions_input = get_press_input(controls.get<std::string>("tile_collisions"), "None");
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
	
	objects_viewport = {
		tiles_viewport.x,
		tiles_viewport.h,
		tiles_viewport.w,
		window_state->screen_height - tiles_viewport.h
	};
	
	{
		SDL_Surface* t = IMG_Load(level_config.objects_path.c_str());
		if (t == nullptr) {
			throw image_load_exception(std::string(IMG_GetError()));
		}
		objects.reset(t);
	}


	SDL_Surface* t = IMG_Load(level_config.tiles_path.c_str());
	if (t == nullptr) {
		throw image_load_exception(std::string(IMG_GetError()));
	}
	tiles.reset(t);

	SDL_Surface* m = IMG_Load(config::get_asset_path("marker.png").c_str());
	if (m == nullptr) {
		throw image_load_exception(std::string(IMG_GetError()));
	}
	marker.reset(m);
	
	window_surface = SDL_GetWindowSurface(gWindow);
}

bool is_pressed(const SDL_Rect& viewport, const int x, const int y) {
	return x >= viewport.x && y >= viewport.y && x < viewport.x + viewport.w && y < viewport.y + viewport.h;
}

void LevelMaker::set_tile(const int index, const Uint32 scale, const Uint32 image_id, const Uint32 tile_type) const {
	level_data.data[index] = ((scale & 0xFF) << 16) | ((image_id & 0xFF) << 8) | (tile_type & 0xFF);
}

void LevelMaker::set_tile(const int index, const Uint32 scale, const Uint32 image_id) const {
	level_data.data[index] = ((scale & 0xFF) << 16) | ((image_id & 0xFF) << 8) | (level_data.data[index] & 0xFF);
}

void LevelMaker::place_spike(const int x_tile, const int y_tile) {
	int first = x_tile + static_cast<int>(tile_scale - 1) / 2;
	int last = x_tile + static_cast<int>(tile_scale) / 2;
	int count = 0;
	for (int y = 0; y < std::min(static_cast<int>(tile_scale), static_cast<int>(level_data.height) - y_tile); ++y) {
		for (int x = x_tile; x < std::min(x_tile + static_cast<int>(tile_scale), static_cast<int>(level_data.width)); ++x) {
			const int index = x + static_cast<int>(level_data.width) * (y + y_tile);
			if (x >= first && x <= last) {
				set_tile(index, 0, 0xFF, 2);
			} else {
				set_tile(index, 0, 0xFF, 0);
			}
		}
		if ((y + tile_scale) % 2 == 1) {
			first--;
			last++;
		}
	}
	set_tile(x_tile + y_tile * static_cast<int>(level_data.width), tile_scale, selected);
}

void LevelMaker::tile_press(const bool put) {
	updated = true;
	const int mouseX = window_state->mouseX;
	const int mouseY = window_state->mouseY;
	if (is_pressed(tiles_viewport, mouseX, mouseY)) {
		const int index = (mouseX - tiles_viewport.x) / TILE_SELECTOR_SIZE 
			+ ((mouseY - tiles_viewport.y) / TILE_SELECTOR_SIZE) * TILE_SELECTOR_TW;
		if (index >= 0 && index < level_config.img_tilecount) {
			selected = index;
		}
	} else if (is_pressed(editor_viewport, mouseX, mouseY)) {
		const double real_tile_scale = DEFAULT_TS * SCALE_FACTORS[scale_factor];
		const int x_tile = static_cast<int>((mouseX + camera_x - editor_viewport.x) / real_tile_scale);
		const int y_tile = static_cast<int>((mouseY + camera_y - editor_viewport.y) / real_tile_scale);

		const int index = x_tile + y_tile * static_cast<int>(level_data.width);

		if (editor_mode == PLACE_TILES) {
			if (selected - level_config.img_tilecount == static_cast<int>(LevelObject::SPIKE) && put) {
				place_spike(x_tile, y_tile);
			} else {
				for (int y  = y_tile; y < std::min(y_tile + static_cast<int>(tile_scale), static_cast<int>(level_data.height)); ++y) {
					for (int x = x_tile; x < std::min(x_tile + static_cast<int>(tile_scale), static_cast<int>(level_data.width)); ++x) {
						int i = x + static_cast<int>(level_data.width) * y;
						if (tile_collisions) {
							set_tile(i, 0, 0xFF, put ? 1 : 0);
						} else {
							set_tile(i, 0, 0xFF);
						}
					}
				}
				const Uint32 img = put ? selected : 0xFF;
				const Uint32 ts = put ? tile_scale : 0;
				if (tile_collisions) {
					set_tile(index, ts, img, put ? 1 : 0);
				} else {
					set_tile(index, ts, img);
				}
			}
		} else {
			level_data.data[index] = (level_data.data[index] & 0xFFFFFF00) | (put ? 1 : 0);
		}
	} else if (is_pressed(objects_viewport, mouseX, mouseY)) {
		const int index = (mouseX - objects_viewport.x) / TILE_SELECTOR_SIZE 
			+ ((mouseY - objects_viewport.y) / TILE_SELECTOR_SIZE) * TILE_SELECTOR_TW;
		if (index >= 0 && index < TOTAL_OBJECTS) {
			selected = level_config.img_tilecount + index;
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
}
		
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
	} else if (collisions_input->is_targeted(key, mouse)) {
		editor_mode = PLACE_COLLISIONS;
		updated = true;
	}
	if (tile_collisions_input->is_targeted(key, mouse)) {
        tile_collisions = !tile_collisions;
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
		SDL_GetRelativeMouseState(nullptr, nullptr);
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
					SDL_BlitScaled(
						tile_index < level_config.img_tilecount ? tiles.get() : objects.get(),
						&source, window_surface, &dest
					);
				}
			} else {
				SDL_Rect dest = {
					static_cast<int>(editor_viewport.x + x * ts - camera_x),
					static_cast<int>(editor_viewport.y + y * ts - camera_y), 
					static_cast<int>(ts), static_cast<int>(ts)
				};
				Uint16 tile_type = (level_data.data[x + level_data.width * y] & 0xFF);
				if (tile_type == 1) {
					SDL_FillRect(window_surface, &dest, SDL_MapRGB(window_surface->format, 0xFF, 0, 0));
				} else if (tile_type == 2) {
					SDL_FillRect(window_surface, &dest, SDL_MapRGB(window_surface->format, 0xFF, 0x7F, 0));
				}
			}
		}
	}

	SDL_SetClipRect(window_surface, nullptr);
	SDL_FillRect(window_surface, &tiles_viewport, SDL_MapRGB(window_surface->format, 0xFF, 0xAA, 0));
	for (int i = 0; i < level_config.img_tilecount; ++i) {
		SDL_Rect tile_rect = get_tile_rect(i, level_config);
		SDL_Rect dest = {
			tiles_viewport.x + (i % TILE_SELECTOR_TW) * TILE_SELECTOR_SIZE,
			(i / TILE_SELECTOR_TW) * TILE_SELECTOR_SIZE, 
			TILE_SELECTOR_SIZE, TILE_SELECTOR_SIZE
		};
		SDL_BlitScaled(tiles.get(), &tile_rect, window_surface, &dest);
		if (i == selected) {
			SDL_BlitScaled(marker.get(), nullptr, window_surface, &dest);
		}
	}
	
	SDL_Rect dest = {
		objects_viewport.x, objects_viewport.y, TILE_SELECTOR_SIZE, TILE_SELECTOR_SIZE
	};
    SDL_Rect src = get_tile_rect(level_config.img_tilecount, level_config);
	SDL_BlitScaled(objects.get(), &src, window_surface, &dest);
	if (selected - level_config.img_tilecount == 0) {
		SDL_BlitScaled(marker.get(), nullptr, window_surface, &dest);
	}
	updated = false;
	
	SDL_UpdateWindowSurface(gWindow);
}
