#ifndef BALL_GAME_00_H
#define BALL_GAME_00_H
#include <vector>
#include <memory>
#include <unordered_map>
#include "util/utilities.h"
#include "engine/game.h"
#include "engine/input.h"
#include "globals.h"
#include "level.h"
#include "entity.h"


class ClimbGame : public State {
	public:
		ClimbGame() : State(), level(TILE_SIZE) {}

		void tick(Uint64 delta, StateStatus& res) override;

		void init(WindowState* window_state) override;

		void render() override;

		void handle_up(SDL_Keycode key, Uint8 mouse) override;

		void handle_down(SDL_Keycode key, Uint8 mouse) override;

	private:

		void handle_input(StateStatus &res);

		void create_inputs();
		
		SDL_Rect game_viewport = {};

		double camera_y = 0.0, camera_y_min = 0.0, camera_y_max = 0.0;

		int visible_tiles_x = 0, visible_tiles_y = 0;

		Level level;

		std::shared_ptr<Player> player;
		
		std::unique_ptr<PressInput> grapple_input, pull_input, release_input, jump_input, return_input;
		std::unique_ptr<HoldInput> left_input, right_input;
		bool do_grapple = false;
		
		std::vector<std::shared_ptr<Entity>> entities;
		std::unique_ptr<EntityTemplate> player_template;
};




#endif