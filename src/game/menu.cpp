#include "menu.h"
#include "util/exceptions.h"
#include "climbGame.h"
#include "levelMaker.h"
#include "config.h"
#include <iostream>

const std::string MainMenu::BUTTON_NAMES[] = {"Start Game", "Level Maker", "Options"};

void MainMenu::init() {
	State::init();
	buttons.reserve(ButtonId::TOTAL);
	for (int i = 0; i < ButtonId::TOTAL; ++i) {
		buttons.emplace_back(
			(window_width - BUTTON_WIDTH) / 2,
			(i + 1) * window_height / 4,
			BUTTON_WIDTH,
			BUTTON_HEIGHT,
			BUTTON_NAMES[i]
		);
	}
	exit = false;
}

void MainMenu::button_press(const int btn) {
	switch (btn) {
		case START_GAME:
			next_state = new ClimbGame();
			break;
		case LEVEL_MAKER:
			next_state = new LevelMaker();
			break;
		case OPTIONS:
			next_state = new OptionsMenu();
			break;
	}
}

void OptionsMenu::init() {
	Menu::init();
	const JsonObject& bindings = config::get_options().get<JsonObject>(bindings::KEY_NAME);
	int total_height = MARGIN_Y;
	int index = 0;
	for (auto& binding : bindings) {
		const std::string& key = binding.first;
		const std::string& val = std::get<std::string>(binding.second);
		int x = ((index % 2) == 0) ? window_width / 2 - BUTTON_WIDTH - MARGIN_X : window_width / 2 + MARGIN_X;
		int y = (index / 2) * (BUTTON_HEIGHT + MARGIN_Y) + MARGIN_Y;
		buttons.emplace_back(x, y, BUTTON_WIDTH, BUTTON_HEIGHT, val);
		++index;
	}
}

void OptionsMenu::handle_down(const SDL_Keycode key, const Uint8 mouse) {
	Menu::handle_down(key, mouse);
	
}

void OptionsMenu::button_press(const int btn) {
	
}
