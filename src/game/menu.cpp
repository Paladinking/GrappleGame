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
	const JsonObject& bindings = config::get_bindings();

	camera_y = 0;
	int h_offset = MARGIN_Y;
	int group_start = 0;
	for (auto& it = bindings.keys_begin(); it != bindings.keys_end(); ++it) {
		text.emplace_back((window_width - 60) / 2, h_offset, 120, 60, *it, 25);
		const JsonObject& group = bindings.get<JsonObject>(*it);
		h_offset += 60 + MARGIN_Y;
		int index = 0;
		for (auto& k = group.keys_begin(); k != group.keys_end(); ++k) {
			const std::string& key = *k;
			const std::string& val = group.get<std::string>(key);
			int x = 0;
			if (index % 2 == 0) {
				x = window_width / 2 - BUTTON_WIDTH - MARGIN_X;
			} else {
				x = window_width / 2 + MARGIN_X + MARGIN_X / 2 + BUTTON_WIDTH;
			}
			buttons.emplace_back(x, h_offset, BUTTON_WIDTH, BUTTON_HEIGHT, val, 15);
			button_data.emplace_back(&(*it), key);
			text.emplace_back(x - BUTTON_WIDTH - MARGIN_X / 2, h_offset, BUTTON_WIDTH, BUTTON_HEIGHT, key + ":", 15);
			if (index % 2 == 1) {
				h_offset += BUTTON_HEIGHT + MARGIN_Y;
			}
			bool duplicate_binding = false;
			for (int i = group_start; i < index; ++i) {
				if (buttons[i].get_text() == buttons[index].get_text()) {
					duplicate_binding = true;
					if (buttons[i].get_text_color().r != 0xFF) {
						buttons[i].set_text_color(0xFF, 0x00, 0x00, 0xFF);
					}
				}
			}
			if (duplicate_binding) buttons[index].set_text_color(0xFF, 0x00, 0x00, 0xFF);
			++index;
		}
		if (index % 2 == 1) {
			h_offset += BUTTON_HEIGHT + MARGIN_Y;
		}
		group_start += index;
	}
	full_height = h_offset;

	input_promt = TextBox(0, 0, window_width, window_height, "Press any button: ", 50);
	waiting_for_input = false;
}

void OptionsMenu::handle_wheel(const SDL_MouseWheelEvent &e) {
	camera_y += e.y;
	if (camera_y < 0 ) camera_y = 0;
	if (camera_y > full_height - window_height) {
		camera_y = full_height - window_height;
	}
}

void OptionsMenu::handle_down(const SDL_Keycode key, const Uint8 mouse) {
	if (!waiting_for_input) {
		Menu::handle_down(key, mouse);
	}
}

void OptionsMenu::handle_up(const SDL_Keycode key,const Uint8 mouse) {
	if (waiting_for_input) {
		waiting_for_input = false;
		
		std::string old_input = buttons[btn].get_text();
		std::string new_input = get_input_name(key, mouse);
		JsonObject& bindings = config::get_bindings(*(button_data[btn].first));

		bindings.set<std::string>(button_data[btn].second, new_input);
		buttons[btn].set_text(new_input);
		color_matching(0, btn, new_input, old_input);
	} else {
		Menu::handle_up(key, mouse);
	}
}

void OptionsMenu::color_matching(const int start, const int index, const std::string& cur, const std::string& old) {
	bool taken_input = false;
	for (int i = start; i < buttons.size(); ++i) {
		if (i == index || button_data[index].first != button_data[i].first) continue;
		if (buttons[i].get_text() == old && buttons[i].get_text_color().r == 0xFF) {
			buttons[i].set_text_color(0x00, 0x00, 0x00, 0xFF);
		} else if (buttons[i].get_text() == cur && buttons[i].get_text_color().r == 0x00) {
			buttons[i].set_text_color(0xFF, 0x00, 0x00, 0xFF);
			taken_input = true;
		}
	}
	if (taken_input && buttons[index].get_text_color().r == 0x00) {
		buttons[index].set_text_color(0xFF, 0x00, 0x00, 0xFF);
	} else if (buttons[index].get_text_color().r == 0xFF) {
		buttons[index].set_text_color(0x00, 0x00, 0x00, 0xFF);
	}
}

void OptionsMenu::button_press(const int btn) {
	waiting_for_input = true;
	this->btn = btn;
}

void OptionsMenu::render() {
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);
	int mx = waiting_for_input ? -1 : mouseX;
	for (auto& b : buttons) {
		b.render(mx, mouseY);
	}
	for (auto& t : text) {
		t.render();
	}
	if (waiting_for_input) {
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x50);
		SDL_RenderFillRect(gRenderer, NULL);
		input_promt.render();
	}
	SDL_RenderPresent(gRenderer);
}
