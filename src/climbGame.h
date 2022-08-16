#ifndef BALL_GAME_00_H
#define BALL_GAME_00_H
#include "game.h"
#include "globals.h"
#include "utilities.h"
#include "player.h"

class ClimbGame : public Game {
	public:
		ClimbGame() : Game(SCREEN_WIDTH, SCREEN_HEIGHT, "Climbgame") {}
	protected:
		virtual void tick(Uint64 delta) override;
		
		virtual void init() override;
		
		virtual void render() override;
		
		void handle_input(double delta);
		
	private:
	
		void render_tilemap();
		
		double dCamera_y = 0.0;
		long camera_y;
		int visible_tiles_x, visible_tiles_y;

		const double MAX_VEL = 600.0;
		const double MAX_VEL_SQUARED = MAX_VEL * MAX_VEL;
		const double FRICTION_FACTOR = 800.0;
		const double ACC_FACTOR = 2400.0;
		
		const int TILE_SIZE = 8;
		const int FULL_TILE_HEIGHT = 160;
		const int FULL_TILE_WIDTH = 80;
		
		
		TileMap tilemap;
		Player player;
};




#endif