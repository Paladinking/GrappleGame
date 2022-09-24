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

class PressInput {
	public:
		virtual bool is_targeted(const SDL_Keycode key, const Uint32 mouseButton) = 0;
};

class MousePressInput : public PressInput {
	public:
		MousePressInput(const Uint32 mouse_button) : mouse_button(mouse_button) {}
		
		virtual bool is_targeted(const SDL_Keycode key, const Uint32 mouse) override {
			return mouse == mouse_button;
		}

	private:
		const Uint32 mouse_button;
};

class EmptyPressInput : public PressInput {
	public:
		virtual bool is_targeted(const SDL_Keycode key, const Uint32 mouse) override {
			return false;
		}
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

class EmptyHoldInput : public HoldInput {
	public:
		virtual bool is_pressed(const Uint8* keys, const Uint32 mouseButton) override {
			return false;
		}
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