#ifndef LEVEL_00_H
#define LEVEL_00_H
#include "util/utilities.h"
#include "engine/texture.h"
#include "file/json.h"


/**
 * LevelData is a struct representing the data contained in a level file.
 */
struct LevelData {
	Uint32 width;
	Uint32 height;
	Uint32 img_tilesize;
	Uint32 img_tilewidth;
	Uint32 img_tilecount;
	std::unique_ptr<Uint32[]> data;

	void load_from_file(const std::string& path);
	
	void write_to_file(const std::string& path);
};

/**
 * LevelConfig is a struct representing asset configuration for a level.
 */
struct LevelConfig {
	int img_tilesize;
	int img_tilewidth;
	int img_tilecount;
	std::string tiles_path;
	std::string objects_path;
	
	static LevelConfig load_from_json(const JsonObject& obj);
};

class Corner {
	public:
		double x, y;
		bool ignored = false;

		Corner(double x, double y) : x(x), y(y) {}

		Corner() {}

};

enum class Tile : Uint16 {
	EMPTY, BLOCKED, SPIKES
};

class Level : public TileMap<Tile> {
	public:
		Level(const int tile_size) : TileMap(Tile::BLOCKED) {
			this->tile_size = tile_size;
		}

		void load_from_file(const std::string& path, const JsonObject& config);

		void render(int cameraY);

		std::vector<std::shared_ptr<Corner>>& get_corners();

		void set_screen_size(const int screen_width, const int screen_height);

	private:
		int screen_width, screen_height;

		std::vector<Texture> level_textures;

		std::vector<std::shared_ptr<Corner>> corners;

		void create_corners();

};
#endif
