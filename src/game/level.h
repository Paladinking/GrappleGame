#ifndef LEVEL_00_H
#define LEVEL_00_H
#include "util/utilities.h"
#include "texture.h"



class Level : public TileMap {
	public:
		Level() {}
		
		void load_from_file(std::string path);
	
		void render(int cameraY);
		
		std::vector<std::shared_ptr<Corner>>& get_corners();
		
		void set_window_size(const int win_width, const int win_height);
	
	private:
		int window_width, window_height;
		
		int tile_width, tile_height;
	
		std::vector<Texture> level_textures;
		
		std::vector<std::shared_ptr<Corner>> corners;
		
		void create_corners();

};
#endif