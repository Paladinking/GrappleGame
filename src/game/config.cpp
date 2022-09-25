#include "config.h"
#include "file/fileIO.h"
#include <iostream>

bool options_loaded = false;
JsonObject options;

void add_bindings() {
	JsonObject& obj = options.get<JsonObject>(bindings::KEY_NAME);
	for (auto& g : bindings::GROUPINGS) {
		if (!obj.has_key_of_type<JsonObject>(g->key)) {
			obj.set<JsonObject>(g->key, JsonObject());
		}
		JsonObject& group_bindings = obj.get<JsonObject>(g->key);
		for (auto& it : g->defaults) {
			if (!group_bindings.has_key_of_type<std::string>(it.first)) {
				group_bindings.set<std::string>(it.first, it.second);
			}
		}
	}
}

JsonObject& config::get_options() {
	if (!options_loaded) {
		if (VERBOSE) std::cout << "Loading " << CONFIG_ROOT << OPTION_FILE << std::endl;
		try {
			options = json::read_from_file(CONFIG_ROOT + OPTION_FILE);
			if (!options.has_key_of_type<JsonObject>(bindings::KEY_NAME)) {
				if (VERBOSE) std::cout << "No bindings in options file, using default bindings" << std::endl;
				options.set<JsonObject>(bindings::KEY_NAME, JsonObject());
			}
		} catch (const base_exception& e) {
			if (VERBOSE) std::cout << e.msg << "\nUsing default options" << std::endl;
			options.clear();
			options.set<JsonObject>(bindings::KEY_NAME, JsonObject());
		}
		add_bindings();
		options_loaded = true;
	}

	return options;
}

JsonObject& config::get_bindings() {
	return config::get_options().get<JsonObject>(bindings::KEY_NAME);
}


JsonObject& config::get_bindings(const std::string& key) {
	return config::get_bindings().get<JsonObject>(key);
}

void config::write_options() {
	try {
		json::write_to_file(CONFIG_ROOT + OPTION_FILE, options);
	} catch(const base_exception& e) {
		std::cout << e.msg << std::endl;
	}
}

void config::reset_bindings() {
	options.set<JsonObject>(bindings::KEY_NAME, JsonObject());
	add_bindings();
}

constexpr int TOTAL_LEVELS = 1;

bool levels_loaded = false;
JsonObject levels;

std::pair<std::string, std::string> config::get_level(const int index) {
	if (!levels_loaded) {
		if (VERBOSE) std::cout << "Loading " << CONFIG_ROOT << LEVELS_FILE << std::endl;
		//No try ... catch since failing to load levels cannot be handled.
		levels = json::read_from_file(CONFIG_ROOT + LEVELS_FILE);
		if (!levels.has_key_of_type<JsonList>("LEVELS")) {
			throw file_exception("Invalid levels file");
		}
		JsonList& lvls = levels.get<JsonList>("LEVELS");
		if (lvls.size() < TOTAL_LEVELS) {
			throw file_exception("Levels missing from levels file");
		}
		for (int i = 0; i < lvls.size(); ++i) {
			if(!lvls.has_index_of_type<JsonObject>(i)) throw file_exception("Invalid levels file");
			JsonObject& obj = lvls.get<JsonObject>(i);
			if (!obj.has_key_of_type<std::string>("file") || !obj.has_key_of_type<std::string>("tiles")) {
				throw file_exception("Level" + std::to_string(i) + " is invalid");
			}
		}
	}
	const JsonObject& lvl = levels.get<JsonList>("LEVELS").get<JsonObject>(index);
	return {LEVELS_ROOT + lvl.get<std::string>("file"), ASSETS_ROOT + lvl.get<std::string>("tiles")};
}