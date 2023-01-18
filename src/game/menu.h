#ifndef MENU_00_H
#define MENU_00_H
#include <string>
#include "globals.h"
#include "engine/ui.h"
#include "file/json.h"
#include "level.h"

class MainMenu : public Menu {
	public:
		/**
		 * Constructs a MainMenu object.
		 */
		MainMenu() : Menu() {};

		/**
		 * Initializes the MainMenu, creating all buttons.
		 */
		void init(WindowState* window_state) override;

		/**
		 * Fixes the viewport.
		 */
		void resume() override;

	protected:

		void button_press(int btn) override;

	private:
		static const int BUTTON_WIDTH = 180, BUTTON_HEIGHT = 90;

		enum ButtonId {
			START_GAME, LEVEL_MAKER, OPTIONS, TOTAL
		};

		static const std::string BUTTON_NAMES[ButtonId::TOTAL];
};

class OptionsMenu : public Menu {
	public:
		OptionsMenu() : Menu() {};

		void init(WindowState* window_state) override;

	protected:

		static const int MARGIN_X = 40, MARGIN_Y = 30;
		static const int BUTTON_WIDTH = 100, BUTTON_HEIGHT = 50;

		void handle_down(SDL_Keycode key, Uint8 mouse) override;

		void handle_up(SDL_Keycode key, Uint8 mouse) override;

        void button_press(int btn) override;

        void render() override;

        void handle_wheel(const SDL_MouseWheelEvent &e) override;

        void menu_exit() override;

	private:
		bool waiting_for_input = false;

		TextBox input_promt;

		//(Key to grouping, key to binding)
		std::vector<std::pair<const std::string, std::string>> button_data;

		int last_input_button = 0;

		int btn = -1;

		int full_height = 0, camera_y = 0;

		void color_matching(int index, const std::string& cur, const std::string& old);

};

class LevelMakerStartup : public Menu {
	public:
		LevelMakerStartup() : Menu() {};

        void init(WindowState* window_state) override;

	protected:

        void button_press(int btn) override;

	private:
		enum ButtonId {
			START_LEVEL_MAKER, NEW_LEVEL, ADD_HEIGHT, SUB_HEIGHT, LEVELS_PREV, LEVELS_NEXT
		};

		void create_default_level();
		
		void create_levels_buttons(const JsonList& levels);

		int loaded = -1;
		
		int levels_button_start = 0;
		int levels_button_fits = 0;
		int levels_button_page = 0;
		
		std::shared_ptr<Texture> btn_texture;

		LevelData data;
};

#endif