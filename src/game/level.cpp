#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include "level.h"
#include "engine/engine.h"
#include "file/fileIO.h"
#include "file/json.h"
#include "util/exceptions.h"
#include "globals.h"


void LevelData::load_from_file(const std::string& path) {
	FileReader reader = FileReader(path, true);
	char* img_path_cstr;
	if (
		!reader.read_next(width) ||
		!reader.read_next(height) ||
		!reader.read_next(img_tilesize) ||
		!reader.read_next(img_tilewidth) ||
		!reader.read_next(img_tilecount) ||
		!reader.read_string(img_path_cstr)
	) {
		throw file_exception("Could not read level file");
	}
	img_path = std::string(img_path_cstr);
	delete[] img_path_cstr;
	data = std::make_unique<Uint16[]>(width * height);
	if (!reader.read_many(data.get(), width * height)) {
		throw file_exception("Could not read level file");
	}
}

void LevelData::write_to_file(const std::string& path) {
	FileWriter writer = FileWriter(path, true);
	if (
		!writer.write(width) ||
		!writer.write(height) ||
		!writer.write(img_tilesize) ||
		!writer.write(img_tilewidth) ||
		!writer.write(img_tilecount) ||
		!writer.write(img_path.c_str(), img_path.length()) ||
		!writer.write_many(data.get(), width * height)
	) {
		throw file_exception("Could not write to level file");
	}
}

void Level::set_window_size(const int win_width, const int win_height) {
	window_width = win_width;
	window_height = win_height;
}

void Level::load_from_file(const std::string& path) {
	LevelData level_data;
	level_data.load_from_file(path);
	
	if (level_data.width != window_width / TILE_SIZE || level_data.height % (window_width / TILE_SIZE) != 0) {
		throw file_exception("Invalid level file");
	}
	
	std::unique_ptr<SDL_Surface, SurfaceDeleter> tiles(IMG_Load(level_data.img_path.c_str()));
	
	if (tiles == nullptr) {
		throw image_load_exception(std::string(IMG_GetError()));
	}

	int visible_screens = level_data.height / (window_height / TILE_SIZE);
	
	std::unique_ptr<std::unique_ptr<SDL_Surface, SurfaceDeleter>[]> surfaces = 
		std::make_unique<std::unique_ptr<SDL_Surface, SurfaceDeleter>[]>(visible_screens);

	for (int i = 0; i < visible_screens; ++i) {
		surfaces[i].reset(SDL_CreateRGBSurfaceWithFormat(
			0, window_width, window_height, tiles->format->BitsPerPixel, tiles->format->format
		));
	}
	
	map = std::make_unique<bool[]>(level_data.width * level_data.height);
	for (int i = 0; i < level_data.width * level_data.height; ++i) {
		Uint16 tile = level_data.data[i];
		
		int tile_index = (tile >> 8);
		bool filled = (tile & 0xFF) != 0;
		
		map[i] = filled;
		

		if (tile_index == 0xFF) {
			continue;
		};
		if (tile_index >= level_data.img_tilecount) {
			throw file_exception("Invalid tile " + std::to_string(tile_index) + " in level file");
		}
		
		SDL_Rect source = {
			(tile_index % static_cast<int>(level_data.img_tilewidth)) * static_cast<int>(level_data.img_tilesize), 
			(tile_index / static_cast<int>(level_data.img_tilewidth)) * static_cast<int>(level_data.img_tilesize),
			static_cast<int>(level_data.img_tilesize), 
			static_cast<int>(level_data.img_tilesize)
		};
		
		SDL_Rect dest = {
			(i % static_cast<int>(level_data.width)) * TILE_SIZE,
			((i / static_cast<int>(level_data.width)) * TILE_SIZE) % window_height,
			TILE_SIZE, TILE_SIZE
		};

		const int surface_index = ((i / level_data.width)) / (window_height / TILE_SIZE);

		SDL_BlitScaled(tiles.get(), &source, surfaces[surface_index].get(), &dest);
	}

	level_textures.clear();
	for (int i = 0; i < visible_screens; ++i) {
		SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surfaces[i].get());
		level_textures.emplace_back(texture, window_width, window_height);
	}
	
	
	tile_size = TILE_SIZE;
	this->width = level_data.width;
	this->height = level_data.height;
	create_corners();
}

void Level::create_corners() {
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			if (!is_blocked(x, y)) continue;
			bool top_left = true, top_right = true, botton_left = true, bottom_right = true;
			if (is_blocked(x - 1, y)) {
				top_left = false;
				botton_left = false;
			}
			if (is_blocked(x + 1, y)) {
				top_right = false;
				bottom_right = false;
			}
			if (is_blocked(x, y - 1)) {
				top_left = false;
				top_right = false;
			}
			if (is_blocked(x, y + 1)) {
				botton_left = false;
				bottom_right = false;
			}
			double x_pos = (double) x, y_pos = (double) y;
			if (top_left) {
				corners.push_back(std::shared_ptr<Corner>(new Corner(x_pos * tile_size, y_pos * tile_size)));
			}
			if (top_right) {
				corners.push_back(std::shared_ptr<Corner>(new Corner((x_pos + 1) * tile_size, y_pos * tile_size)));
			}
			if (botton_left) {
				corners.push_back(std::shared_ptr<Corner>(new Corner(x_pos * tile_size, (y_pos + 1) * tile_size)));
			}
			if (bottom_right) {
				corners.push_back(std::shared_ptr<Corner>(new Corner((x_pos + 1) * tile_size, (y_pos + 1) * tile_size)));
			}
		}
	}
}

std::vector<std::shared_ptr<Corner>>& Level::get_corners() {
	return corners;
}

void Level::render(int cameraY) {
	int first = cameraY / window_height;
	int last = (cameraY + window_height) / window_height;
	for (int i = 0; i < 2; ++i) {
		level_textures[i].render(0, i * window_height - cameraY);
	}
}