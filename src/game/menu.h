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
		virtual void init(WindowState* window_state) override;

		/**
		 * Fixes the viewport.
		 */
		virtual void resume() override;

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
		OptionsMenu() : Menu() {};

		virtual void init(WindowState* window_state) override;

	protected:

		static const int MARGIN_X = 40, MARGIN_Y = 30;
		static const int BUTTON_WIDTH = 100, BUTTON_HEIGHT = 50;

		virtual void handle_down(const SDL_Keycode key, const Uint8 mouse) override;

		virtual void handle_up(const SDL_Keycode key,const Uint8 mouse) override;

		virtual void button_press(const int btn) override;

		virtual void render() override;

		virtual void handle_wheel(const SDL_MouseWheelEvent &e) override;

		virtual void menu_exit() override;

	private:
		bool waiting_for_input;

		TextBox input_promt;

		//(Key to grouping, key to binding)
		std::vector<std::pair<const std::string, std::string>> button_data;

		int last_input_button;

		int btn;

		int full_height, camera_y;

		void color_matching(const int index, const std::string& cur, const std::string& old);

};

class LevelMakerStartup : public Menu {
	public:
		LevelMakerStartup() : Menu() {};

		virtual void init(WindowState* window_state) override;

	protected:

		virtual void button_press(const int btn) override;

	private:
		enum ButtonId {
			START_LEVEL_MAKER, NEW_LEVEL, ADD_HEIGHT, SUB_HEIGHT, LEVELS_PREV, LEVELS_NEXT
		};

		void create_default_level();
		
		void create_levels_buttons(const JsonList& levels);

		void sync_text();

		int loaded = -1;
		
		int levels_button_start;
		int levels_button_fits;
		int levels_button_page;

		std::string tileset_path;
		
		std::shared_ptr<Texture> btn_texture;

		LevelData data;
};

#endif