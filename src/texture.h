#ifndef TEXTURE_H_00
#define TEXTURE_H_00

#include <SDL.h>
#include <SDL_image.h>
#include <String>

class Texture {
	public:
		Texture() {};
		
		~Texture();
		
		void load_from_file(std::string path);
	
		void free();
		
		void render(const int x, const int y);
		
		int getWidth();
		
		int getHeight();
		
	private:
		SDL_Texture* texture = NULL;
		int width = 0;
		int height = 0;
};
#endif