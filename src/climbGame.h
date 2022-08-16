#ifndef BALL_GAME_00_H
#define BALL_GAME_00_H
#include <vector>
#include <memory>
#include "game.h"
#include "globals.h"
#include "utilities.h"
#include "entity.h"

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

		const double MAX_GRAVITY_VEL = 700.0;
		const double GRAVITY_ACCELERATION = 2500.0;
		const double MAX_MOVEMENT_VEL = 550.0;
		const double MOVEMENT_ACCELERATION = 2200.0;
		const double FRICTION_FACTOR = 1200.0;
		const double JUMP_VEL = 700;
		
		const int PLAYER_FULL_WIDTH = 40;
		const int PLAYER_FULL_HEIGHT = 56;
		
		
		const int TILE_SIZE = 8;
		const int FULL_TILE_HEIGHT = 160;
		const int FULL_TILE_WIDTH = 80;
		
		
		TileMap tilemap;
		std::shared_ptr<Player> player;
		
		std::vector<std::shared_ptr<Entity>> entities;
		
};




#endif