#ifndef TEXTURE_H_00
#define TEXTURE_H_00

#include <SDL.h>
#include <SDL_image.h>
#include <String>

/**
 * Wrapper class for an SDL_Texture, also containing width and height.
 */
class Texture {
	public:
		Texture() {};
		
		Texture(SDL_Texture* const t, const int w, const int h) : texture(t), width(w), height(h) {};
		
		~Texture();
		
		/**
		 * Reads a texture from a file, trowing a image_load_exception if something goes wrong.
		 */
		void load_from_file(std::string path);
		
		/**
		 * Reads a texture form a file and stretches it to (w, h), throwing a image_load_exception if something goes wrong.
		 */
		void load_from_file(std::string path, const int w, const int h);
	
		/**
		 * Frees all resources associated with this texture.
		 */
		void free();
		
		/**
		 * Renders this texture at position (x, y) using the global gRenderer.
		 */
		void render(const int x, const int y);
		
		/**
		 * Renders the rectangle (x, y, w, h) of this texture at position (dest_x, dest_y),
		 * using the global gRenderer.
		 */
		void render(const int dest_x, const int dest_y, const int x, const int y, const int w, const int h);
		
		/**
		 * Returns the width of this texture.
		 */
		int getWidth() const;
		
		/**
		 * Returns the height of this texture.
		 */
		int getHeight() const;
		
		/**
		 * Sets the width and height.
		 */
		void set_dimensions(const int w, const int h);
		
		/** 
		 * Sets the color modulation.
		 */
		void set_color_mod(Uint8 r, Uint8 g, Uint8 b);
		
	private:
		SDL_Texture* texture = NULL;
		int width = 0;
		int height = 0;
};
#endif