#ifndef TEXTURE_H_00
#define TEXTURE_H_00

#include <SDL.h>
#include <SDL_image.h>
#include <string>

/**
 * Wrapper class for an SDL_Texture, also containing width and height.
 */
class Texture {
	public:
		Texture() = default;
		
		Texture(SDL_Texture* const t, const int w, const int h) : texture(t), width(w), height(h) {};

		/**
		 * Copying a texture is not supported, since it would be very rarely desired.
		 * If so, copy the underlying SDL_Texture and create a new Texture instead.
		 */
		Texture(const Texture& o) = delete;
		Texture& operator=(const Texture&) = delete;

		Texture(Texture&& o) noexcept;
		
		Texture& operator=(Texture&&) noexcept;
		
		~Texture();
		
		/**
		 * Reads a texture from a file, trowing a image_load_exception if something goes wrong.
		 */
		void load_from_file(const std::string& path);
		
		/**
		 * Reads a texture form a file and stretches it to (w, h), throwing a image_load_exception if something goes wrong.
		 */
		void load_from_file(const std::string& path, int w, int h);
	
		/**
		 * Frees all resources associated with this texture.
		 */
		void free();
		
		/**
		 * Renders this texture at position (x, y) using the global gRenderer.
		 */
		void render(int x, int y) const;
		
		/**
		 * Renders the rectangle (x, y, w, h) of this texture at position (dest_x, dest_y),
		 * using the global gRenderer.
		 */
		void render(int dest_x, int dest_y, int x, int y, int w, int h) const;
		
		/**
		 * Returns the width of this texture.
		 */
		[[nodiscard]] int get_width() const;
		
		/**
		 * Returns the height of this texture.
		 */
		[[nodiscard]] int get_height() const;
		
		/**
		 * Sets the width and height.
		 */
		void set_dimensions(int w, int h);
		
		/** 
		 * Sets the color modulation.
		 */
		void set_color_mod(Uint8 r, Uint8 g, Uint8 b);
		
	private:
		SDL_Texture* texture = nullptr;
		int width = 0;
		int height = 0;
};
#endif