#ifndef BALL_GAME_00_H
#define BALL_GAME_00_H
#include "game.h"
#include "globals.h"
#include "utilities.h"

class BallGame : public Game {
	public:
		BallGame() : Game(SCREEN_WIDTH, SCREEN_HEIGHT, "Ballgame"), pos(0, 0), vel(0, 0) {}
	protected:
		virtual void tick(Uint64 delta) override;
		
		virtual void init() override;
		
		virtual void render() override;
		
		void handle_input(double delta);
		
	private:

		const double MAX_VEL = 600.0;
		const double MAX_VEL_SQUARED = MAX_VEL * MAX_VEL;
		const double FRICTION_FACTOR = 800.0;
		const double ACC_FACTOR = 2400.0;
		
		Texture ballTexture;
		
		Vector2D pos;
		
		Vector2D vel;
};




#endif