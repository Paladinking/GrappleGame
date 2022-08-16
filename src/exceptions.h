#ifndef EXCEPTIONS_00_H
#define EXCEPTIONS_00_H
#include <exception>
#include <string>
#include <SDL.h>

/**
 * Base exception class containing a message.
 */
class base_exception : std::exception {
	public:
		std::string msg;
		
		base_exception(std::string msg) : msg(msg) {}; 
};

/**
 * image_load_exception, for when loading an image fails.
 */
class image_load_exception : public base_exception {
	public:
		image_load_exception(std::string msg) : base_exception(msg) {};
	
};

/**
 * SDL_exception, for when loading a part of the SDL library fails.
 */
class SDL_exception : public base_exception {
	public:
		SDL_exception(std::string msg) : base_exception(msg) {};
	
};

/**
 * Game_exception, when creating a game fails (for example when one is already runnning).
 */
class Game_exception : public base_exception {
	public:
		Game_exception(std::string msg) : base_exception(msg) {};
	
};

#endif