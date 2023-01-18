#ifndef CONFIG_00_H
#define CONFIG_00_H
#include "file/json.h"
#include "globals.h"
#include "engine/texture.h"
#include <vector>
#include <array>
#include <tuple>

namespace bindings {
	
	struct Grouping {
		const std::string key;
		const std::vector<std::pair<std::string, const std::string>> defaults;
	};
	
	const std::string KEY_NAME = "BINDINGS";
	
	const Grouping CLIMBGAME = {
		"CLIMBGAME",
		{
			{"grapple", "Mouse Left"},
			{"pull", "Q"},
			{"release", "E"},
			{"return_grapple", "Left Shift"},
			{"jump", "Space"},
			{"left", "A"},
			{"right", "D"}
		}
	};

	const Grouping LEVELMAKER = {
		"LEVELMAKER",
		{
			{"place_tile", "Mouse Left"},
			{"clear_tile", "Mouse Right"},
			{"zoom_in", "+"},
			{"zoom_out", "-"},
			{"navigate_up", "Up"},
			{"navigate_down", "Down"},
			{"navigate_left", "Left"},
			{"navigate_right", "Right"},
			{"save_level", "S"},
			{"tiles_mode", "1"},
			{"collision_mode", "2"},
			{"tile_collisions", "B"},
			{"tile_scale_up", "Q"},
			{"tile_scale_down", "E"},
			{"camera_pan", "Mouse Middle"}
		}
	};
	
	const Grouping GENERAL = {
		"GENERAL",
		{
			{"exit_menu", "Escape"}
		}
	};
	
	const std::array<const Grouping*, 3> GROUPINGS = {&CLIMBGAME, &LEVELMAKER, &GENERAL}; 
}

namespace config {
	
	JsonObject& get_options();
	
	JsonObject& get_bindings();
	
	JsonObject& get_bindings(const std::string& key);
	
	const JsonList& get_levels();

	const JsonObject& get_level(int index);

	const JsonObject& get_level_config(const std::string& key);
	
	std::pair<std::string, const JsonObject&> get_level_and_config(int index);

	void write_options();
	
	void reset_bindings();
	
	void init();
	
	const JsonObject& get_template(const std::string& name);
	
	std::string get_asset_path(const std::string& path);
	
	std::string get_level_path(const std::string& path);
}


#endif