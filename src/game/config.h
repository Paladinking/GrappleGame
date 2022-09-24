#ifndef CONFIG_00_H
#define CONFIG_00_H
#include "file/json.h"
#include "globals.h"
#include <unordered_map>

namespace bindings {
	
	const std::string KEY_NAME = "BINDINGS";
	
	const std::unordered_map<std::string, const std::string> DEFAULTS = {
		{"left", "A"},
		{"right", "D"},
		{"jump", "Space"},
		{"pull", "Q"},
		{"release", "E"},
		{"return_grapple", "Left Shift"},
		{"grapple", "Mouse Left"},

		{"zoom_in", "+"},
		{"zoom_out", "-"},
		{"navigate_left", "Left"},
		{"navigate_right", "Right"},
		{"navigate_up", "Up"},
		{"navigate_down", "Down"}
	};
};

namespace config {
	JsonObject& get_options();
}


#endif