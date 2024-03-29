#include "util/exceptions.h"
#include "texture.h"
#include "engine.h"

void Texture::load_from_file(const std::string& path) {
	free();
	
	SDL_Texture* new_texture = IMG_LoadTexture(gRenderer, path.c_str());
	
	if (new_texture == nullptr) {
		throw image_load_exception(std::string(IMG_GetError()));
	}
	// Use QueryTexture to set width and height.
	SDL_QueryTexture(new_texture, nullptr, nullptr, &width, &height);
	texture = new_texture;
}

void Texture::load_from_file(const std::string& path, const int w, const int h) {
	free();
	SDL_Surface* surface = IMG_Load(path.c_str());
	if (surface == nullptr) {
		throw image_load_exception(std::string(IMG_GetError()));
	}
	SDL_PixelFormat* format = surface->format;
	SDL_Surface* stretched = SDL_CreateRGBSurfaceWithFormat(0, w, h, format->BitsPerPixel, format->format);
	if (stretched == nullptr) {
		SDL_FreeSurface(surface);
		throw image_load_exception(std::string(IMG_GetError()));
	}
	SDL_BlitScaled(surface, nullptr, stretched, nullptr);
	SDL_FreeSurface(surface);
	
	texture = SDL_CreateTextureFromSurface(gRenderer, stretched);
	SDL_FreeSurface(stretched);
	
	if (texture == nullptr) {
		throw image_load_exception(std::string(IMG_GetError()));
	}
	
	width = w;
	height = h;	
}


void Texture::free() {
	if (texture != nullptr) {
		SDL_DestroyTexture(texture);
		texture = nullptr;
		width = 0;
		height = 0;
	}
}

void Texture::render(const int x, const int y) const {
	SDL_Rect rect = {x, y, width, height};
	SDL_RenderCopy(gRenderer, texture, nullptr, &rect);
}

void Texture::render(const int dest_x, const int dest_y, const int x, const int y, const int w, const int h) const {
	SDL_Rect target = {dest_x, dest_y, width, height};
	SDL_Rect source = {x, y, w, h};
	SDL_RenderCopy(gRenderer, texture, &source, &target);
}

int Texture::get_height() const {
	return height;
}

int Texture::get_width() const {
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

Texture::Texture(Texture&& o) noexcept {
	free();
	texture = o.texture;
	width = o.width;
	height = o.height;
	o.texture = nullptr;
}

Texture& Texture::operator=(Texture&& o) noexcept {
	free();
	texture = o.texture;
	width = o.width;
	height = o.height;
	o.texture = nullptr;
	return *this;
}