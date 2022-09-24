#ifndef MENU_00_H
#define MENU_00_H
#include <string>
#include "globals.h"
#include "engine/ui.h"
#include "file/json.h"

class MainMenu : public Menu {
	public:
		/**
		 * Constructs a MainMenu object.
		 */
		MainMenu() : Menu(SCREEN_WIDTH, SCREEN_HEIGHT, "Climbgame") {};

		/**
		 * Initializes the MainMenu, creating all buttons.
		 */
		virtual void init() override;

	protected:

		virtual void button_press(const int btn) override;

	private:
		static const int BUTTON_WIDTH = 180, BUTTON_HEIGHT = 90;

		enum ButtonId {
			START_GAME, LEVEL_MAKER, OPTIONS, TOTAL
		};

		static const std::string BUTTON_NAMES[ButtonId::TOTAL];
};

class OptionsMenu : public Menu {
	public:
		OptionsMenu() : Menu(-1, -1, "") {};
	
		virtual void init() override;

	protected:
		
		static const int MARGIN_X = 50, MARGIN_Y = 30;
		static const int BUTTON_WIDTH = 120, BUTTON_HEIGHT = 50;

		virtual void handle_down(const SDL_Keycode key, const Uint8 mouse) override;
		
		virtual void button_press(const int btn) override;

};

#endif