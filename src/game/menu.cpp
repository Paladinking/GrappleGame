#include "menu.h"
#include "util/exceptions.h"
#include "climbGame.h"
#include "levelMaker.h"
#include <iostream>

TTF_Font* TextBox::font;

void TextBox::init() {
	Button::font = TTF_OpenFont((ASSETS_ROOT + "font/OpenSans-Bold.ttf").c_str(), 20); //TODO make path dynamic
	if (Button::font == NULL) {
		throw game_exception(std::string(TTF_GetError()));
	}
}

TextBox::TextBox(
	const int x, const int y, const int w, const int h, const std::string& text) : x(x), y(y), w(w), h(h), text(text), texture(nullptr, 0, 0) {
	generate_texture();
}


void TextBox::generate_texture() {
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

void TextBox::set_position(const int x, const int y) {
	this->x = x;
	this->y = y;
}

void TextBox::set_text(const std::string& text) {
	this->text = text;
	generate_texture();
}

const std::string& TextBox::get_text() {
	return text;
}


void TextBox::set_dimensions(const int w, const int h) {
	text_offset_x = (w - this->w) / 2 + text_offset_x;
	text_offset_y = (h - this->h) / 2 + text_offset_y;
	this->w = w;
	this->h = h;
}

void TextBox::render() {
	texture.render(x + text_offset_x, y + text_offset_x);
}

bool Button::is_pressed(const int mouseX, const int mouseY) const {
	return mouseX >= x && mouseX < x + w && mouseY >= y && mouseY < y + h;
}

void Button::render(const int mouseX, const int mouseY) {
	if (is_pressed(mouseX, mouseY)) {
		SDL_SetRenderDrawColor(gRenderer, 200, 200, 240, 0xFF);
	} else {
		SDL_SetRenderDrawColor(gRenderer, 100, 100, 220, 0xFF);
	}
	SDL_Rect r = {x, y, w, h};
	SDL_RenderFillRect(gRenderer, &r);
	TextBox::render();
}

void Button::render() {
	render(x - 1, y - 1);
}

void Menu::handle_down(const SDL_Keycode key, const Uint8 mouse) {
	if (mouse == SDL_BUTTON_LEFT) {
		targeted_button = -1;
		for (int i = 0; i < buttons.size(); ++i) {
			if (buttons[i].is_pressed(mouseX, mouseY)) {
				targeted_button = i;
				break;
			}
		}
	}
}

void Menu::handle_up(const SDL_Keycode key, const Uint8 mouse) {
	if (mouse == SDL_BUTTON_LEFT) {
		if (targeted_button >= 0 && buttons[targeted_button].is_pressed(mouseX, mouseY)) {
			button_press(targeted_button);
		}
	}
}

void Menu::render() {
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	for (auto& b : buttons) {
		b.render(mouseX, mouseY);
	}
	for (auto& t : text) {
		t.render();
	}
	SDL_RenderPresent(gRenderer);
}

void Menu::tick(const Uint64 delta, StateStatus& res) {
	if (exit) {
		res.action = StateStatus::POP;
	} else if (next_state != nullptr) {
		res.new_state = next_state;
		res.action = StateStatus::PUSH;
		next_state = nullptr;
	}
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
