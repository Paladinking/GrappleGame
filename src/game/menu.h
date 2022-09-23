#ifndef MENU_00_H
#define MENU_00_H
#include <string>
#include "globals.h"
#include "engine/ui.h"

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

#endif