#include "entity.h"
#include "engine/engine.h"
#include <iostream>
#include "util/geometry.h"

constexpr double MAX_GRAVITY_VEL = 700.0;
constexpr double GRAVITY_ACCELERATION = 3000.0;
constexpr double FRICTION_FACTOR = 700.0;
constexpr double AIR_RES_FACTOR = 0.004;

constexpr int GRAPPLE_LENGTH = 600;
constexpr double GRAPPLE_SPEED = 1300.0;
constexpr double GRAPPLE_PULL = 5000.0;
constexpr double GRAPPLE_RELEASE = 200.0;

Entity::~Entity() {}

void Entity::load_texture(std::string texture_path) 
{
	texture.load_from_file(texture_path.c_str());
	texture.set_dimensions(width, height);
}

void Entity::tick(const double delta, Level& level) 
{
	int tilesize = level.get_tilesize();
	vel.x += acc.x * delta;
	vel.y += acc.y * delta;
	Vector2D to_move = {vel.x * delta, vel.y * delta};
	acc.x = 0.0;
	acc.y = 0.0;
	double len = to_move.length();
	if (len > 0) 
	{
		// Get vector in movement direction of length TILE_SIZE.
		// Calculate how many of those steps needed for full movement + the remainder movement.
		// Then try_move won't skip any potential blocked tiles, even at low framerates.
		Vector2D move_step = {(to_move.x / len) * tilesize, (to_move.y / len) * tilesize};
		int steps = static_cast<int>(len / tilesize);
		to_move.x -= steps * move_step.x;
		to_move.y -= steps * move_step.y;
		for (; steps > 0; steps--) 
		{
			try_move(move_step.x, move_step.y, tilesize, level);
		}
		try_move(to_move.x, to_move.y, tilesize, level);
	}
}

void Entity::render(const int cameraY) 
{
	int x = static_cast<int>(pos.x), y = static_cast<int>(pos.y - cameraY);
	texture.render(x, y);
}




void Entity::try_move(const double dx, const double dy, int tilesize, const Level &level) 
{
	double new_x = pos.x, new_y = pos.y;
	new_x += dx;
	int x_tile = static_cast<int>((dx > 0 ? (new_x + width - 1) : new_x) / tilesize);
	if (level.is_blocked_line_v(x_tile, new_y, height)) 
	{
		vel.x = 0;
		new_x = dx > 0 ? (x_tile * tilesize - width) : (x_tile + 1) * tilesize;
	}

	new_y += dy;
	int y_tile = static_cast<int>((dy > 0 ? (new_y + height - 1) : new_y) / tilesize);
	if (level.is_blocked_line_h(y_tile, new_x, width))
	{
		vel.y = 0;
		new_y = dy > 0 ? y_tile * tilesize - height : (y_tile + 1) * tilesize;
	}
	pos.x = new_x;
	pos.y = new_y;
}

void Entity::add_acceleration(const double dx, const double dy) 
{
	acc.x += dx;
	acc.y += dy;
}

void Entity::add_velocity(const double dx, const double dy) {
	vel.x += dx;
	vel.y += dy;
}

const Vector2D &Entity::get_velocity() const 
{
	return vel;
}

const Vector2D &Entity::get_position() const 
{
	return pos;
}

bool Entity::on_ground(const Level &level) const 
{
	int tilesize = level.get_tilesize();
	int y_tile =  static_cast<int>((pos.y + height) / tilesize);
	for (int i = static_cast<int>(pos.x / tilesize); i <= (pos.x + width - 1) / tilesize; ++i)
	{
		if (level.is_blocked(i, y_tile))
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
	grapple_max_len = GRAPPLE_LENGTH;
	load_texture(ASSETS_ROOT + PLAYER_IMG);
	grapple_hook.load_from_file(ASSETS_ROOT + HOOK_IMG);
	grapple_hook.set_dimensions(4, 4);
}

void Player::render(const int cameraY) 
{
	Entity::render(cameraY);
	if (grappling_mode == UNUSED) return;
	
	Corner prev_pos = {(pos.x + width / 2), (pos.y + height / 2)};
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
	for (auto it = grapple_points.rbegin(); it != grapple_points.rend(); it++) 
	{
		SDL_RenderDrawLine(gRenderer,
			static_cast<int>(prev_pos.x),
			static_cast<int>(prev_pos.y - cameraY),
			static_cast<int>(it->corner->x),
			static_cast<int>(it->corner->y - cameraY)
		);
		prev_pos = *(it->corner);
	}

	grapple_hook.render(static_cast<int>(hook->x) - 2, static_cast<int>(hook->y - cameraY) - 2);
	
	
}



void Player::tick(const double delta, Level &level)
{
	Vector2D old_pos = {pos.x + width / 2, pos.y + height / 2};
	bool is_on_ground = on_ground(level);
	if (is_on_ground) {
		double factor = delta * FRICTION_FACTOR;
		if (vel.x > 0) {
			acc.x += factor > vel.x ? -vel.x : -FRICTION_FACTOR;
		} else {
			acc.x += factor > -vel.x ? -vel.x : FRICTION_FACTOR;
		}
	}
	if (vel.y < MAX_GRAVITY_VEL) {
		acc.y += GRAVITY_ACCELERATION;
	}
	acc.x -= vel.x * std::abs(vel.x) * AIR_RES_FACTOR;
	acc.y -= vel.y * std::abs(vel.y) * AIR_RES_FACTOR;
	
	if (grappling_mode == PLACED && pull) {
		const std::shared_ptr<Corner> &anchor = grapple_points[grapple_points.size() - 2].corner;
		Vector2D line_vector = {anchor->x - center_point->x, anchor->y - center_point->y};
		double line_vec_len = line_vector.length();
		acc.x += line_vector.x / line_vec_len * GRAPPLE_PULL;
		acc.y += line_vector.y / line_vec_len * GRAPPLE_PULL;
	}


	vel.x += delta * acc.x;
	vel.y += delta * acc.y;
	acc.x = 0.0;
	acc.y = 0.0;

	const double delta_sq = delta * delta;

	Vector2D to_move = {vel.x * delta + 0.5 * acc.x * delta_sq, vel.y * delta + 0.5 * acc.y * delta_sq};
	
	if (grappling_mode == PLACED && (to_move.x != 0 || to_move.y != 0)) {
		const std::shared_ptr<Corner> &anchor = grapple_points[grapple_points.size() - 2].corner;
		Vector2D line_vector = {anchor->x - center_point->x, anchor->y - center_point->y};
		Vector2D new_line_vector = {anchor->x - (center_point->x + to_move.x), anchor->y - (center_point->y + to_move.y)};
		const double prev_len = line_vector.length();
		const double new_len = new_line_vector.length();
		const double len = grapple_length - prev_len + new_len;

		if (len >= grapple_max_len) {	
			double angle = get_angle(line_vector.x, line_vector.y, to_move.x, to_move.y);
			if (angle > PI_HALF) {
				double rotated_x = -line_vector.y, rotated_y = line_vector.x;
				double to_move_scalar = (rotated_x * to_move.x + rotated_y * to_move.y) /
						(rotated_x * rotated_x + rotated_y * rotated_y);
				double vel_scalar = (rotated_x * vel.x + rotated_y * vel.y) / (rotated_x * rotated_x + rotated_y * rotated_y);

				Vector2D new_to_move = {to_move_scalar * rotated_x, to_move_scalar * rotated_y};
				vel.x = vel_scalar * rotated_x;
				vel.y = vel_scalar * rotated_y;
				const double desired_length = grapple_max_len - grapple_length + prev_len;

				to_move.x = -(center_point->x + (new_line_vector.x / new_len) * desired_length - anchor->x);
				to_move.y = -(center_point->y + (new_line_vector.y / new_len) * desired_length - anchor->y);

			} 
		}
	}


	int tilesize = level.get_tilesize();
	double len = to_move.length();
	if (len > 0) 
	{
		// Get vector in movement direction of length TILE_SIZE.
		// Calculate how many of those steps needed for full movement + the remainder movement.
		// Then try_move won't skip any potential blocked tiles, even at low framerates.
		Vector2D move_step = {(to_move.x / len) * tilesize, (to_move.y / len) * tilesize};
		int steps = static_cast<int>(len / tilesize);
		to_move.x -= steps * move_step.x;
		to_move.y -= steps * move_step.y;
		for (; steps > 0; steps--) 
		{
			try_move(move_step.x, move_step.y, tilesize, level);
		}
		try_move(to_move.x, to_move.y, tilesize, level);
	}

	if (grappling_mode == TRAVELING || (grappling_mode == PLACED && (len > 0))) 
	{
		center_point->x = pos.x + width / 2;
		center_point->y = pos.y + height / 2;
		update_grapple(level.get_corners(), old_pos, false);
		if (pull) {
			if (grapple_length + 10.0 < grapple_max_len) {
				grapple_max_len = grapple_length + 10.0;
			}
		} else if (release) {
			if (grapple_max_len < GRAPPLE_LENGTH) {
				grapple_max_len += GRAPPLE_RELEASE * delta;
			}
		}
	}

	if (grappling_mode == TRAVELING)
	{
		Vector2D to_move = {grapple_vel.x * delta, grapple_vel.y * delta};
		
		
		grapple_vel.y += GRAVITY_ACCELERATION * delta;
		
		if (grapple_length >= grapple_max_len)
		{
			const std::shared_ptr<Corner> &anchor = grapple_points[1].corner;
			Vector2D line_vector = {anchor->x - hook->x, anchor->y - hook->y};
			double rotated_x = -line_vector.y, rotated_y = line_vector.x;
			double proj_scalar = (rotated_x * grapple_vel.x + rotated_y * grapple_vel.y)
						/ (rotated_x * rotated_x + rotated_y * rotated_y);
			grapple_vel.x = proj_scalar * rotated_x;
			grapple_vel.y = proj_scalar * rotated_y;
			
			double prev_len = line_vector.length();
			line_vector.x = anchor->x - (hook->x + grapple_vel.x * delta);
			line_vector.y = anchor->y - (hook->y + grapple_vel.y * delta);
			double new_len = line_vector.length();
	
			to_move.x = -(hook->x + (line_vector.x / new_len) * (grapple_max_len - (grapple_length - prev_len)) - anchor->x);
			to_move.y = -(hook->y + (line_vector.y / new_len) * (grapple_max_len - (grapple_length - prev_len)) - anchor->y);
			
		}
		
		double new_x = hook->x, new_y = hook->y;
		double len = to_move.length();
		Vector2D move_step = {(to_move.x / len), (to_move.y / len)};
		int steps = (int)len;
		to_move.x -= steps * move_step.x;
		to_move.y -= steps * move_step.y;
		for (; steps > 0; steps--) 
		{
			new_x += move_step.x;
			new_y += move_step.y;
			if (level.is_blocked_pixel(new_x, new_y))
			{
				place_grapple(new_x, new_y, move_step.x, move_step.y, tilesize, level.get_corners());
				return;
			}
		}
		new_x += to_move.x;
		new_y += to_move.y;
		if (level.is_blocked_pixel(new_x, new_y)) {
			place_grapple(new_x, new_y, to_move.x, to_move.y, tilesize, level.get_corners());
			return;
		}
		Vector2D prev = {hook->x, hook->y};
		hook->x = new_x;
		hook->y = new_y;
		update_grapple(level.get_corners(), prev, true);
	} 
}

void Player::place_grapple(const double x, const double y, const double dx, const double dy, const int tilesize, CornerList &corners) 
{
	grappling_mode = PLACED;
	int prev_tile_x  = static_cast<int>((x - dx) / tilesize);
	int prev_tile_y  = static_cast<int>((y - dy) / tilesize);
	int tile_x = static_cast<int>(x / tilesize);
	int tile_y = static_cast<int>(y / tilesize);
	Vector2D prev = {hook->x, hook->y};
	hook->x = (((int)x) / tilesize + 0.5 + prev_tile_x - tile_x) * tilesize;
	hook->y = (((int)y) / tilesize + 0.5 + prev_tile_y - tile_y) * tilesize;
	update_grapple(corners, prev, true);
}

void Player::return_grapple() {
	if (grappling_mode == TRAVELING ||  grappling_mode == PLACED) {
		grapple_points.clear();
		grappling_mode = UNUSED;
	}
}

void Player::fire_grapple(const int targetX, const int targetY) 
{
	if (grappling_mode == UNUSED || (grappling_mode == PLACED && grapple_length < 20.0)) 
	{
		grapple_points.clear();
		grappling_mode = TRAVELING;
		pull = false;
		release = false;
		grapple_length = 0;
		grapple_max_len = GRAPPLE_LENGTH;
		grapple_vel.x = targetX - (pos.x + width  / 2);
		grapple_vel.y = targetY - (pos.y + height / 2);
		grapple_vel.normalize();
		grapple_vel.scale(GRAPPLE_SPEED);
		hook = std::shared_ptr<Corner>(new Corner(pos.x + width / 2, pos.y + height / 2));
		center_point = std::shared_ptr<Corner>(new Corner(pos.x + width / 2, pos.y + height / 2));
		grapple_points.push_back({hook, false});
		grapple_points.push_back({center_point, false});
	} else if(grappling_mode == PLACED) {
		return_grapple();
	}
}

void Player::set_pull(bool b) {
	pull = b;
}

void Player::set_release(bool b) {
	release = b;
}

void Player::update_grapple(CornerList &corners, Vector2D prev, bool first) {
	CornerList contained;
	update_grapple(corners, corners, contained, prev, first);
	double len = 0.0;
	for (unsigned i = 0; i < grapple_points.size() - 1; ++i) {
		const GrapplePoint &p1 = grapple_points[i];
		const GrapplePoint &p2 = grapple_points[i + 1];
		double dx = p1.corner->x - p2.corner->x, dy = p1.corner->y - p2.corner->y;
		len += std::sqrt(dx * dx + dy * dy);
	}
	grapple_length = len;
}

void Player::update_grapple(CornerList &allCorners, CornerList &corners, CornerList &contained, Vector2D prev, bool first)
{
	// Index of moved point, direction to go in vector.
	int mp_index = 0, dir = 1;
	if (!first) {
		mp_index = grapple_points.size() - 1;
		dir = -1;
	}
	std::shared_ptr<Corner> cur = grapple_points[mp_index].corner;
	
	// The fixed point connected to the moved point.
	GrapplePoint &anchorPoint = grapple_points[mp_index + dir];
	std::shared_ptr<Corner> anchor = anchorPoint.corner;
	
	bool free_point = false, add_point = false;
	double smallest_angle = 100.0; //Largest possible angle is PI, so 100.0 will always be larger.
	
	std::shared_ptr<Corner> prev_anchor;
	
	// Check if the anchor point is free, by seeing of the orientation has changed from when the point was created.
	// Include first in boolean logic so that the orientation is the same from both sides.
	// If the point is free, potentialy it should be removed from the vector.
	if (grapple_points.size() > 2) {
		prev_anchor = grapple_points[mp_index + 2 * dir].corner;
		bool orientation = first != is_clockwise(prev_anchor->x, prev_anchor->y, anchor->x, anchor->y, cur->x, cur->y);
		if (orientation != anchorPoint.orientation)
		{
			smallest_angle = get_angle(anchor->x - prev_anchor->x, anchor->y - prev_anchor->y, prev.x - anchor->x, prev.y - anchor->y);
			free_point = true;
		}
	}
	
	Triangle t = {prev.x, prev.y, cur->x, cur->y, anchor->x, anchor->y};
	anchor->ignored = true;
	std::shared_ptr<Corner> to_be_added;

	for (std::shared_ptr<Corner> &corner : corners) {
		if (!corner->ignored && t.contains_point(corner->x, corner->y)) {
			contained.push_back(corner);
			double angle = get_angle(prev.x, prev.y, anchor->x, anchor->y, corner->x, corner->y);
			if (angle < smallest_angle) {
				smallest_angle = angle;
				free_point = false;
				add_point = true;
				to_be_added = corner;
			}
		}
	}
	anchor->ignored = false;
	
	if (add_point) {
		bool orientation = first != is_clockwise(anchor->x, anchor->y, to_be_added->x, to_be_added->y, cur->x, cur->y);
		grapple_points.insert(grapple_points.begin() + mp_index + first, {to_be_added, orientation});
		CornerList new_points;
		new_points.swap(contained);
		update_grapple(allCorners, new_points, contained, prev, first);
	} else if (free_point) {
		grapple_points.erase(grapple_points.begin() + mp_index + dir);
		Vector2D new_prev = get_line_intersection(prev.x, prev.y, cur->x, cur->y, anchor->x, anchor->y, prev_anchor->x, prev_anchor->y);
		contained.clear();
		anchor->ignored = true;
		update_grapple(allCorners, allCorners, contained, new_prev, first);
		anchor->ignored = false;
	}
}
