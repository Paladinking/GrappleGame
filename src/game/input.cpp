#include "input.h"
#include "util/exceptions.h"
#include <iostream>

std::unique_ptr<PressInput> get_press_input(const std::string& key_name, const std::string& default_name, const JsonObject& obj) {
	return get_press_input(obj.get_default<std::string>(key_name, default_name), default_name);
}

std::unique_ptr<HoldInput> get_hold_input(const std::string& key_name, const std::string& default_name, const JsonObject& obj) {
	return get_hold_input(obj.get_default<std::string>(key_name, default_name), default_name);
}

std::unique_ptr<PressInput> get_press_input(const std::string& name, const std::string& default_name) {
	try {
		return get_press_input(name);
	} catch (base_exception &e) {
		std::cout << "Invalid key " << name << " using " << default_name << std::endl;
		return get_press_input(default_name);
	}
}

std::unique_ptr<HoldInput> get_hold_input(const std::string& name, const std::string& default_name) {
	try {
		return get_hold_input(name);
	} catch (base_exception &e) {
		std::cout << "Invalid key \"" << name << "\" using \"" << default_name << '"' << std::endl;
		return get_hold_input(default_name);
	}	
}

std::unique_ptr<PressInput> get_press_input(const std::string& name) {

	if (name == "Mouse Left") {
		return std::unique_ptr<PressInput>(new MousePressInput(SDL_BUTTON_LEFT));
	} else if (name == "Mouse Middle") {
		return std::unique_ptr<PressInput>(new MousePressInput(SDL_BUTTON_MIDDLE));
	} else if (name == "Mouse Right") {
		return std::unique_ptr<PressInput>(new MousePressInput(SDL_BUTTON_RIGHT));
	}
	
	SDL_Keycode keycode = SDL_GetKeyFromName(name.c_str());
	if (keycode == SDLK_UNKNOWN) {
		throw game_exception("Invalid key name \"" + name + "\"");
	}
	return std::unique_ptr<PressInput>(new KeyPressInput(keycode));	
}

std::unique_ptr<HoldInput> get_hold_input(const std::string& name) {

	if (name == "Mouse Left") {
		return std::unique_ptr<HoldInput>(new MouseHoldInput(SDL_BUTTON_LMASK));
	} else if (name == "Mouse Middle") {
		return std::unique_ptr<HoldInput>(new MouseHoldInput(SDL_BUTTON_MMASK));
	} else if (name == "Mouse Right") {
		return std::unique_ptr<HoldInput>(new MouseHoldInput(SDL_BUTTON_RMASK));
	}
	
	SDL_Scancode scancode = SDL_GetScancodeFromName(name.c_str());
	if (scancode == SDL_SCANCODE_UNKNOWN) {
		throw game_exception("Invalid key name \"" + name + "\"");
	}
	return std::unique_ptr<HoldInput>(new KeyHoldInput(scancode));
}