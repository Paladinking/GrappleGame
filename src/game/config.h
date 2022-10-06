#ifndef CONFIG_00_H
#define CONFIG_00_H
#include "file/json.h"
#include "globals.h"
#include "engine/texture.h"
#include <vector>
#include <array>

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
			{"save_level", "S"}
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
	
	std::pair<std::string, std::string> get_level(const int index);

	void write_options();
	
	void reset_bindings();
	
	void init();
	
	const JsonObject& get_template(const std::string& name);
	
	std::string get_asset_path(const std::string& path);
	
	std::string get_default_tileset();
}


#endif