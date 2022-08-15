#include <stdlib.h>
#include "ballGame.h"

void BallGame::tick(Uint64 delta) {
	double dDelta = delta / 1000.0;
	handle_input(dDelta);
	pos.add_scaled(vel, dDelta);
	if (pos.x + ballTexture.getWidth() >= window_width) 
	{
		vel.x *= -1.0;
		pos.x = window_width - ballTexture.getWidth();
	} else if (pos.x <= 0) 
	{
		vel.x *= -1;
		pos.x = 0;
	}
	if (pos.y + ballTexture.getHeight() >= window_height) 
	{
		vel.y *= -1;
		pos.y = window_height - ballTexture.getHeight();
	} else if (pos.y <= 0) 
	{
		vel.y *= -1;
		pos.y = 0;
	}
	
	vel.decreace(dDelta * FRICTION_FACTOR);
}


void BallGame::handle_input(double delta) {
	const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
	Vector2D acc(0.0, 0.0);
    if(currentKeyStates[SDL_SCANCODE_UP])
    {
        acc.y -= 1.0;
    }
    if(currentKeyStates[SDL_SCANCODE_DOWN])
    {
        acc.y += 1.0;
    }
    if(currentKeyStates[SDL_SCANCODE_LEFT])
    {
        acc.x -= 1.0;
    }
    if(currentKeyStates[SDL_SCANCODE_RIGHT])
    {
		acc.x += 1.0;
    }
	if(currentKeyStates[SDL_SCANCODE_ESCAPE]) 
	{
		exit_game();
	}
	acc.normalize();
	vel.add_scaled(acc, delta * ACC_FACTOR);
	if (vel.length_squared() > MAX_VEL_SQUARED)
	{
		vel.normalize();
		vel.scale(MAX_VEL);
	}
}

void BallGame::render() {
	SDL_RenderClear(gRenderer);
	
	ballTexture.render(pos.x, pos.y);
	
	SDL_RenderPresent(gRenderer);
}

void BallGame::init() {
	vel = {1000.0, 1000.0};
	ballTexture.load_from_file("assets/ball.png");
	pos.x = window_width /2 - ballTexture.getWidth() / 2;
	pos.y = window_height / 2 - ballTexture.getHeight() / 2;
}