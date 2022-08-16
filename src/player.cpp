#include "player.h"


void Player::init(const double x, const double y, const int w, const int h) {
	texture.load_from_file("assets/player.png");
	pos.x = x;
	pos.y = y;
	vel.x = 0;
	vel.y = 0;
	texture.set_dimensions(w, h);
	width = w;
	height = h;
}

void Player::render(int cameraY) {
	int x = pos.x, y = pos.y - cameraY;
	texture.render(x, y);
}

void Player::tick(const double delta, const TileMap &tilemap)
{	
	int tilesize = tilemap.get_tilesize();
	Vector2D to_move = {vel.x * delta, vel.y * delta};
	double len = to_move.length();
	if (len > 0) {
		Vector2D move_step = {(to_move.x / len) * tilesize, (to_move.y / len) * tilesize};
		int steps = len / tilesize;
		to_move.x -= steps * move_step.x;
		to_move.y -= steps * move_step.y;
		for (; steps > 0; steps--) {
			try_move(move_step.x, move_step.y, tilesize, tilemap);
		}
		try_move(to_move.x, to_move.y, tilesize, tilemap);
	}
}

/**
 * Tries to move the player by (dx, dy), stopping if a wall is in the way. 
 * Requires that -tilesize < dx < tilesize and -tilesize < dy < tilesize
 */
void Player::try_move(double dx, double dy, int tilesize, const TileMap &tilemap) {
	double new_x = pos.x, new_y = pos.y;
	new_x += dx;
	int x_tile = (dx > 0 ? (new_x + width - 1) : new_x) / tilesize;
	for (int i = new_y / tilesize; i <= (new_y + height - 1) / tilesize; i++) {
		if (tilemap.is_blocked(x_tile, i))
		{
			vel.x = 0;
			new_x = dx > 0 ? (x_tile * tilesize - width) : (x_tile + 1) * tilesize;
			break;
		}
	}

	new_y += dy;
	int y_tile = (dy > 0 ? (new_y + height - 1) : new_y) / tilesize;
	for (int i = new_x / tilesize; i <= (new_x + width - 1) / tilesize; i++) {
		if (tilemap.is_blocked(i, y_tile))
		{
			vel.y = 0;
			new_y = dy > 0 ? (y_tile * tilesize - height) : (y_tile + 1) * tilesize; 
			break;
		}
	}
	
	pos.x = new_x;
	pos.y = new_y;
}