#ifndef MENU_00_H
#define MENU_00_H
#include <SDL_ttf.h>
#include <vector>
#include "state.h"

class Button {
	public:
		Button() {};

		Button(const int x, const int y, const int w, const int h, const std::string& text);

		bool is_pressed(const int mouseX, const int mouseY) const;

		void set_text(const std::string& text);

		void set_position(const int x, const int y);

		void set_dimensions(const int w, const int h);

		void render(const int mouseX, const int mouseY);
		
		static void init();
	private:
		int x, y, w, h;
		
		int text_offset_x, text_offset_y;

		Texture texture;

		std::string text;

		void generate_texture();
		
		static TTF_Font* font;
};

class MainMenu : public State {
	public:
		MainMenu() : State(SCREEN_WIDTH, SCREEN_HEIGHT, "Climbgame") {};

		virtual void init() override;

		virtual void handle_down(const SDL_Keycode key, const Uint8 mouse) override;

		virtual void handle_up(const SDL_Keycode key, const Uint8 mouse) override;

		virtual void tick(const Uint64 delta, StateStatus& res) override;

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