#include "ui.h"

TTF_Font* TextBox::font;

void TextBox::init(SDL_RWops* font_data) {
	TextBox::font = TTF_OpenFontRW(font_data, 1, 20);
	if (TextBox::font == NULL) {
		throw game_exception(std::string(TTF_GetError()));
	}
}

TextBox::TextBox(const int x, const int y, const int w, const int h, const std::string& text) : TextBox(x, y, w, h, text, 20) {};

TextBox::TextBox(
	const int x, const int y, const int w, const int h, const std::string& text, const int font_size) : x(x), y(y), w(w), h(h), text(text), font_size(font_size), texture(nullptr, 0, 0) {
	generate_texture();
}


void TextBox::generate_texture() {
	TTF_SetFontSize(font, font_size);
	SDL_Surface* text_surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
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

void TextBox::set_text_color(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a) {
	color = {r, g, b, a};
	generate_texture();
}

const SDL_Color& TextBox::get_text_color() const {
	return color;
}

void TextBox::set_font_size(const int font_size) {
	this->font_size = font_size;
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

void TextBox::render(const int x_offset, const int y_offset) {
	texture.render(x_offset + x + text_offset_x, y_offset + y + text_offset_y);
}

bool Button::is_pressed(const int mouseX, const int mouseY) const {
	return mouseX >= x && mouseX < x + w && mouseY >= y && mouseY < y + h;
}

void Button::set_hover(const bool hover) {
	this->hover = hover;
}

void Button::render(const int x_offset, const int y_offset) {
	if (hover) {
		SDL_SetRenderDrawColor(gRenderer, 200, 200, 240, 0xFF);
	} else {
		SDL_SetRenderDrawColor(gRenderer, 100, 100, 220, 0xFF);
	}
	SDL_Rect r = {x + x_offset, y + y_offset, w, h};
	SDL_RenderFillRect(gRenderer, &r);
	TextBox::render(x_offset, y_offset);
}

Menu::Menu() : State() {
	// Since get_press_input can throw exceptions...
	exit_input = std::unique_ptr<PressInput>(new KeyPressInput(SDLK_ESCAPE));
}

Menu::Menu(const std::string& exit_input) : State() {
	this->exit_input = get_press_input(exit_input, "Escape");
}

void Menu::handle_down(const SDL_Keycode key, const Uint8 mouse) {
	if (mouse == SDL_BUTTON_LEFT) {
		targeted_button = -1;
		for (int i = 0; i < buttons.size(); ++i) {
			if (buttons[i].is_pressed(window_state->mouseX, window_state->mouseY)) {
				targeted_button = i;
				break;
			}
		}
	}
	if (exit_input->is_targeted(key, mouse)) {
		menu_exit();
	}
}

void Menu::handle_up(const SDL_Keycode key, const Uint8 mouse) {
	if (mouse == SDL_BUTTON_LEFT) {
		if (targeted_button >= 0 && buttons[targeted_button].is_pressed(window_state->mouseX, window_state->mouseY)) {
			button_press(targeted_button);
		}
	}
}

void Menu::render() {
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	for (auto& b : buttons) {
		b.set_hover(b.is_pressed(window_state->mouseX, window_state->mouseY));
		b.render(0, 0);
	}
	for (auto& t : text) {
		t.render(0, 0);
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

void Menu::menu_exit() {
	exit = true;
}