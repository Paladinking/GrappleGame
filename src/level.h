#ifndef LEVEL_00_H
#define LEVEL_00_H
#include "utilities.h"
#include "texture.h"



class Level : public TileMap {
	public:
		Level(const int win_width, const int win_height) : window_width(win_width), window_height(win_height) {}
		
		void load_from_file(std::string path);
	
		void render(int cameraY);
	
	private:
		int window_width, window_height;
		
		int tile_width, tile_height;
	
		std::vector<Texture> level_textures;

};
#endif