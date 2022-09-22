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

constexpr int ZOOM_FACTOR = 4;
constexpr int ZOOM_MAX = 19;

void LevelMaker::init() {
	State::init();
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
	
	zoom_in_input = get_press_input("zoom_in", input::ZOOM_IN, controls);
	zoom_out_input = get_press_input("zoom_out", input::ZOOM_OUT, controls);
	put_tile_input = get_press_input("place_tile", input::PLACE_TILE, controls);
	clear_tile_input = get_press_input("clear, tile", input::CLEAR_TILE, controls);
	
	left_input = get_press_input("navigate_left", input::LM_LEFT, controls);
	right_input = get_press_input("navigate_right", input::LM_RIGHT, controls);
	up_input = get_press_input("navigate_up", input::LM_UP, controls);
	down_input = get_press_input("navigate_down", input::LM_DOWN, controls);
	
	editor_width = window_width / 2;

	data = std::make_unique<Uint16[]>(width * height);

	SDL_Surface* t = IMG_Load("assets/tiles/tiles.png");
	if (t == NULL) {
		throw image_load_exception(std::string(IMG_GetError()));
	}
	tiles.reset(t);
	
	SDL_Surface* m = IMG_Load("assets/marker.png");
	if (m == NULL) {
		throw image_load_exception(std::string(IMG_GetError()));
	}
	marker.reset(m);
	
	window_surface = SDL_GetWindowSurface(gWindow);
	
	tile_size = editor_width / width;
	
	for (int i = 0; i < width * height; ++i) data[i] = 0xFF;
}

SDL_Rect get_tile_rect(int index) {
	return {(index % TILE_IMG_TW) * TILE_IMG_SIZE, (index / TILE_IMG_TW) * TILE_IMG_SIZE,  TILE_IMG_SIZE, TILE_IMG_SIZE};
}

void LevelMaker::tile_press(const bool put) {
	if (mouseX > editor_width) {
		int index = (mouseX - editor_width) / TILE_SELECTOR_SIZE + (mouseY / TILE_SELECTOR_SIZE) * TILE_SELECTOR_TW;
		if (index >= 0 && index < TILE_COUNT) {
			selected = index;
		}
	} else {
		int zoom_tile_size = editor_width / (x_end - x_start);
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
	SDL_Rect r = {0, 0, static_cast<int>(window_width), static_cast<int>(window_height)};
	SDL_FillRect(window_surface, &r, SDL_MapRGB(window_surface->format, 255, 255, 255));
	int ts = static_cast<int>(tile_size * (static_cast<double>(width) / static_cast<double>(x_end - x_start)));
	for (int x = x_start; x < x_end; ++x) {
		for (int y = y_start; y < y_end; ++y) {
			int tile_index = data[x + width * y];
			if (tile_index != 0xFF) {
				SDL_Rect source = get_tile_rect(tile_index);
				SDL_Rect dest = {(x - x_start) * ts, (y - y_start) * ts, ts, ts};
				SDL_BlitScaled(tiles.get(), &source, window_surface, &dest);
			}
		}
	}


	for (int i = 0; i < TILE_COUNT; ++i) {
		SDL_Rect tile_rect = get_tile_rect(i);
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
