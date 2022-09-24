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
		// Window dimensions
		const int initial_width = 100, initial_height = 100;
		
		// Mouse position, updated every frame.
		int mouseX, mouseY;
		
		Uint8 mouseButton;
		
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
		
		const std::string initial_title = "Title";
};

class State;

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
		
		virtual void handle_wheel(const SDL_MouseWheelEvent &e) {};

		const std::string& get_title() const;

		int get_width() const;

		int get_height() const;

	protected:
		std::string title;

		int window_width, window_height;

		int mouseX, mouseY;

		Uint8 mouseButton;
};

class StateGame : public Game {
	public:
		StateGame(State* state);

	protected:
		
		virtual void init() override;

		virtual void render() override;

		virtual void tick(Uint64 delta) override;

		virtual void handle_keydown(SDL_KeyboardEvent &e) override;

		virtual void handle_keyup(SDL_KeyboardEvent &e) override;

		virtual void handle_mousedown(SDL_MouseButtonEvent &e) override;
		
		virtual void handle_mouseup(SDL_MouseButtonEvent &e) override;
		
		virtual void handle_mousewheel(SDL_MouseWheelEvent &e) override;

		
	private:
		std::stack<std::unique_ptr<State>> states;
	
};


#endif //Include guard