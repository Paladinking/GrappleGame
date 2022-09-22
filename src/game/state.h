#ifndef STATE_00_H
#define STATE_00_H
#include "game.h"

class State;

struct StateStatus {
	enum {NONE, SWAP, PUSH, POP, EXIT} action = NONE;
	// Not smart pointer, will be put in unique_ptr on the State stack later
	State* new_state = nullptr;
};

class State {
	public:
		virtual void init() = 0;
		
		virtual void tick(const Uint64 delta, const int mouseX, const int mouseY, StateStatus& res) {};
		
		virtual void render() {};
		
		virtual void handle_down(const SDL_Keycode key, const Uint8 mouse) {};
		
		virtual void handle_up(const SDL_Keycode key, const Uint8 mouse) {};
};

#endif