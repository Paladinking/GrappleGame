#ifndef ENGINE_00_H
#define ENGINE_00_H
extern SDL_Renderer* gRenderer;
extern SDL_Window* gWindow;

struct SurfaceDeleter {
	void operator()(SDL_Surface* s) {SDL_FreeSurface(s);}
};

#endif
