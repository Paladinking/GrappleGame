#ifndef LEVEL_00_H
#define LEVEL_00_H
#include "util/utilities.h"
#include "engine/texture.h"


/**
 * LevelData is a struct representing the data contained in a level file.
 */
struct LevelData {
	Uint32 width;
	Uint32 height;
	Uint32 img_tilesize;
	Uint32 img_tilewidth;
	Uint32 img_tilecount;
	std::string img_path;
	std::unique_ptr<Uint16[]> data;

	void load_from_file(const std::string& path);
	
	void write_to_file(const std::string& path);
};

class Corner {
	public:
		double x, y;
		bool ignored = false;

		Corner(double x, double y) : x(x), y(y) {}

		Corner() {}

};

class Level : public TileMap {
	public:
		Level() {}

		void load_from_file(const std::string& path);

		void render(int cameraY);

		std::vector<std::shared_ptr<Corner>>& get_corners();

		void set_window_size(const int win_width, const int win_height);

	private:
		int window_width, window_height;

		std::vector<Texture> level_textures;

		std::vector<std::shared_ptr<Corner>> corners;

		void create_corners();

};
#endif
