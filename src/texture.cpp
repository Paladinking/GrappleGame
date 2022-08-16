#include "texture.h"
#include "globals.h"
#include "exceptions.h"

void Texture::load_from_file(std::string path) {
	free();
	
	SDL_Texture* new_texture = IMG_LoadTexture(gRenderer, path.c_str());
	
	if (new_texture == NULL) {
		throw image_load_exception(std::string(IMG_GetError()));
	}
	// Use QueryTexture to set width and height.
	SDL_QueryTexture(new_texture, NULL, NULL, &width, &height);
	texture = new_texture;
}


void Texture::free() {
	if (texture != NULL) {
		SDL_DestroyTexture(texture);
		texture = NULL;
		width = 0;
		height = 0;
	}
}

void Texture::render(const int x, const int y) {
	SDL_Rect rect = {x, y, width, height};
	SDL_RenderCopy(gRenderer, texture, NULL, &rect); 
}

int Texture::getHeight() const {
	return height;
}

int Texture::getWidth() const {
	return width;
}

void Texture::set_dimensions(const int w, const int h) {
	this->width = w;
	this->height = h;
}

void Texture::set_color_mod(Uint8 r, Uint8 g, Uint8 b) {
	SDL_SetTextureColorMod(texture, r, g, b);
}

Texture::~Texture() {
	free();
}