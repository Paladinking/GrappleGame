#include "menu.h"
#include "util/exceptions.h"
#include "climbGame.h"
#include "levelMaker.h"
#include <iostream>

TTF_Font* Button::font;

void Button::init() {
	Button::font = TTF_OpenFont((ASSETS_ROOT + "font/OpenSans-Bold.ttf").c_str(), 20); //TODO make path dynamic
	if (Button::font == NULL) {
		throw game_exception(std::string(TTF_GetError()));
	}
}

Button::Button(
	const int x, const int y, const int w, const int h, const std::string& text) : x(x), y(y), w(w), h(h), text(text), texture(nullptr, 0, 0) {
	generate_texture();
}

bool Button::is_pressed(const int mouseX, const int mouseY) const {
	return mouseX >= x && mouseX < x + w && mouseY >= y && mouseY < y + h;
}

void Button::generate_texture() {
	SDL_Color text_color = {0, 0, 0, 0};
	SDL_Surface* text_surface = TTF_RenderUTF8_Solid(font, text.c_str(), text_color);
	if (text_surface == NULL) {
		throw image_load_exception(std::string(TTF_GetError()));
	}
	int width = text_surface->w;
	int height = text_surface->h;
	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(gRenderer, text_surface);
	SDL_FreeSurface(text_surface);
	
	if (text_texture == NULL) {
		throw image_load_exception(std::string(SDL_GetError()));
	}
	texture = Texture(text_texture, width, height);

	TTF_SizeUTF8(font, text.c_str(), &width, &height);
	text_offset_x = (w - width) / 2;
	text_offset_y = (h - height) / 2;
}

void Button::set_position(const int x, const int y) {
	this->x = x;
	this->y = y;
}

void Button::set_text(const std::string& text) {
	this->text = text;
	generate_texture();
}

void Button::render(const int mouseX, const int mouseY) {
	if (is_pressed(mouseX, mouseY)) {
		SDL_SetRenderDrawColor(gRenderer, 200, 200, 240, 0xFF);
	} else {
		SDL_SetRenderDrawColor(gRenderer, 100, 100, 220, 0xFF);
	}
	SDL_Rect r = {x, y, w, h};
	if (SDL_RenderFillRect(gRenderer, &r) != 0) {
		std::cout << SDL_GetError() << std::endl;
	}
	texture.render(x + text_offset_x, y + text_offset_y);
}

void Button::set_dimensions(const int w, const int h) {
	text_offset_x = (w - this->w) / 2 + text_offset_x;
	text_offset_y = (h - this->h) / 2 + text_offset_y;
	this->w = w;
	this->h = h;
}

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

void MainMenu::tick(const Uint64 delta, StateStatus& res) {
	if (exit) {
		res.action = StateStatus::POP;
	} else if (next_state != nullptr) {
		res.new_state = next_state;
		res.action = StateStatus::PUSH;
		next_state = nullptr;
	}
}

void MainMenu::handle_down(const SDL_Keycode key, const Uint8 mouse) {
	if (mouse == SDL_BUTTON_LEFT) {
		targeted_button = ButtonId::NONE;
		for (int i = 0; i < buttons.size(); ++i) {
			if (buttons[i].is_pressed(mouseX, mouseY)) {
				targeted_button = static_cast<ButtonId>(i);
				break;
			}
		}
	} else if (key == SDLK_ESCAPE) {
		exit = true;
	}
}

void MainMenu::handle_up(const SDL_Keycode key, const Uint8 mouse) {
	if (mouse == SDL_BUTTON_LEFT) {
		if (targeted_button != ButtonId::NONE && buttons[targeted_button].is_pressed(mouseX, mouseY)) {
			switch (targeted_button) {
				case ButtonId::START_GAME:
					next_state = new ClimbGame();
					break;
				case ButtonId::LEVEL_MAKER:
					next_state = new LevelMaker();
					break;
			}
		}
	}
}

void MainMenu::render() {
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	if (0 != SDL_RenderClear(gRenderer)) {
		std::cout << SDL_GetError() << std::endl;
	}
	for (auto& b : buttons) {
		b.render(mouseX, mouseY);
	}
	SDL_RenderPresent(gRenderer);
}

