#ifndef ENGINE_00_H
#define ENGINE_00_H

// Global renderer variable
extern SDL_Renderer* gRenderer;
// Global window variable
extern SDL_Window* gWindow;

/**
 * Struct used for putting an SDL_Surface in a smart-pointer.
 */
struct SurfaceDeleter {
	void operator()(SDL_Surface* s) {SDL_FreeSurface(s);}
};

namespace engine {
	void init();

    /**
    * Returns a random number between min (inclusive) and max (exclusive).
    */
    int random(int min, int max);
};

#endif
