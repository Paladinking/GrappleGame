#ifndef MENU_00_H
#define MENU_00_H
#include <SDL_ttf.h>
#include <vector>
#include "state.h"

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
		static void init();

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

class MainMenu : public State {
	public:
		/**
		 * Constructs a MainMenu object.
		 */
		MainMenu() : State(SCREEN_WIDTH, SCREEN_HEIGHT, "Climbgame") {};

		/**
		 * Initializes the MainMenu, creating all buttons.
		 */
		virtual void init() override;

		/**
		 * Handles a down-event of keyboard or mouse.
		 */
		virtual void handle_down(const SDL_Keycode key, const Uint8 mouse) override;

		/**
		 * Handles an up-event of keyboard or mouse.
		 */
		virtual void handle_up(const SDL_Keycode key, const Uint8 mouse) override;

		/**
		 * Ticks the menu, deciding if to switch state.
		 */
		virtual void tick(const Uint64 delta, StateStatus& res) override;

		/**
		 * Renders the full menu.
		 */
		virtual void render() override;

	private:
		static const int BUTTON_WIDTH = 180, BUTTON_HEIGHT = 90;

		enum ButtonId {
			START_GAME, LEVEL_MAKER, OPTIONS, TOTAL, NONE
		} targeted_button = ButtonId::NONE;

		static const std::string BUTTON_NAMES[ButtonId::TOTAL];

		State* next_state = nullptr;

		std::vector<Button> buttons;

		bool exit;
};

#endif