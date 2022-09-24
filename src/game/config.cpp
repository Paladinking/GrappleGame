#include "config.h"
#include <iostream>

static bool options_loaded = false;
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
		if (VERBOSE) std::cout << "Loading " << CONFIG_ROOT << OPTIONS_FILE << std::endl;
		try {
			options = json::read_from_file(CONFIG_ROOT + OPTIONS_FILE);
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