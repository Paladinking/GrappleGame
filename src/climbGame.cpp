#include <stdlib.h>
#include "climbGame.h"

void ClimbGame::tick(Uint64 delta) {
	double dDelta = delta / 1000.0;
	handle_input(dDelta);
}


void ClimbGame::handle_input(double delta) {
	const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
	
}

void ClimbGame::render() {
	SDL_RenderClear(gRenderer);
	tilemap.render();
	
	
	SDL_RenderPresent(gRenderer);
}

void ClimbGame::init() {
	tilemap.load_from_image("assets/map.png");
}