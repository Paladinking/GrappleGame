#ifndef CONFIG_00_H
#define CONFIG_00_H
#include "file/json.h"
#include "globals.h"
#include <unordered_map>
#include <array>

namespace bindings {
	
	struct Grouping {
		const std::string key;
		const std::unordered_map<std::string, const std::string> defaults;
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
			{"navigate_down", "Down"}
			{"navigate_left", "Left"},
			{"navigate_right", "Right"},
		}
	};
	
	const Grouping GENERAL = {
		"GENERAL",
		{
			{"exit_menu", "Escape"}
		}
	};
	
	const std::array<const Grouping*, 3> GROUPINGS = {&CLIMBGAME, &LEVELMAKER, &GENERAL}; 
};

namespace config {
	JsonObject& get_options();
	
	JsonObject& get_bindings();
	
	JsonObject& get_bindings(const std::string& key);
	
	void write_options();
	
	void reset_bindings();
}


#endif