#include "menu.h"
#include "util/exceptions.h"
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
	SDL_RenderFillRect(gRenderer, &r);
	texture.render(x + text_offset_x, y + text_offset_y);
	
}

void Button::set_dimensions(const int w, const int h) {
	text_offset_x = (w - this->w) / 2 + text_offset_x;
	text_offset_y = (h - this->h) / 2 + text_offset_y;
	this->w = w;
	this->h = h;
	
}


void MainMenu::init() {
	State::init();
	button = Button(100, 100, 100, 100, "Start Game");
	button.set_dimensions(200, 150);
}

void MainMenu::tick(const Uint64 delta, StateStatus& res) {
	if (next_state != nullptr) {
		res.new_state = next_state;
		next_state = nullptr; //Just for safety
		res.action = StateStatus::SWAP;
	}
}

void MainMenu::handle_down(const SDL_Keycode key, const Uint8 mouse) {
	
}

void MainMenu::render() {
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	button.render(mouseX, mouseY);
	SDL_RenderPresent(gRenderer);
}

