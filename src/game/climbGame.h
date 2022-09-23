#ifndef BALL_GAME_00_H
#define BALL_GAME_00_H
#include <vector>
#include <memory>
#include "util/utilities.h"
#include "engine/game.h"
#include "engine/input.h"
#include "globals.h"
#include "level.h"
#include "entity.h"


class ClimbGame : public State {
	public:
		ClimbGame() : State(SCREEN_WIDTH, SCREEN_HEIGHT, "Climbgame") {}

		virtual void tick(const Uint64 delta, StateStatus& res) override;

		virtual void init() override;

		virtual void render() override;

		virtual void handle_up(const SDL_Keycode key, const Uint8 mouse) override;

		virtual void handle_down(const SDL_Keycode key, const Uint8 mouse) override;

	private:
	
		void handle_input(double delta, StateStatus& res);
	
		void render_tilemap();
		
		void create_inputs();

		double camera_y, camera_y_min, camera_y_max;
		
		int visible_tiles_x, visible_tiles_y;

		Level level;
		Texture tilemapTexture;
		
		std::shared_ptr<Player> player;
		
		std::unique_ptr<PressInput> grapple_input, pull_input, release_input, jump_input, return_input;
		std::unique_ptr<HoldInput> left_input, right_input;
		bool do_grapple = false;
		
		std::vector<std::shared_ptr<Entity>> entities;
};




#endif