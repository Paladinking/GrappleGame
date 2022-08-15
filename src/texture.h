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
		
		~Texture();
		
		/**
		 * Reads a texture from a file, trowing a image_load_exception if something goes wrong.
		 */
		void load_from_file(std::string path);
	
		/**
		 * Frees all resources associated with this texture.
		 */
		void free();
		
		/**
		 * Renders this texture at position (x, y) using the global gRenderer.
		 */
		void render(const int x, const int y);
		
		/**
		 * Returns the width of this texture.
		 */
		int getWidth();
		
		/**
		 * Returns the height of this texture.
		 */
		int getHeight();
		
	private:
		SDL_Texture* texture = NULL;
		int width = 0;
		int height = 0;
};
#endif