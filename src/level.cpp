#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include "level.h"
#include "fileIO.h"
#include "json.h"
#include "exceptions.h"



struct SurfaceDeleter {
	void operator()(SDL_Surface* s) {SDL_FreeSurface(s);}
};

void Level::load_from_file(std::string path) {

	FileReader reader = FileReader(path, true);
	Uint32 width, height;
	if (!(reader.read_next(width) && reader.read_next(height) && 
		width == window_width / TILE_SIZE && height % (window_height / TILE_SIZE) == 0
	)) {
		throw game_exception("Invalid level file");
	}
	
	Uint32 img_tilesize, img_tilewidth, img_tilecount;
	if (!(reader.read_next(img_tilesize) && reader.read_next(img_tilewidth) && reader.read_next(img_tilecount))) {
		throw game_exception("Invalid level file");
	}
	
	char* tile_path;
	if (!(reader.read_string(tile_path))) {
		throw game_exception("Invalid level file");
	}
	
	std::unique_ptr<SDL_Surface, SurfaceDeleter> tiles(IMG_Load(tile_path));
	delete[] tile_path;
	
	if (tiles == nullptr) {
		
		throw image_load_exception(std::string(IMG_GetError()));
	}
	
	std::unique_ptr<Uint16[]> data = std::make_unique<Uint16[]>(width * height);
	if (!reader.read_many(data.get(), width * height)) {
		throw game_exception("Invalid level file");
	}

	int visible_screens = height / (window_height / TILE_SIZE);
	
	std::unique_ptr<std::unique_ptr<SDL_Surface, SurfaceDeleter>[]> surfaces = 
		std::make_unique<std::unique_ptr<SDL_Surface, SurfaceDeleter>[]>(visible_screens);
	
	//SDL_Surface** surfaces = new SDL_Surface*[visible_screens];
	for (int i = 0; i < visible_screens; ++i) {
		surfaces[i].reset(SDL_CreateRGBSurfaceWithFormat(
			0, window_width, window_height, tiles->format->BitsPerPixel, tiles->format->format
		));
	}
	
	map = std::make_unique<bool[]>(width * height);
	for (int i = 0; i < width * height; ++i) {
		Uint16 tile = data[i];
		bool filled = (tile >> 8) != 0;
		int tile_index = (tile & 0xFF);
		
		map[i] = filled;
		
		if (tile_index == 0xFF) continue;
		if (tile_index >= img_tilecount) {
			throw game_exception("Invalid tile " + std::to_string(tile_index) + " in level file");
		}
		
		SDL_Rect source = {
			(tile_index % (int)img_tilewidth) * (int)img_tilesize, 
			(tile_index / (int)img_tilewidth) * (int)img_tilesize,
			(int)img_tilesize, 
			(int)img_tilesize
		};
		
		SDL_Rect dest = {
			(i % (int)width) * TILE_SIZE, (i / (int)width) * TILE_SIZE, TILE_SIZE, TILE_SIZE
		};
		
		SDL_BlitScaled(tiles.get(), &source, surfaces[(i / width) / (window_height / TILE_SIZE)].get(), &dest);
	}
	
	level_textures.clear();
	for (int i = 0; i < visible_screens; ++i) {
		SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surfaces[i].get());
		level_textures.emplace_back(texture, window_width, window_height);
	}
}

void Level::render(int cameraY) {
	int first = cameraY / window_height;
	int last = (cameraY + window_height) / window_height;
	for (int i = first; i < last; ++i) {
		level_textures[i].render(0, i * window_height);
	}
}