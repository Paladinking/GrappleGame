#ifndef STATE_00_H
#define STATE_00_H
class State;

#include "game.h"
#include "globals.h"

struct StateStatus {
	enum {NONE, SWAP, PUSH, POP, EXIT} action = NONE;
	// Not smart pointer, will be put in unique_ptr on the State stack later
	State* new_state = nullptr;
};

class State {
	public:
		State(const int window_width, const int window_height, const std::string& title) :
			window_width(window_width), window_height(window_height), title(title) {};
			
		State() : title(""), window_width(-1), window_height(-1) {};

		void set_mouse_state(const int mouseX, const int mouseY, const Uint8 mouseButton);

		virtual void init();

		virtual void tick(const Uint64 delta, StateStatus& res) {};

		virtual void render() {};

		virtual void handle_down(const SDL_Keycode key, const Uint8 mouse) {};

		virtual void handle_up(const SDL_Keycode key, const Uint8 mouse) {};

		const std::string& get_title() const;

		int get_width() const;

		int get_height() const;

	protected:
		std::string title;

		int window_width, window_height;

		int mouseX, mouseY;

		Uint8 mouseButton;
};

#endif
