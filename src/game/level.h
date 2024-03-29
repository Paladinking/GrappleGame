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

	std::unique_ptr<Uint32[]> data;

	void load_from_file(const std::string& path, Uint32 tile_count);
	
	void write_to_file(const std::string& path) const;
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
		double x = 0, y = 0;
		bool ignored = false;

		Corner(double x, double y) : x(x), y(y) {}

		Corner() = default;

};

enum class Tile : Uint16 {
	EMPTY, BLOCKED, SPIKES, TOTAL
};

enum class LevelObject {
	SPIKE = 0, TOTAL = 1
};

class Level : public TileMap<Tile> {
	public:
		explicit Level(const int tile_size) : TileMap(Tile::BLOCKED) {
			this->tile_size = tile_size;
		}

		void load_from_file(const std::string& path, const JsonObject& config);

		void render(int cameraY);

		std::vector<std::shared_ptr<Corner>>& get_corners();

		void set_screen_size(int screen_width, int screen_height);

	private:
		int screen_width = 0, screen_height = 0;

		std::vector<Texture> level_textures;

		std::vector<std::shared_ptr<Corner>> corners;

		void create_corners();

};

SDL_Rect get_tile_rect(Uint16 index, const LevelConfig& level_config);
#endif
