#ifndef GAME_00_H
#define GAME_00_H
#include <memory>
#include <stack>
#include <SDL.h>
#include "util/exceptions.h"
#include "texture.h"
#include "engine.h"

/**
 * Game_exception, when creating a game fails (for example when one is already runnning).
 */
class game_exception : public base_exception {
	public:
		game_exception(std::string msg) : base_exception(msg) {};
	
};

/**
 * WindowState struct for storing state about the window.
 */
struct WindowState {
	// Actual dimensions of the window
	int window_width;
	int window_height;

	// Size of the rendering context
	int screen_width;
	int screen_height;

	int mouseX;
	int mouseY;
	Uint8 mouseButton;
};


/**
 * Bases Game class, to be extended by a more specific game. On it's own is only a white window with a title that can be closed.
 */
class Game {
	public:
		Game(const int window_width, const int window_height, const std::string& title);
		
		virtual ~Game();
		
		/**
		 * Creates a new game, opening a window and creating a renderer. Throws SDL_exception if creating window or renderer fails.
		 * Throws a Game_exception if a game is already running.
		 */
		void create();
		
		/**
		 * Starts and runs the game loop, calling tick and render every frame and handle_keydown / handle_keyup when a keypress happens.
		 * Will return after exit_game has been called, or immedietly if the game has not been created or has been destroyed.
		 * Closing the window will call exit_game and cause run to return.
		 */
		void run();
		
		/**
		 * Exits an ongoing game.
		 */
		void exit_game();
		
		/**
		 * Frees all resources used by the game.
		 */
		virtual void destroy_game();
		
	protected:
		WindowState window_state;

		/**
		 * Called once per frame, after tick has been called. 
		 * In the future the renderer might be given as a parameter here instead of being global.
		 */
		virtual void render() {};

		/**
		 * Initializes a game, called at the end of create. If init trows an exception the game will not be successfully created.
		 */
		virtual void init() {};

		/**
		 * Tick function, called once every frame, before render. Delta is the passed time in milliseconds.
		 */
		virtual void tick(Uint64 delta) {};

		/**
		 * Called every time a KEYDOWN-event happens.
		 */
		virtual void handle_keydown(SDL_KeyboardEvent &e) {};

		/**
		 * Called every time a KEYUP-event happens.
		 */
		virtual void handle_keyup(SDL_KeyboardEvent &e) {};

		/**
		 * Called every time a MOUSEDOWN-event happens.
		 */
		virtual void handle_mousedown(SDL_MouseButtonEvent &e) {};

		/**
		 * Called every time a MOUSEUP-event happens.
		 */
		virtual void handle_mouseup(SDL_MouseButtonEvent &e) {};

		/**
		 * Called every time a MOUSEWHEEL-event happens.
		 */
		 virtual void handle_mousewheel(SDL_MouseWheelEvent &e) {};

	private:
		bool running = false;
		bool destroyed = true;

		const int initial_width = 100, initial_height = 100;
		const std::string initial_title = "Title";
};

class State;

/**
 * Struct used for signaling a change of state in a StateGame.
 */
struct StateStatus {
	enum {NONE, SWAP, PUSH, POP, EXIT} action = NONE;
	// Not smart pointer, will be put in unique_ptr on the State stack later
	State* new_state = nullptr;
};

/**
 * Class containing a state. 
 * In a state game, tick, render and events are redirected to the top state.
 */
class State {
	public:
		State(WindowState* window_state) : window_state(window_state) {};

		State() : window_state(nullptr) {};

		/**
		 * Initializes this state.
		 */
		virtual void init(WindowState* window_state);
		
		/*
		 * Called when a state above this in the stack is popped.
		 */
		virtual void resume() {};

		/**
		 * Ticks the state, with a timestep delta. The parameter res is used to signal a change of state.
		 */
		virtual void tick(const Uint64 delta, StateStatus& res) {};

		/**
		 * Renders the state.
		 */
		virtual void render() {};

		/**
		 * Called when a down event (mouse or keyboard) happens.
		 */
		virtual void handle_down(const SDL_Keycode key, const Uint8 mouse) {};

		/**
		 * Called when an up event (mouse or keyboard) happends.
		 */
		virtual void handle_up(const SDL_Keycode key, const Uint8 mouse) {};

		/**
		 * Called when a mousewheel event happends.
		 */
		virtual void handle_wheel(const SDL_MouseWheelEvent &e) {};

		/**
		 * Get the desired window width of this state.
		 */
		virtual int get_prefered_width() const;

		/**
		 * Get the desired window height of this state.
		 */	
		virtual int get_prefered_height() const;

	protected:
		WindowState* window_state; 
};


/**
 * Game class containing a stack of states. Render, tick and events are passed onto the top state.
 * The state call to tick can signal adding new states or removing them.
 */
class StateGame : public Game {
	public:
		StateGame(State* state, const int w, const int h, const std::string& title);

	protected:

		/**
		 * Initializes the StateGame.
		 */
		virtual void init() override;

		/**
		 * Renders the current top state.
		 */
		virtual void render() override;

		/**
		 * Ticks the current top state, and potentialy changes to a new state.
		 * This is done if the top state signals it.
		 */
		virtual void tick(Uint64 delta) override;

		/**
		 * Sends a down-event to the top state with the relevant keycode.
		 */
		virtual void handle_keydown(SDL_KeyboardEvent &e) override;

		/**
		 * Sends an up-event to the top state with the relevant keycode.
		 */
		virtual void handle_keyup(SDL_KeyboardEvent &e) override;

		/**
		 * Send a down-event to the top state with the relevant mouse button.
		 */
		virtual void handle_mousedown(SDL_MouseButtonEvent &e) override;

		/**
		 * Sends an up-event to the top state with the relevant mouse button.
		 */
		virtual void handle_mouseup(SDL_MouseButtonEvent &e) override;

		/**
		 * Sends a mousewheel-event to the top state.
		 */
		virtual void handle_mousewheel(SDL_MouseWheelEvent &e) override;

	private:
	
		/**
		 * Syncs the screen size with the preferred sizes of state.
		 */
		void update_window(const State* const state);
		// State stack
		std::stack<std::unique_ptr<State>> states;
	
};


#endif //Include guard