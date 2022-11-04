#include "config.h"
#include "file/fileIO.h"
#include <iostream>

#ifdef ROOT_BUILD
const std::string PROJECT_ROOT = "./";
#else
const std::string PROJECT_ROOT = "../../";
#endif
const std::string CONFIG_FILE = PROJECT_ROOT + "config.json";

bool VERBOSE = false;

std::string ASSETS_ROOT = PROJECT_ROOT + "assets/";
std::string CONFIG_ROOT = PROJECT_ROOT + "config/";
std::string LEVELS_ROOT = PROJECT_ROOT + ASSETS_ROOT + "levels/";

std::string LEVELS_FILE = "levels.json";
std::string OPTION_FILE = "options.json";
std::string STATIC_TEMPLATES_FILE = "static_templates.json";

void config::init() {
	JsonObject conf;
	try {
		conf = json::read_from_file(CONFIG_FILE);
	} catch(const base_exception& e) {
		std::cout << e.msg << std::endl;
		std::cout << "Using default configs" << std::endl;
	}

	if (conf.has_key_of_type<bool>("VERBOSE")) {
		VERBOSE = conf.get<bool>("VERBOSE");
	}

	if (conf.has_key_of_type<std::string>("ASSETS_ROOT")) {
		ASSETS_ROOT = PROJECT_ROOT + conf.get<std::string>("ASSETS_ROOT");
	}
	if (conf.has_key_of_type<std::string>("CONFIG_ROOT")) {
		CONFIG_ROOT = PROJECT_ROOT + conf.get<std::string>("CONFIG_ROOT");
	}
	if (conf.has_key_of_type<std::string>("LEVELS_ROOT")) {
		LEVELS_ROOT = PROJECT_ROOT + conf.get<std::string>("LEVELS_ROOT");
	}
	if (conf.has_key_of_type<std::string>("LEVELS_FILE")) {
		LEVELS_FILE = conf.get<std::string>("LEVELS_FILE");
	}
	if (conf.has_key_of_type<std::string>("OPTION_FILE")) {
		OPTION_FILE = conf.get<std::string>("OPTION_FILE");
	}
	if (conf.has_key_of_type<std::string>("STATIC_TEMPLATES_FILE")) {
		STATIC_TEMPLATES_FILE = conf.get<std::string>("STATIC_TEMPLATES_FILE");
	}
	
	if (!VERBOSE) return;
	std::cout << "ASSETS_ROOT: " << ASSETS_ROOT << std::endl;
	std::cout << "CONFIG_ROOT: " << CONFIG_ROOT << std::endl;
	std::cout << "LEVELS_ROOT: " << LEVELS_ROOT << std::endl;
	std::cout << "LEVELS_FILE: " << LEVELS_FILE << std::endl;
	std::cout << "OPTION_FILE: " << OPTION_FILE << std::endl;
	std::cout << "STATIC_TEMPLATES_FILE: " << STATIC_TEMPLATES_FILE << std::endl;
}

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

std::tuple<std::string, std::string, std::string> config::get_level(const int index) {
	if (!levels_loaded) {
		if (VERBOSE) std::cout << "Loading " << CONFIG_ROOT << LEVELS_FILE << std::endl;
		//No try ... catch since failing to load levels cannot be handled.
		levels = json::read_from_file(CONFIG_ROOT + LEVELS_FILE);
		if (!levels.has_key_of_type<JsonList>("LEVELS") || !levels.has_key_of_type<JsonObject>("LEVEL_CONFIGURATIONS")) {
			throw file_exception("Invalid levels file");
		}
		JsonList& lvls = levels.get<JsonList>("LEVELS");
		if (lvls.size() < TOTAL_LEVELS) {
			throw file_exception("Levels missing from levels file");
		}
		for (int i = 0; i < lvls.size(); ++i) {
			if(!lvls.has_index_of_type<JsonObject>(i)) throw file_exception("Invalid levels file");
			JsonObject& obj = lvls.get<JsonObject>(i);
			if (!obj.has_key_of_type<std::string>("file") || !obj.has_key_of_type<std::string>("config")) {
				throw file_exception("Level" + std::to_string(i) + " is invalid");
			}
		}
	}
	const JsonObject& lvl = levels.get<JsonList>("LEVELS").get<JsonObject>(index);
	const JsonObject& cnf = levels.get<JsonObject>("LEVEL_CONFIGURATIONS");
	const std::string& cnf_key = lvl.get<std::string>("config");
	if (!cnf.has_key_of_type<JsonObject>(cnf_key)) {
		throw file_exception("Level " + std::to_string(index) + " has invalid config");
	}
	const JsonObject& lvl_config = cnf.get<JsonObject>(cnf_key);
	if (!lvl_config.has_key_of_type<std::string>("tiles") || !lvl_config.has_key_of_type<std::string>("objects")) {
		throw file_exception("Level " + std::to_string(index) + " has invalid config");
	}
	return {
		LEVELS_ROOT + lvl.get<std::string>("file"),
		ASSETS_ROOT + lvl_config.get<std::string>("tiles"),
		ASSETS_ROOT + lvl_config.get<std::string>("objects")
	};
}


bool static_templates_loaded;
// Always loaded templates
JsonObject static_templates;
const JsonObject& config::get_template(const std::string& name) {
	if (!static_templates_loaded) {
		if (VERBOSE) std::cout << "Loading " << CONFIG_ROOT << STATIC_TEMPLATES_FILE << std::endl;
		static_templates = json::read_from_file(CONFIG_ROOT + STATIC_TEMPLATES_FILE);
		if(!static_templates.has_key_of_type<JsonObject>("Player")) {
			throw file_exception("Player template missing");
		}
		static_templates_loaded = true;
	}
	return static_templates.get<JsonObject>(name);
}

std::string config::get_asset_path(const std::string& path) {
	return ASSETS_ROOT + path;
}

std::string config::get_default_tileset() {
	return "tiles/tiles.png";
}