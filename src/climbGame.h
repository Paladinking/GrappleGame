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

		TileMap tilemap;
		std::shared_ptr<Player> player;
		bool grapple_pressed = false;
		
		std::vector<std::shared_ptr<Entity>> entities;
		
};




#endif