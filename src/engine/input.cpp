#include "input.h"
#include "util/exceptions.h"
#include <iostream>

std::string get_input_name(const SDL_Keycode key, const Uint32 mouse) {
	if (mouse == SDL_BUTTON_LEFT) {
		return "Mouse Left";
	} else if (mouse == SDL_BUTTON_MIDDLE) {
		return "Mouse Middle";
	} else if (mouse == SDL_BUTTON_RIGHT) {
		return "Mouse Right";
	}
	
	const char* name = SDL_GetKeyName(key);
	if (name != "") {
		return std::string(name);
	}
	return "None";
}

std::unique_ptr<PressInput> get_press_input(const std::string& name, const std::string& default_name) {
	try {
		return get_press_input(name);
	} catch (const binding_exception&) {
		std::cout << "Invalid key " << name << " using " << default_name << std::endl;
		return get_press_input(default_name);
	}
}

std::unique_ptr<HoldInput> get_hold_input(const std::string& name, const std::string& default_name) {
	try {
		return get_hold_input(name);
	} catch (const binding_exception&) {
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
	} else if (name == "None") {
		return std::unique_ptr<PressInput>(new EmptyPressInput());
	}
	SDL_Keycode keycode = SDL_GetKeyFromName(name.c_str());
	if (keycode == SDLK_UNKNOWN) {
		throw binding_exception("Invalid key name \"" + name + "\"");
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
	} else if (name == "None") {
		return std::unique_ptr<HoldInput>(new EmptyHoldInput());
	}
	
	SDL_Scancode scancode = SDL_GetScancodeFromName(name.c_str());
	if (scancode == SDL_SCANCODE_UNKNOWN) {
		throw binding_exception("Invalid key name \"" + name + "\"");
	}
	return std::unique_ptr<HoldInput>(new KeyHoldInput(scancode));
}