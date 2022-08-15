#ifndef EXCEPTIONS_00_H
#define EXCEPTIONS_00_H
#include <exception>
#include <string>
#include <SDL.h>

class base_exception : std::exception {
	public:
		std::string msg;
		
		base_exception(std::string msg) : msg(msg) {}; 
};

class image_load_exception : public base_exception {
	public:
		image_load_exception(std::string msg) : base_exception(msg) {};
	
};

class SDL_exception : public base_exception {
	public:
		SDL_exception(std::string msg) : base_exception(msg) {};
	
};

class Game_exception : public base_exception {
	public:
		Game_exception(std::string msg) : base_exception(msg) {};
	
};

#endif