#include "menu.h"
#include "util/exceptions.h"
#include "climbGame.h"
#include "levelMaker.h"
#include "config.h"
#include <iostream>
#include <memory>

const std::string MainMenu::BUTTON_NAMES[] = {"Start Game", "Level Maker", "Options"};

void MainMenu::init(WindowState* window_state) {
	SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
	SDL_SetWindowTitle(gWindow, "ClimbGame");
	SDL_ShowWindow(gWindow);
	State::init(window_state);
	SDL_GetRendererOutputSize(gRenderer, &window_state->screen_width, &window_state->screen_height);
	SDL_GetWindowSize(gWindow, &window_state->window_width, &window_state->window_height);

	buttons.reserve(ButtonId::TOTAL);
	for (int i = 0; i < ButtonId::TOTAL; ++i) {
		buttons.emplace_back(
			(window_state->screen_width - BUTTON_WIDTH) / 2,
			(i + 1) * window_state->screen_height / 4,
			BUTTON_WIDTH,
			BUTTON_HEIGHT,
			BUTTON_NAMES[i]
		);
	}
}

void MainMenu::resume() {
	SDL_RenderSetViewport(gRenderer, nullptr);
}

void MainMenu::button_press(const int btn) {
	switch (btn) {
		case START_GAME:
			next_res = {StateStatus::PUSH, new ClimbGame()};
			break;
		case LEVEL_MAKER: 
			next_res = {StateStatus::PUSH, new LevelMakerStartup()};
			break;
		case OPTIONS:
			next_res = {StateStatus::PUSH, new OptionsMenu()};
			break;
        default:
            break;
	}
}

void OptionsMenu::init(WindowState* ws) {
	Menu::init(ws);

	const JsonObject& bindings = config::get_bindings();

	camera_y = 0;
	int h_offset = MARGIN_Y;
	int group_start = 0;
	for (auto it = bindings.keys_begin(); it != bindings.keys_end(); ++it) {
		text.emplace_back((window_state->screen_width - 120) / 2, h_offset, 120, 60, *it, 25);
		const JsonObject& group = bindings.get<JsonObject>(*it);
		h_offset += 60 + MARGIN_Y;
		int index = 0;
		for (auto k = group.keys_begin(); k != group.keys_end(); ++k) {
			const std::string& key = *k;
			const std::string& val = group.get<std::string>(key);
			int x = 0;
			if (index % 2 == 0) {
				x = window_state->screen_width / 2 - BUTTON_WIDTH - MARGIN_X;
			} else {
				x = window_state->screen_width / 2 + MARGIN_X + MARGIN_X / 2 + BUTTON_WIDTH;
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
	buttons.emplace_back((window_state->screen_width - 120) / 2, h_offset, 120, 60, "Reset", 25);
	h_offset += 60 + MARGIN_Y;
	
	full_height = h_offset;
	input_promt = TextBox(0, 0, window_state->screen_width, window_state->screen_height, "Press any button: ", 50);
	waiting_for_input = false;
}

void OptionsMenu::handle_wheel(const SDL_MouseWheelEvent &e) {
	camera_y -= 25 * e.y;
	if (camera_y < 0 ) camera_y = 0;
	if (camera_y > full_height - window_state->screen_height) {
		camera_y = full_height - window_state->screen_height;
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

void OptionsMenu::button_press(const int new_btn) {
	if (new_btn > last_input_button) {
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
		btn = new_btn;
	}
}

void OptionsMenu::menu_exit() {
	config::write_options();
	Menu::menu_exit();
}

void OptionsMenu::render() {
	// A bit hacky... the game updates the mouse position after handling events, 
	// 	so mouseY will still be offset when potential clicks get handled.
	window_state->mouseY += camera_y; 
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	for (auto& b : buttons) {
		b.set_hover(!waiting_for_input && b.is_pressed(window_state->mouseX, window_state->mouseY));
		b.render(0, -camera_y);
	}
	for (auto& t : text) {
		t.render(0, -camera_y);
	}
	if (waiting_for_input) {
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x50);
		SDL_RenderFillRect(gRenderer, nullptr);
		input_promt.render(0, 0);
	}
	SDL_RenderPresent(gRenderer);
}

void LevelMakerStartup::init(WindowState* ws) {
	Menu::init(ws);

	create_default_level();

	buttons.emplace_back(3 * window_state->screen_width / 4 - 100, 3 * window_state->screen_height / 4 - 40, 200, 80, "Start", 30);
	buttons.emplace_back(window_state->screen_width / 3 - 110, 8 * window_state->screen_height / 12 - 20, 100, 40, "New Level", 15);
	buttons.emplace_back(window_state->screen_width / 3 - 50, 21 * window_state->screen_height / 48 - 20, 20, 20, "+", 15);
	buttons.emplace_back(window_state->screen_width / 3 - 80, 21 * window_state->screen_height / 48 - 20, 20, 20, "-", 15);

	text.emplace_back(
		window_state->screen_width / 4 - 100, 
		4 * window_state->screen_height / 8 - 40, 200, 80,
		"Tileset: default", 15
	);
	text.emplace_back(
		window_state->screen_width / 4 - 100,
		6 * window_state->screen_height / 16 - 40, 200, 80, 
		"Height: " + std::to_string(data.height), 15
	);
	text.emplace_back(
		window_state->screen_width / 4 - 100,
		5 * window_state->screen_height / 16 - 40,
		200, 80, "Data: clear", 15
	);
	
	btn_texture = std::make_shared<Texture>();
	btn_texture->load_from_file(config::get_asset_path("button.png"));
	const JsonList& levels = config::get_levels();
	levels_button_fits = (window_state->screen_width - 100) / 300;
	if (levels_button_fits < levels.size()) {
		buttons.emplace_back(50, 100, 25, 25, "<");
		buttons.emplace_back(window_state->screen_width - 75, 100, 25, 25, ">");
		levels_button_start = LEVELS_NEXT + 1;
	} else {
		levels_button_start = LEVELS_PREV;
	}
	levels_button_page = 0;
	create_levels_buttons(levels);
}

void LevelMakerStartup::create_levels_buttons(const JsonList& levels) {
	const int start_x = window_state->screen_width / 2 - (300 * levels_button_fits) / 2 + 25;
	for (int i = 0; i < levels_button_fits; ++i) {
		if (i + levels_button_fits * levels_button_page >= levels.size()) break;
		const std::string& text = levels.get<JsonObject>(i + levels_button_fits * levels_button_page).get<std::string>("name");
		if (buttons.size() > i + levels_button_start) {
			buttons[i + levels_button_start].set_text(text);
		} else {
			buttons.emplace_back(start_x + i * 300, 50, 250, 100, text, btn_texture);
		}
	}
	while (buttons.size() - levels_button_start > levels.size() - levels_button_page * levels_button_fits) {
		buttons.pop_back();
	}
}

void LevelMakerStartup::create_default_level() {
	loaded = -1;
	data.height = TILE_HEIGHT * 2;
	data.width = TILE_WIDTH;
}

void LevelMakerStartup::button_press(const int btn) {
	switch (btn) {
		case START_LEVEL_MAKER: {
			if (loaded == -1) {
				data.data = std::make_unique<Uint32[]>(data.width * data.height);
				for (size_t i = 0; i < data.width * data.height; ++i) {
					data.data[i] = (0xFF << 8);
				}
			}
			const JsonObject& lvl_config = loaded == -1 ? config::get_level_config("default") : 
				config::get_level_config(config::get_level(loaded).get<std::string>("config"));
			next_res = {
				StateStatus::SWAP, 
				new LevelMaker(
					std::move(data), 
					LevelConfig::load_from_json(lvl_config)
				)
			};
			break;
		}
		case NEW_LEVEL: 
			create_default_level();
			text[1].set_text("Height: " + std::to_string(data.height));
			text[2].set_text("Data: clear");
			break; 
		case ADD_HEIGHT:
			if (loaded == -1) {
				data.height += TILE_HEIGHT;
				text[1].set_text("Height: " + std::to_string(data.height));
			}
			break;
		case SUB_HEIGHT:
			if (loaded == -1 && data.height > TILE_HEIGHT) {
				data.height -= TILE_HEIGHT;
				text[1].set_text("Height: " + std::to_string(data.height));
			}
			break;
		default:
			if (btn >= levels_button_start) {
				loaded = btn + levels_button_page * levels_button_fits - levels_button_start;
				const JsonObject& lvl = config::get_level(loaded);
				const JsonObject& conf = config::get_level_config(lvl.get<std::string>("config"));
				try {
					LevelData temp;
					temp.load_from_file(
						config::get_level_path(lvl.get<std::string>("file")), 
						conf.get<int>("tile_count")
					);
					data = std::move(temp);
				} catch (const base_exception& e) {
					std::cout << e.msg << std::endl;
					loaded = -1;
				}
				text[1].set_text("Height: " + std::to_string(data.height));
				text[2].set_text(loaded == -1 ? "Data: clear" : "Data: " + lvl.get<std::string>("name"));
			} else if (btn == LEVELS_PREV) {
				if (levels_button_page > 0) { 
					--levels_button_page;
					const JsonList& lvls = config::get_levels();
					create_levels_buttons(lvls);
				}
			} else if (btn == LEVELS_NEXT) {
				const JsonList& lvls = config::get_levels();
				if ((levels_button_page + 1) * levels_button_fits < lvls.size()) {
					++levels_button_page;
					create_levels_buttons(lvls);
				}
			}
	}
}