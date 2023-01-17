#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include "level.h"
#include "engine/engine.h"
#include "file/fileIO.h"
#include "util/exceptions.h"
#include "globals.h"
#include "config.h"

void LevelData::load_from_file(const std::string& path, const Uint32 tile_count) {
	FileReader reader = FileReader(path, true, true);
	if (!reader.read_next(width) || !reader.read_next(height)) {
		throw file_exception("Invalid level file");
	}
	data = std::make_unique<Uint32[]>(width * height);
	if (!reader.read_many(data.get(), width * height)) {
		throw file_exception("Invalid level file");
	}
	if (width != TILE_WIDTH || (height % TILE_HEIGHT) != 0) {
		throw file_exception("Bad dimensions in level file");
	}
	const Uint32 total_images = tile_count + static_cast<Uint32>(LevelObject::TOTAL);
	for (size_t i = 0; i < width * height; ++i) {
		const Uint32 type = data[i] & 0xFF; 
		const Uint32 img = (data[i] >> 8) & 0xFF;
		const Uint32 scale = (data[i] >> 16) & 0xFF;
		if (type >= static_cast<Uint16>(Tile::TOTAL) || img != 0xFF && (img >= total_images || scale > 8))
			throw file_exception("Invalid tile in level file");
	}
	
}

void LevelData::write_to_file(const std::string& path) {
	FileWriter writer = FileWriter(path, true, true);
	if (
		!writer.write(width) ||
		!writer.write(height) ||
		!writer.write_many(data.get(), width * height)
	) {
		throw file_exception("Could not write to level file");
	}
}

LevelConfig LevelConfig::load_from_json(const JsonObject& obj) {
	return {
		obj.get<int>("tile_size"),
		obj.get<int>("tile_width"),
		obj.get<int>("tile_count"),
		config::get_asset_path(obj.get<std::string>("tiles")),
		config::get_asset_path(obj.get<std::string>("objects"))
	};
}

void Level::set_screen_size(const int sw, const int sh) {
	screen_width = sw;
	screen_height = sh;
}


void Level::load_from_file(const std::string& path, const JsonObject& obj) {
	LevelConfig conf = LevelConfig::load_from_json(obj);
	LevelData level_data;
	level_data.load_from_file(path, conf.img_tilecount);


	std::unique_ptr<SDL_Surface, SurfaceDeleter> tiles(IMG_Load(conf.tiles_path.c_str()));
	if (tiles == nullptr) {
		throw image_load_exception(std::string(IMG_GetError()));
	}
	std::unique_ptr<SDL_Surface, SurfaceDeleter> objects(IMG_Load(conf.objects_path.c_str()));
	if (objects == nullptr) {
		throw image_load_exception(std::string(IMG_GetError()));
	}

	int visible_screens = level_data.height / TILE_HEIGHT;
	
	std::unique_ptr<std::unique_ptr<SDL_Surface, SurfaceDeleter>[]> surfaces = 
		std::make_unique<std::unique_ptr<SDL_Surface, SurfaceDeleter>[]>(visible_screens);

	for (int i = 0; i < visible_screens; ++i) {
		surfaces[i].reset(SDL_CreateRGBSurfaceWithFormat(
			0, screen_width, screen_height, tiles->format->BitsPerPixel, tiles->format->format
		));
	}
	
	map = std::make_unique<Tile[]>(level_data.width * level_data.height);
	for (int i = 0; static_cast<unsigned>(i) < level_data.width * level_data.height; ++i) {
		Uint32 t = level_data.data[i]; //FIX??

		int tile_index = (t >> 8) & 0xFF;
		int tile_scale = (t >> 16) & 0xFF;
		Tile tile = static_cast<Tile>(t & 0xFF);

		map[i] = tile;

		if (tile_index == 0xFF) {
			continue;
		};

		SDL_Rect source = get_tile_rect(tile_index, conf);

		SDL_Rect dest = {
			(i % static_cast<int>(level_data.width)) * tile_size,
			((i / static_cast<int>(level_data.width)) * tile_size) % screen_height,
			tile_size * tile_scale, tile_size * tile_scale
		};

		const int surface_index = (i / level_data.width) / (screen_height / tile_size);

		SDL_Surface* surface = tile_index >= conf.img_tilecount ? objects.get() : tiles.get();
		SDL_BlitScaled(surface, &source, surfaces[surface_index].get(), &dest);

		// A scaled tile might be part of two screens
		const int next_surface_index = (i / level_data.width + tile_scale - 1) / (screen_height / tile_size);
		if (next_surface_index <= visible_screens - 1 && surface_index != next_surface_index) {
			dest.y -= static_cast<int>(level_data.width) * tile_size;
			dest.h = tile_size * tile_scale; //SDL_BlitScaled modifies dest, so h needs to be changed back.
			int res = SDL_BlitScaled(surface, &source, surfaces[next_surface_index].get(), &dest);
		}
	}

	level_textures.clear();
	for (int i = 0; i < visible_screens; ++i) {
		SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surfaces[i].get());
		level_textures.emplace_back(texture, screen_width, screen_height);
	}



	this->width = static_cast<int>(level_data.width);
	this->height = static_cast<int>(level_data.height);
	create_corners();
}

void Level::create_corners() {
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			if (get_tile(x, y) != Tile::BLOCKED) continue;
			bool top_left = true, top_right = true, botton_left = true, bottom_right = true;
			if (get_tile(x - 1, y) == Tile::BLOCKED) {
				top_left = false;
				botton_left = false;
			}
			if (get_tile(x + 1, y) == Tile::BLOCKED) {
				top_right = false;
				bottom_right = false;
			}
			if (get_tile(x, y - 1) == Tile::BLOCKED) {
				top_left = false;
				top_right = false;
			}
			if (get_tile(x, y + 1) == Tile::BLOCKED) {
				botton_left = false;
				bottom_right = false;
			}
			double x_pos = static_cast<double>(x), y_pos = static_cast<double>(y);
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
	int first = cameraY / screen_height;
	int last = (cameraY + 2 * screen_height - 1) / screen_height;
	for (int i = first; i < last; ++i) {
		level_textures[i].render(0, i * screen_height - cameraY);
	}
}

SDL_Rect get_tile_rect(Uint16 index, const LevelConfig& level_config) {
	if (index < level_config.img_tilecount) {
		return {
			static_cast<int>((index % level_config.img_tilewidth) * level_config.img_tilesize),
			static_cast<int>((index / level_config.img_tilewidth) * level_config.img_tilesize),
			static_cast<int>(level_config.img_tilesize),
			static_cast<int>(level_config.img_tilesize)
		}; 
	} else if (index - level_config.img_tilecount == static_cast<int>(LevelObject::SPIKE)){
		return {
			0, 0, static_cast<int>(level_config.img_tilesize), static_cast<int>(level_config.img_tilesize)
		};
	} else {
		throw logic_exception("Rect index out of bounds");
	}
}