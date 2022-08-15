#ifndef UTILITIES_00_H
#define UTILITIES_00_H
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include "globals.h"
#include "exceptions.h"

/**
 * Class containing a 2-dimensional vector using double.
 */
class Vector2D {
	public:
		/**
		 * Creates a new vector.
		 */
		Vector2D(double x, double y) : x(x), y(y) {}
		
		/**
		 * Gets the length of this vector.
		 */
		double length() const
		{
			return sqrt(x * x + y * y);
		}
		
		
		/**
		 * Gets the squared length of this vector.
		 */
		double length_squared() const
		{
			return x * x + y * y;
		}
		
		/**
		 * Gets the distance from this vector to other.
		 */
		double distance(const Vector2D &other) const 
		{
			return sqrt((other.x - x) * (other.x - x) + (other.y - y) * (other.y - y));
		}
		
		/**
		 * Gets the squared distance from this vector to other.
		 */
		double distance_squared(const Vector2D &other) const
		{
			return (other.x - x) * (other.x - x) + (other.y - y) * (other.y - y);
		}
		
		/**
		 * Adds this vector to other.
		 */
		void add(Vector2D &other) 
		{
			x += other.x;
			y += other.y;
		}
		
		/**
		 * Adds this vector to other using a scaling factor.
		 */
		void add_scaled(Vector2D &other, double factor) 
		{
			x += other.x * factor;
			y += other.y * factor;
		}
		
		/**
		 * Scales this vector.
		 */
		void scale(double factor) 
		{
			x *= factor;
			y *= factor;
		}
		
		/**
		 * Nomalizes this vector.
		 */
		void normalize() 
		{
			double len = length();
			if (len == 0.0) {
				return;
			}
			x /= len;
			y /= len;
		}
		
		/**
		 * Returns the dot-product of this vector and other.
		 */
		double dot(Vector2D &other)
		{
			return x * other.x + y * other.y;
		}
		
		/**
		 * Reduces the length of this vector by amount, stopping at 0.
		 */
		void decreace(double amount) {
			double len = length();
			double diff = len - amount;
			if (diff <= 0) {
				x = 0;
				y = 0;
				return;
			}
			x = (x / len) * diff;
			y = (y / len) * diff;
		}
		
		/**
		 * Subtracts other vector from this.
		 */
		void subtract(Vector2D &other) {
			x -= other.x;
			y -= other.y;
		}
		
		double x, y;
};

/**
 * A tilemap for collisions.
 */
class TileMap {
	public:
		TileMap() {}
		
		~TileMap() {
			delete[] map;
		}
	
		TileMap(const unsigned width, const unsigned height) : width(width), height(height) {} 
	
		/**
		 * Loads this tilemap from an immage, trowing an image_load_exception if something goes wrong.
		 * White pixels are free, all others are filled.
		 */
		void load_from_image(std::string path) {
			SDL_Surface* surface = IMG_Load(path.c_str());
			if (surface == NULL) {
				throw image_load_exception("Could not load tilemap image, " + std::string(IMG_GetError()));
			}
			SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
			SDL_FreeSurface(surface);
			
			SDL_LockSurface(converted);
			width = converted->w;
			height = converted->h;
			map = new bool[width * height];
			SDL_PixelFormat* fmt = converted->format;

			for (unsigned i = 0; i < width; i++) {
				for (unsigned j = 0;j < height; j++) {
					Uint32 temp;
					
					Uint32 pixel = ((Uint32*)surface->pixels)[i + j * width];
					Uint8 red, green, blue, alpha;
					/* Get Red component */
					temp = pixel & fmt->Rmask;  /* Isolate red component */
					temp = temp >> fmt->Rshift; /* Shift it down to 8-bit */
					temp = temp << fmt->Rloss;  /* Expand to a full 8-bit number */
					red = (Uint8)temp;
					
					/* Get Green component */
					temp = pixel & fmt->Gmask;  /* Isolate green component */
					temp = temp >> fmt->Gshift; /* Shift it down to 8-bit */
					temp = temp << fmt->Gloss;  /* Expand to a full 8-bit number */
					green = (Uint8)temp;
					
					/* Get Blue component */
					temp = pixel & fmt->Bmask;  /* Isolate blue component */
					temp = temp >> fmt->Bshift; /* Shift it down to 8-bit */
					temp = temp << fmt->Bloss;  /* Expand to a full 8-bit number */
					blue = (Uint8)temp;
					
					// White pixels are false, others true
					map[i + j * width] = (red != 0xff || green != 0xff || blue != 0xff);
				}
			}
			SDL_UnlockSurface(converted);
			SDL_FreeSurface(converted);
		}
		
		void render() {
			int tileSize = SCREEN_WIDTH / width;
			for (int i = 0; i < width; i++) {
				for (int j = 0; j < height; j++) {
					if (map[i + width * j]) {
						SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );     
					} else {
						SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );  
					}				
					SDL_Rect fillRect = {i * tileSize, j * tileSize, tileSize, tileSize};
					SDL_RenderFillRect( gRenderer, &fillRect );
				}
			}
			
		}
		
	
	
	private:
		unsigned width = 0, height = 0;
		
		bool *map = NULL;
};



#endif