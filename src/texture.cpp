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

void Texture::load_from_file(std::string path, const int w, const int h) {
	SDL_Surface* surface = IMG_Load(path.c_str());
	if (surface == NULL) {
		throw image_load_exception(std::string(IMG_GetError()));
	}
	SDL_PixelFormat* format = surface->format;
	SDL_Surface* stretched = SDL_CreateRGBSurfaceWithFormat(0, w, h, format->BitsPerPixel, format->format);
	if (stretched == NULL) {
		SDL_FreeSurface(surface);
		throw image_load_exception(std::string(IMG_GetError()));
	}
	SDL_BlitScaled(surface, NULL, stretched, NULL);
	SDL_FreeSurface(surface);
	
	texture = SDL_CreateTextureFromSurface(gRenderer, stretched);
	SDL_FreeSurface(stretched);
	
	if (texture == NULL) {
		throw image_load_exception(std::string(IMG_GetError()));
	}
	
	width = w;
	height = h;	
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

void Texture::render(const int dest_x, const int dest_y, const int x, const int y, const int w, const int h) {
	SDL_Rect target = {dest_x, dest_y, width, height};
	SDL_Rect source = {x, y, w, h};
	SDL_RenderCopy(gRenderer, texture, &source, &target);
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