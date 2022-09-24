#ifndef UI_00_H
#define UI_00_H
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "util/exceptions.h"
#include "game.h"
#include "input.h"

class TextBox {
	public:
		TextBox() {};

		TextBox(const int x, const int y, const int w, const int h, const std::string& text);

		/**
		 * Sets the text of the textbox.
		 */
		void set_text(const std::string& text);

		/**
		 * Gets the text of the textbox.
		 */
		const std::string& get_text();

		/**
		 * Sets the position of the button (upper corner) to (x, y).
		 */
		void set_position(const int x, const int y);

		/**
		 * Sets the dimensions of the button to (w, h).
		 */
		void set_dimensions(const int w, const int h);

		/**
		 * Renders the textbox.
		 */
		virtual void render();

		/**
		 * Initializes the button class, loading the font used for the button text.
		 */
		static void init(const std::string& font_path);

	protected:
		int x, y, w, h;

		int text_offset_x;

		int text_offset_y;

		std::string text;

	private:
		Texture texture;

		/**
		 * Generates the texture containing the text of the button. Called by constuctor and set_text.
		 */
		void generate_texture();

		static TTF_Font* font;
};

class Button : public TextBox {
	public:
		/**
		 * Default initialization
		 */
		Button() {};

		/**
		 * Constructs a button with given size and text positioned at given location.
		 */
		Button(const int x, const int y, const int w, const int h, const std::string& text) : TextBox(x, y, w, h, text){};

		/**
		 * Returns true if the button contains the point (mouseX, mousey).
		 */
		bool is_pressed(const int mouseX, const int mouseY) const;

		/**
		 * Renders the button, differently if the mouse is over it.
		 */
		virtual void render(const int mouseX, const int mouseY);

		/**
		 * Renders the button.
		 */
		virtual void render() override;
};

class Menu : public State {
	public:	
		Menu(const int w, const int h, const std::string& title);
		
		Menu(const int w, const int h, const std::string& title, const std::string& exit_input);
		/**
		 * Handles a down-event of keyboard or mouse.
		 */
		virtual void handle_down(const SDL_Keycode key, const Uint8 mouse) override;

		/**
		 * Handles an up-event of keyboard or mouse.
		 */
		virtual void handle_up(const SDL_Keycode key, const Uint8 mouse) override;

		/**
		 * Renders the full menu.
		 */
		virtual void render() override;

		/**
		 * Ticks the menu, deciding if to switch state.
		 */
		virtual void tick(const Uint64 delta, StateStatus& res) override;

	protected:
		std::vector<Button> buttons;
		std::vector<TextBox> text;
		


		// Set by subclasses to swap state
		State* next_state = nullptr;	
		bool exit = false;

		/**
		 * Called when a button is pressed.
	     * The int btn will contain the index of the button in the buttons vector.
		 */
		virtual void button_press(const int btn) = 0;

		/**
		 * Called when the Menu_exit input is recieved (Typicly Escape).
		 * This function allows most menu subclasses not to override handle_down or handle_up.
		 */
		virtual void menu_exit();

	private:
		int targeted_button;

		std::unique_ptr<PressInput> exit_input;
};

#endif