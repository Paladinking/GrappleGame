#include "entity.h"


Entity::~Entity() {}

void Entity::load_texture(std::string texture_path) 
{
	texture.load_from_file(texture_path.c_str());
	texture.set_dimensions(width, height);
}

void Entity::tick(const double delta, const TileMap &tilemap) 
{
	int tilesize = tilemap.get_tilesize();
	Vector2D to_move = {vel.x * delta, vel.y * delta};
	double len = to_move.length();
	if (len > 0) 
	{
		// Get vector in movement direction of length TILE_SIZE.
		// Calculate how many of those steps needed for full movement + the remainder movement.
		// Then try_move won't skip any potential blocked tiles, even at low framerates.
		Vector2D move_step = {(to_move.x / len) * tilesize, (to_move.y / len) * tilesize};
		int steps = len / tilesize;
		to_move.x -= steps * move_step.x;
		to_move.y -= steps * move_step.y;
		for (; steps > 0; steps--) 
		{
			try_move(move_step.x, move_step.y, tilesize, tilemap);
		}
		try_move(to_move.x, to_move.y, tilesize, tilemap);
	}
}

void Entity::render(const int cameraY) 
{
	int x = pos.x, y = pos.y - cameraY;
	texture.render(x, y);
}




void Entity::try_move(const double dx, const double dy, int tilesize, const TileMap &tilemap) 
{
	double new_x = pos.x, new_y = pos.y;
	new_x += dx;
	int x_tile = (dx > 0 ? (new_x + width - 1) : new_x) / tilesize;
	if (tilemap.is_blocked_line_v(x_tile, new_y, height)) 
	{
		vel.x = 0;
		new_x = dx > 0 ? (x_tile * tilesize - width) : (x_tile + 1) * tilesize;
	}

	new_y += dy;
	int y_tile = (dy > 0 ? (new_y + height - 1) : new_y) / tilesize;
	if (tilemap.is_blocked_line_h(y_tile, new_x, width)) 
	{
		vel.y = 0;
		new_y = dy > 0 ? y_tile * tilesize - height : (y_tile + 1) * tilesize;
	}
	pos.x = new_x;
	pos.y = new_y;
}

void Entity::add_velocity(const double dx, const double dy) 
{
	vel.x += dx;
	vel.y += dy;
}

Vector2D &Entity::get_velocity() 
{
	return vel;
}

const Vector2D &Entity::get_position() const 
{
	return pos;
}

bool Entity::on_ground(const TileMap &tilemap) const 
{
	int tilesize = tilemap.get_tilesize();
	int y_tile =  (pos.y + height) / tilesize;
	for (int i = pos.x / tilesize; i <= (pos.x + width - 1) / tilesize; i++) 
	{
		if (tilemap.is_blocked(i, y_tile))
		{
			return true;
		}
	}
	return false;
}

Player::~Player() {}

void Player::init(const double x, const double y, const int w, const int h) 
{
	pos.x = x;
	pos.y = y;
	vel.x = 0;
	vel.y = 0;
	width = w;
	height = h;
	load_texture("assets/player.png");
	grapple_hook.load_from_file("assets/ball.png");
	grapple_hook.set_dimensions(4, 4);
}

void Player::render(const int cameraY) 
{
	Entity::render(cameraY);
	if (grappling_mode == UNUSED) return;
	
	Vector2D center_point = {pos.x + width / 2, pos.y + height / 2};
	Vector2D const *prev_pos = &center_point;
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
	for (const auto &point : grapple_points) 
	{
		SDL_RenderDrawLine(gRenderer, prev_pos->x, prev_pos->y - cameraY, point.x, point.y - cameraY);
		prev_pos = &point;
	}

	grapple_hook.render(grapple_points[0].x - 2, grapple_points[0].y - cameraY - 2);
}




void Player::tick(const double delta, const TileMap &tilemap)
{
	Entity::tick(delta, tilemap);
	printf("%f\n", delta);
	
	if (grappling_mode == TRAVELING || grappling_mode == PLACED) 
	{
	/*	Vector2D p2g = {grapple_points[grapple_points.size() -1].x - pos.x, grapple_points[grapple_points.size() -1].y - pos.y};
		double len = p2g.length();
		double x_pos = pos.x + width / 2, y_pos = pos.y + height / 2;
		Vector2D step = {p2g.x / len, p2g.y / len};
		for (int steps = (int)len; steps > 0; steps--)
		{
			x_pos += step.x;
			y_pos += step.y;
			if (tilemap.is_blocked_pixel(x_pos, y_pos)) 
			{
				int prev_tile_x  = (x_ - dx) / tilesize;
				int prev_tile_y  = (y - dy) / tilesize;
				int tile_x = x / tilesize;
				int tile_y = y / tilesize;
				grapple_points.push_back(x_pos, y)
			}
		}
			*/
	}

	if (grappling_mode == TRAVELING)
	{
		if (grapple_length >= GRAPPLE_LENGTH)
		{
			grapple_points.clear();
			grappling_mode = UNUSED;
			return;
		}
		int tilesize = tilemap.get_tilesize();
		double new_x = grapple_points[0].x, new_y = grapple_points[0].y;
		Vector2D to_move = {grapple_vel.x * delta, grapple_vel.y * delta};
		double len = to_move.length();
		Vector2D move_step = {(to_move.x / len), (to_move.y / len)};
		int steps = (int)len;
		to_move.x -= steps * move_step.x;
		to_move.y -= steps * move_step.y;
		for (; steps > 0; steps--) 
		{
			new_x += move_step.x;
			new_y += move_step.y;
			if (tilemap.is_blocked_pixel(new_x, new_y))
			{
				place_grapple(new_x, new_y, move_step.x, move_step.y, tilesize);
				return;
			}
		}
		new_x += to_move.x;
		new_y += to_move.y;
		if (tilemap.is_blocked_pixel(new_x, new_y)) {
			place_grapple(new_x, new_y, to_move.x, to_move.y, tilesize);
			return;
		}
		grapple_length += len;
		grapple_points[0].x = new_x;
		grapple_points[0].y = new_y;
	} 
}

void Player::place_grapple(const double x, const double y, const double dx, const double dy, const int tilesize) 
{
	grappling_mode = PLACED;
	int prev_tile_x  = (x - dx) / tilesize;
	int prev_tile_y  = (y - dy) / tilesize;
	int tile_x = x / tilesize;
	int tile_y = y / tilesize;
	grapple_points[0].x = (((int)x) / tilesize + 0.5 + prev_tile_x - tile_x) * tilesize;
	grapple_points[0].y = (((int)y) / tilesize + 0.5 + prev_tile_y - tile_y) * tilesize;
}

void Player::fire_grapple(const int targetX, const int targetY) 
{
	if (grappling_mode == UNUSED) 
	{
		grappling_mode = TRAVELING;
		grapple_length = 0;
		grapple_vel.x = targetX - (pos.x + width  / 2);
		grapple_vel.y = targetY - (pos.y + height / 2);
		grapple_vel.normalize();
		grapple_vel.scale(GRAPPLE_SPEED);
		grapple_points.push_back({pos.x + width / 2, pos.y + height / 2});
	}
	else if (grappling_mode != TRAVELING)
	{
		grapple_points.clear();
		grappling_mode = UNUSED;
	}

	
}
