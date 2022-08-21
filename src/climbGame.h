#ifndef BALL_GAME_00_H
#define BALL_GAME_00_H
#include <vector>
#include <memory>
#include "game.h"
#include "globals.h"
#include "utilities.h"
#include "entity.h"
#include "input.h"

class ClimbGame : public Game {
	public:
		ClimbGame() : Game(SCREEN_WIDTH, SCREEN_HEIGHT, "Climbgame") {}
	protected:
		virtual void tick(Uint64 delta) override;
		
		virtual void init() override;
		
		virtual void render() override;
	
		virtual void handle_keydown(SDL_KeyboardEvent e) override;
		
		virtual void handle_keyup(SDL_KeyboardEvent e) override;
		
		virtual void handle_mousedown(SDL_MouseButtonEvent e) override;
		
		virtual void handle_mouseup(SDL_MouseButtonEvent e) override;
		
		void handle_input(double delta);
		
	private:
	
		void handle_up(const SDL_Keycode key, const Uint8 mouse);
		
		void handle_down(const SDL_Keycode key, const Uint8 mouse);
	
		void render_tilemap();
		
		void create_inputs();

		double camera_y, camera_y_min, camera_y_max;
		
		int visible_tiles_x, visible_tiles_y;

		TileMap tilemap;
		Texture ball;
		
		std::shared_ptr<Player> player;
		
		std::unique_ptr<PressInput> grapple_input, pull_input, release_input, jump_input, return_input;
		std::unique_ptr<HoldInput> left_input, right_input;
		bool do_grapple = false;
		
		std::vector<std::shared_ptr<Entity>> entities;
		std::vector<std::shared_ptr<Corner>> corners;
};




#endif