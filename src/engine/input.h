#ifndef INPUT_00_H
#define INPUT_00_H
#include "file/json.h"
#include "util/exceptions.h"
#include <memory>
#include <string>
#include <SDL.h>

class binding_exception : public base_exception {
	public:
		binding_exception(std::string msg) : base_exception(msg) {};
	
};

namespace input {
	const std::string // Default bindings
		//Regular game
		LEFT = "A",
		RIGHT = "D", 
		JUMP = "Space", 
		PULL = "Q", 
		RELEASE = "E", 
		RETURN = "Left Shift",
		GRAPPLE = "Mouse Left",
		
		//Level maker controlls
		ZOOM_IN = "+",
		ZOOM_OUT = "-",
		PLACE_TILE = "Mouse Left",
		CLEAR_TILE = "Mouse Right",
		LM_LEFT = "Left",
		LM_RIGHT = "Right",
		LM_UP = "Up",
		LM_DOWN = "Down";
};

class PressInput {
	public:
		virtual bool is_targeted(const SDL_Keycode key, const Uint32 mouseButton) = 0;
};

class MousePressInput : public PressInput {
	public:
		MousePressInput(const Uint32 mouse_button) : mouse_button(mouse_button) {}
		
		virtual bool is_targeted(const SDL_Keycode key, const Uint32 mouseButton) override {
			return mouseButton == mouse_button;
		}

	private:
		const Uint32 mouse_button;
};

class KeyPressInput : public PressInput {
	public:
		KeyPressInput(const SDL_Keycode key_code) : key_code(key_code) {}
		
		virtual bool is_targeted(const SDL_Keycode key, const Uint32 mouseButton) override {
			return key_code == key;
		}
	
	private:
		const SDL_Keycode key_code;
};

class HoldInput {
	public:
		virtual bool is_pressed(const Uint8* keys, const Uint32  mouseButton) = 0;
};

class MouseHoldInput : public HoldInput {
	public:
		MouseHoldInput(const Uint32 mouse_mask) : mouse_mask(mouse_mask) {}
	
		virtual bool is_pressed(const Uint8* keys, const Uint32 mouseButton) override {
			return mouse_mask & mouseButton != 0;
		}

	private:
		const Uint32 mouse_mask;
};

class KeyHoldInput : public HoldInput {
	public:
		KeyHoldInput(const SDL_Scancode key) : key(key) {}
	
		virtual bool is_pressed(const Uint8* keys, const Uint32 mouseButton) override {
			return keys[key];
		}

	private:
		const SDL_Scancode key;
};

std::unique_ptr<PressInput> get_press_input(const std::string& name);

std::unique_ptr<HoldInput> get_hold_input(const std::string& name);

std::unique_ptr<PressInput> get_press_input(const std::string& name, const std::string& default_name);

std::unique_ptr<HoldInput> get_hold_input(const std::string& name, const std::string& default_name);

std::unique_ptr<PressInput> get_press_input(const std::string& key_name, const std::string& default_name, const JsonObject& obj);

std::unique_ptr<HoldInput> get_hold_input(const std::string& key_name, const std::string& default_name, const JsonObject& obj);



#endif