#include "levelMaker.h"
#include "file/Json.h"
#include "util/exceptions.h"
#include <algorithm>

constexpr int TILE_IMG_SIZE = 32;
constexpr int TILE_IMG_TW = 12;
constexpr int TILE_IMG_TH = 8;
constexpr int TILE_COUNT = 96;
constexpr int TILE_SELECTOR_SIZE = 64;
constexpr int TILE_SELECTOR_TW = 10;
constexpr int TILE_SELECTOR_TH = 10;
constexpr int EDITOR_W = 640;
constexpr int EDITOR_H = 640;
constexpr int SELECTOR_W = 640;
constexpr int SELECTOR_H = EDITOR_H;

constexpr int ZOOM_FACTOR = 4;
constexpr int ZOOM_MAX = 19;

void LevelMaker::init() {
	try {
		load_globals();
	} catch (base_exception &e) {
		std::cout << e.msg << "Using default globals" << std::endl;
	}

	JsonObject options, controls;
	try {
		options = json::read_from_file(CONFIG_ROOT + OPTIONS_FILE);
		if (options.has_key_of_type<JsonObject>("CONTROLS")) {
			controls = options.get<JsonObject>("CONTROLS");
		} else {
			std::cout << "No controls in options\nUsing default bindings" << std::endl;
		}
	} catch (base_exception &e) {
		std::cout << e.msg << "Using default bindings" << std::endl;
	}
	
	zoom_in_input = get_press_input(controls.get_default<std::string>("zoom_in", input::ZOOM_IN), input::ZOOM_IN);
	zoom_out_input = get_press_input(controls.get_default<std::string>("zoom_out", input::ZOOM_OUT), input::ZOOM_OUT);
	put_tile_input = get_press_input(controls.get_default<std::string>("place_tile", input::PLACE_TILE), input::PLACE_TILE);
	clear_tile_input = get_press_input(controls.get_default<std::string>("clear, tile", input::CLEAR_TILE), input::CLEAR_TILE);
	
	left_input = get_press_input(controls.get_default<std::string>("navigate_left", input::LM_LEFT), input::LM_LEFT);
	right_input = get_press_input(controls.get_default<std::string>("navigate_right", input::LM_RIGHT), input::LM_RIGHT);
	up_input = get_press_input(controls.get_default<std::string>("navigate_up", input::LM_UP), input::LM_UP);
	down_input = get_press_input(controls.get_default<std::string>("navigate_down", input::LM_DOWN), input::LM_DOWN);
	
	data = std::make_unique<Uint16[]>(width * height);

	tiles = IMG_Load("assets/tiles/tiles.png");
	
	marker = IMG_Load("assets/marker.png");
	
	window_surface = SDL_GetWindowSurface(gWindow);
	
	tile_size = EDITOR_W / width;
	
	for (int i = 0; i < width * height; ++i) data[i] = 0xFF;
}


void LevelMaker::handle_keydown(SDL_KeyboardEvent e) {
	handle_down(e.keysym.sym, 0);
}

SDL_Rect get_tile_rect(int index) {
	return {(index % TILE_IMG_TW) * TILE_IMG_SIZE, (index / TILE_IMG_TW) * TILE_IMG_SIZE,  TILE_IMG_SIZE, TILE_IMG_SIZE};
}

void LevelMaker::handle_mousedown(SDL_MouseButtonEvent e) {
	handle_down(SDLK_UNKNOWN, e.button);
	
}

void LevelMaker::tile_press(const bool put) {
	if (mouseX > EDITOR_W) {
		int index = (mouseX - EDITOR_W) / TILE_SELECTOR_SIZE + (mouseY / TILE_SELECTOR_SIZE) * TILE_SELECTOR_TW;
		if (index >= 0 && index < TILE_COUNT) {
			selected = index;
		}
	} else {
		int zoom_tile_size = EDITOR_W / (x_end - x_start);
		int x_tile = x_start + (mouseX / zoom_tile_size);
		int y_tile = y_start + (mouseY / zoom_tile_size);
		int index = x_tile + y_tile * width;
		data[index] = put ? selected : 0xFF;
	}
}
		
void LevelMaker::handle_down(const SDL_Keycode key, const Uint8 mouse) {
	if (put_tile_input->is_targeted(key, mouse)) {
		tile_press(true);
	} 
	if (clear_tile_input->is_targeted(key, mouse)) {
		tile_press(false);
	}
	if (zoom_in_input->is_targeted(key, mouse) && zoom_level < ZOOM_MAX) {
		zoom_level++;
		x_start += ZOOM_FACTOR / 2;
		x_end -= ZOOM_FACTOR / 2;
		y_start += ZOOM_FACTOR / 2;
		y_end -= ZOOM_FACTOR / 2;
	} else if (zoom_out_input->is_targeted(key, mouse) && zoom_level > 0) {
		zoom_level--;
		int delta_x_start = std::min(ZOOM_FACTOR / 2, x_start) - std::min(0, width - x_end - 2);
		int delta_y_start = std::min(ZOOM_FACTOR / 2, y_start) - std::min(0, height - y_end - 2);
		
		x_start -= delta_x_start;
		x_end += ZOOM_FACTOR - delta_x_start;
		y_start -= delta_y_start;
		y_end += ZOOM_FACTOR - delta_y_start;
	} 
	if (left_input->is_targeted(key, mouse) && x_start > 0) {
		x_start--;
		x_end--;
	} else if (right_input->is_targeted(key, mouse) && x_end < width) {
		x_start++;
		x_end++;
	}
	if (up_input->is_targeted(key, mouse) && y_start > 0) {
		y_start--;
		y_end--;
	} else if (down_input->is_targeted(key, mouse) && y_end < height) {
		y_start++;
		y_end++;
	}
}



void LevelMaker::render() {
	SDL_Rect r = {0, 0, SELECTOR_W + EDITOR_W, EDITOR_H};
	SDL_FillRect(window_surface, &r, SDL_MapRGB(window_surface->format, 255, 255, 255));
	int ts = tile_size * ((double)width / ((double)x_end - (double)x_start));
	for (int x = x_start; x < x_end; ++x) {
		for (int y = y_start; y < y_end; ++y) {
			int tile_index = data[x + width * y];
			if (tile_index != 0xFF) {
				SDL_Rect source = get_tile_rect(tile_index);
				SDL_Rect dest = {(x - x_start) * ts, (y - y_start) * ts, ts, ts};
				SDL_BlitScaled(tiles, &source, window_surface, &dest);
			}
		}
	}
	

	for (int i = 0; i < TILE_COUNT; ++i) {
		SDL_Rect tile_rect = get_tile_rect(i);
		SDL_Rect dest = {
			EDITOR_W + (i % TILE_SELECTOR_TW) * TILE_SELECTOR_SIZE, 
			(i / TILE_SELECTOR_TW) * TILE_SELECTOR_SIZE, 
			TILE_SELECTOR_SIZE, TILE_SELECTOR_SIZE
		};
		SDL_BlitScaled(tiles, &tile_rect, window_surface, &dest);
		if (i == selected) {
			SDL_BlitScaled(marker, NULL, window_surface, &dest);
		}
	}
	
	
	
	SDL_UpdateWindowSurface(gWindow);
}


void LevelMaker::destroy_game() {
	Game::destroy_game();
	SDL_FreeSurface(tiles);
	SDL_FreeSurface(marker);
}
