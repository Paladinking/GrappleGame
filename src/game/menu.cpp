#include "menu.h"
#include "util/exceptions.h"
#include "climbGame.h"
#include "levelMaker.h"
#include "config.h"
#include "nativefiledialog/nfdcpp.h"
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
			next_state = new LevelMakerStartup();
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
		text.emplace_back((window_width - 120) / 2, h_offset, 120, 60, *it, 25);
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
			button_data.emplace_back(*it, key);
			text.emplace_back(x - BUTTON_WIDTH - MARGIN_X / 2, h_offset, BUTTON_WIDTH, BUTTON_HEIGHT, key + ":", 15);
			if (index % 2 == 1) {
				h_offset += BUTTON_HEIGHT + MARGIN_Y;
			}
			bool duplicate_binding = false;
			for (int i = group_start; i < index + group_start; ++i) {
				if (buttons[i].get_text() == buttons[index + group_start].get_text()) {
					
					duplicate_binding = true;
					if (buttons[i].get_text_color().r != 0xFF) {
						buttons[i].set_text_color(0xFF, 0x00, 0x00, 0xFF);
					}
				}
			}
			if (duplicate_binding) buttons[index + group_start].set_text_color(0xFF, 0x00, 0x00, 0xFF);
			++index;
		}
		if (index % 2 == 1) {
			h_offset += BUTTON_HEIGHT + MARGIN_Y;
		}
		group_start += index;
	}
	
	last_input_button = static_cast<int>(buttons.size()) - 1;
	buttons.emplace_back((window_width - 120) / 2, h_offset, 120, 60, "Reset", 25);
	h_offset += 60 + MARGIN_Y;
	
	full_height = h_offset;
	input_promt = TextBox(0, 0, window_width, window_height, "Press any button: ", 50);
	waiting_for_input = false;
}

void OptionsMenu::handle_wheel(const SDL_MouseWheelEvent &e) {
	camera_y -= 25 * e.y;
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
		JsonObject& bindings = config::get_bindings(button_data[btn].first);

		bindings.set<std::string>(button_data[btn].second, new_input);
		buttons[btn].set_text(new_input);
		color_matching(btn, new_input, old_input);
	} else {
		Menu::handle_up(key, mouse);
	}
}

void OptionsMenu::color_matching(const int index, const std::string& cur, const std::string& old) {
	bool taken_input = false;
	bool single_old = false;
	Button* old_match = nullptr;
	for (int i = 0; i <= last_input_button; ++i) {
		if (i == index || button_data[index].first != button_data[i].first) continue;
		if (buttons[i].get_text() == old) {
			if (old_match == nullptr) {
				old_match = &buttons[i];
				single_old = true;
			} else {
				single_old = false;
			}
		}
		if (buttons[i].get_text() == cur) {
			if (buttons[i].get_text_color().r == 0x00) {
				buttons[i].set_text_color(0xFF, 0x00, 0x00, 0xFF);
			}
			taken_input = true;
		}
	}
	if (single_old) {
		old_match->set_text_color(0x00, 0x00, 0x00, 0xFF);
	}
	if (taken_input) {
		if (buttons[index].get_text_color().r == 0x00) {
			buttons[index].set_text_color(0xFF, 0x00, 0x00, 0xFF);
		}
	} else if (buttons[index].get_text_color().r == 0xFF) {
		buttons[index].set_text_color(0x00, 0x00, 0x00, 0xFF);
	}
}

void OptionsMenu::button_press(const int btn) {
	if (btn > last_input_button) {
		config::reset_bindings();
		const JsonObject& binds = config::get_bindings();
		for (int i = 0; i < last_input_button; ++i) { 
			buttons[i].set_text(binds.get<JsonObject>(button_data[i].first).get<std::string>(button_data[i].second));
			if (buttons[i].get_text_color().r == 0xFF) {// Default bindings have no duplicates
				buttons[i].set_text_color(0x00, 0x00, 0x00, 0xFF);
			}
		}
	} else {
		waiting_for_input = true;
		this->btn = btn;
	}
}

void OptionsMenu::menu_exit() {
	config::write_options();
	Menu::menu_exit();
}

void OptionsMenu::render() {
	// A bit hacky... the game updates the mouse position after handling events, 
	// 	so mouseY will still be offset when potential clicks get handled.
	mouseY += camera_y; 
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	for (auto& b : buttons) {
		b.set_hover(!waiting_for_input && b.is_pressed(mouseX, mouseY));
		b.render(0, -camera_y);
	}
	for (auto& t : text) {
		t.render(0, -camera_y);
	}
	if (waiting_for_input) {
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x50);
		SDL_RenderFillRect(gRenderer, NULL);
		input_promt.render(0, 0);
	}
	SDL_RenderPresent(gRenderer);
}

void LevelMakerStartup::init() {
	Menu::init();

	create_default_level();

	buttons.emplace_back(3 * window_width / 4 - 100, 3 * window_height / 4 - 40, 200, 80, "Start", 30);
	buttons.emplace_back(window_width / 3 - 110, 8 * window_height / 12 - 20, 100, 40, "New Level", 15);
	buttons.emplace_back(window_width / 3 - 110, 9 * window_height / 12 - 20, 100, 40, "Load Level", 15);
	buttons.emplace_back(window_width / 3 - 50, 21 * window_height / 48 - 20, 20, 20, "+", 15);
	buttons.emplace_back(window_width / 3 - 80, 21 * window_height / 48 - 20, 20, 20, "-", 15);

	text.emplace_back(window_width / 4 - 100, 4 * window_height / 8 - 40, 200, 80, "Tileset: " + tileset_path, 15);
	text.emplace_back(window_width / 4 - 100, 6 * window_height / 16 - 40, 200, 80, "Height: " + std::to_string(data.height), 15);
	text.emplace_back(window_width / 4 - 100, 5 * window_height / 16 - 40, 200, 80, "Data: clear", 15);
}

void LevelMakerStartup::create_default_level() {
	loaded = false;
	tileset_path = config::get_default_tileset();
	data.height = TILE_HEIGHT * 2;
	data.width = TILE_WIDTH;
	data.img_tilesize = 32;
	data.img_tilewidth = 10;
	data.img_tilecount = 96;
}

void LevelMakerStartup::button_press(const int btn) {
	switch (btn) {
		case START_LEVEL_MAKER:
			if (!loaded) {
				data.data = std::make_unique<Uint16[]>(data.width * data.height);
				for (size_t i = 0; i < data.width * data.height; ++i) {
					data.data[i] = (0xFF << 8);
				}
			}
			next_state = new LevelMaker(std::move(data), config::get_asset_path(tileset_path));
			break;
		case NEW_LEVEL: 
			create_default_level();
			text[1].set_text("Height: " + std::to_string(data.height));
			text[2].set_text("Data: clear");
			break;
		case LOAD_FILE: {
			std::string file_path;
			if (nfd::OpenDialog(file_path) == NFD_OKAY) {
				try {
					LevelData temp;
					temp.load_from_file(file_path);
					data = std::move(temp);
					loaded = true;
				} catch (const base_exception& e) {
					std::cout << e.msg << std::endl;
				}
				text[1].set_text("Height: " + std::to_string(data.height));
				text[2].set_text(loaded ? "Data: loaded" : "Data: clear");
			}
			break;
		}
		case ADD_HEIGHT:
			if (!loaded) {
				data.height += TILE_HEIGHT;
				data.data = std::make_unique<Uint16[]>(data.width * data.height);
				text[1].set_text("Height: " + std::to_string(data.height));
			}
			break;
		case SUB_HEIGHT:
			if (!loaded && data.height > TILE_HEIGHT) {
				data.height -= TILE_HEIGHT;
				data.data = std::make_unique<Uint16[]>(data.width * data.height);
				text[1].set_text("Height: " + std::to_string(data.height));
			}
			break;
	}
}