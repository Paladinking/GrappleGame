#ifndef PLAYER_00_H
#define PLAYER_00_H
#include "utilities.h"
#include "texture.h"

class Player {
	public:
		Player() {};
		
		void init(const double x, const double y, const int w, const int h);
		
		void render(const int cameraY);
		
		void tick(const double delta, const TileMap &tilemap);
	
	private:
		
		void try_move(double dx, double dy, int tilesize, const TileMap &tilemap);
		
		Texture texture;
	
		Vector2D pos;
		Vector2D vel;
		
		int width, height;
	
};
#endif