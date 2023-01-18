#ifndef UI_00_H
#define UI_00_H
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <memory>
#include "util/exceptions.h"
#include "game.h"
#include "input.h"

class TextBox {
	public:
		TextBox() = default;

		TextBox(int x, int y, int w, int h, const std::string& text);

		TextBox(int x, int y, int w, int h, std::string  text, int font_size);

		/**
		 * Sets the text of the textbox.
		 */
		void set_text(const std::string& text);
		
		
		/**
		 * Sets the font size of the textbox.
		 */
		void set_font_size(int font_size);

		/**
		 * Gets the text of the textbox.
		 */
		const std::string& get_text();

		/**
		 * Sets the position of the textbox (upper corner) to (x, y).
		 */
		void set_position(int x, int y);

		/**
		 * Sets the dimensions of the textbox to (w, h).
		 */
		void set_dimensions(int w, int h);
		
		/**
		 * Sets the color of the text.
		 */
		void set_text_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

		/**
		 * Gets the color of the text;
		 */
		[[nodiscard]] const SDL_Color& get_text_color() const;

		/**
		 * Renders the textbox.
		 */
		virtual void render(int x_offset, int y_offset);

		/**
		 * Initializes the button class, loading the font used for the button text.
		 */
		static void init(SDL_RWops* font_data);

	protected:
		int x{}, y{}, w{}, h{};
		
		int font_size{};

		int text_offset_x{};

		int text_offset_y{};

		std::string text;

	private:
		Texture texture;
		
		SDL_Color color = {0, 0, 0, 0};

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
		Button() = default;

		/**
		 * Constructs a button with given size and text positioned at given location.
		 */
		Button(const int x, const int y, const int w, const int h, const std::string& text) : TextBox(x, y, w, h, text){};
		Button(const int x, const int y, const int w, const int h, const std::string& text, const int font_size) : TextBox(x, y, w, h, text, font_size){};
		Button(const int x, const int y, const int w, const int h, const std::string& text, const int font_size, const std::shared_ptr<Texture>& background) : TextBox(x, y, w, h, text, font_size), background(background) {};
		Button(const int x, const int y, const int w, const int h, const std::string& text, const std::shared_ptr<Texture>& background) : TextBox(x, y, w, h, text), background(background) {};

		/**
		 * Returns true if the button contains the point (mouseX, mousey).
		 */
		[[nodiscard]] bool is_pressed(int mouseX, int mouseY) const;

		/**
		 * Sets the hover state of this button.
		 */
		void set_hover(bool hover);

		/**
		 * Renders the button.
		 */
		void render(int x_offset, int y_offset) override;

	private:
		bool hover = false;
		
		std::shared_ptr<Texture> background;
};

class Menu : public State {
	public:	
		Menu();

		explicit Menu(const std::string& exit_input);

		/**
		 * Handles a down-event of keyboard or mouse.
		 */
		void handle_down(SDL_Keycode key, Uint8 mouse) override;

		/**
		 * Handles an up-event of keyboard or mouse.
		 */
		void handle_up(SDL_Keycode key, Uint8 mouse) override;

		/**
		 * Renders the full menu.
		 */
		void render() override;

		/**
		 * Ticks the menu, deciding if to switch state.
		 */
		void tick(Uint64 delta, StateStatus& res) override;

	protected:
		std::vector<Button> buttons;
		std::vector<TextBox> text;


		// Set by subclasses to swap state
		StateStatus next_res;

		/**
		 * Called when a button is pressed.
	     * The int btn will contain the index of the button in the buttons vector.
		 */
		virtual void button_press(int btn) = 0;

		/**
		 * Called when the Menu_exit input is recieved (Typicly Escape).
		 * This function allows most menu subclasses not to override handle_down or handle_up.
		 */
		virtual void menu_exit();

	private:
		int targeted_button = 0;

		std::unique_ptr<PressInput> exit_input;
};

#endif