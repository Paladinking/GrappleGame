#include "ui.h"

TTF_Font* TextBox::font;

void TextBox::init(const std::string& font_path) {
	Button::font = TTF_OpenFont(font_path.c_str(), 20); //TODO make path dynamic
	if (Button::font == NULL) {
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
	SDL_Surface* text_surface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
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

void TextBox::render() {
	texture.render(x + text_offset_x, y + text_offset_y);
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

Menu::Menu(const int w, const int h, const std::string& title) : State(w, h, title) {
	// Since get_press_input can throw exceptions...
	exit_input = std::unique_ptr<PressInput>(new KeyPressInput(SDLK_ESCAPE));
}

Menu::Menu(const int w, const int h, const std::string& title, const std::string& exit_input) : State(w, h, title) {
	this->exit_input = get_press_input(exit_input, "Escape");
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
	if (exit_input->is_targeted(key, mouse)) {
		menu_exit();
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

void Menu::menu_exit() {
	exit = true;
}