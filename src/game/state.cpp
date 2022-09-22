#include "state.h"
#include <SDL.h>

void State::init() {
	if (title == "") {
		title = std::string(SDL_GetWindowTitle(gWindow));
	} else {
		SDL_SetWindowTitle(gWindow, title.c_str());
	}

	if (window_width <= 0 || window_height <= 0) {
		SDL_GetWindowSize(gWindow, &window_width, &window_height);
	} else {
		SDL_SetWindowSize(gWindow, window_width, window_height);
		SDL_SetWindowPosition(gWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}
}

void State::set_mouse_state(const int mouseX, const int mouseY, const Uint8 mouseButton) {
	this->mouseX = mouseX;
	this->mouseY = mouseY;
	this->mouseButton = mouseButton;
}

const std::string& State::get_title() const {
	return title;
}

int State::get_width() const {
	return window_width;
}

int State::get_height() const {
	return window_height;
}