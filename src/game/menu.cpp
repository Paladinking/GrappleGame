#include "menu.h"
#include "util/exceptions.h"
#include "climbGame.h"
#include "levelMaker.h"
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
	}
}
