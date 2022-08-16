#include "entity.h"


void Entity::load_texture(std::string texture_path) {
	texture.load_from_file(texture_path.c_str());
	texture.set_dimensions(width, height);
}

void Entity::tick(const double delta, const TileMap &tilemap) {
	printf("dx: %f, dy: %f\n", vel.x, vel.y);
	int tilesize = tilemap.get_tilesize();
	Vector2D to_move = {vel.x * delta, vel.y * delta};
	double len = to_move.length();
	if (len > 0) {
		// Get vector in movement direction of length TILE_SIZE.
		// Calculate how many of those steps needed for full movement + the remainder movement.
		// Then try_move won't skip any potential blocked tiles, even at low framerates.
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

void Entity::render(const int cameraY) {
	int x = pos.x, y = pos.y - cameraY;
	texture.render(x, y);
}


void Entity::try_move(const double dx, const double dy, int tilesize, const TileMap &tilemap) {
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

void Entity::add_velocity(const double dx, const double dy) {
	vel.x += dx;
	vel.y += dy;
}

Vector2D &Entity::get_velocity() {
	return vel;
}

bool Entity::on_ground(const TileMap &tilemap) const {
	int tilesize = tilemap.get_tilesize();
	int y_tile =  (pos.y + height) / tilesize;
	for (int i = pos.x / tilesize; i <= (pos.x + width - 1) / tilesize; i++) {
		if (tilemap.is_blocked(i, y_tile))
		{
			return true;
		}
	}
	return false;
}


void Player::init(const double x, const double y, const int w, const int h) {
	pos.x = x;
	pos.y = y;
	vel.x = 0;
	vel.y = 0;
	width = w;
	height = h;
	load_texture("assets/player.png");
}

