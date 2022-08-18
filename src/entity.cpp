#include "entity.h"
#include "geometry.h"

Entity::~Entity() {}

void Entity::load_texture(std::string texture_path) 
{
	texture.load_from_file(texture_path.c_str());
	texture.set_dimensions(width, height);
}

void Entity::tick(const double delta, const TileMap &tilemap, std::vector<std::shared_ptr<Corner>> &corners) 
{
	vel.add_scaled(acc, delta);
	double x = pos.x + width / 2, y = pos.y + height / 2, dx = vel.x, dy = vel.y;
	printf("%f, %f, ", vel.x, vel.y);
	double delta_part = delta / 10.0;
	double moved_x = 0.0, moved_y = 0.0;
	for (int i = 0; i < 10; i++) {
		Vector2D d_acc = get_dynamic_acc(x, y, dx, dy);
		dx += d_acc.x * delta_part;
		dy += d_acc.y * delta_part;
		moved_x += dx * delta_part;
		moved_y += dy * delta_part;
		x += dx * delta_part;
		y += dy * delta_part;
		
	}
	printf("%f, %f\n", moved_x, moved_y);
	vel.x = dx;
	vel.y = dy;
	acc.x = 0.0;
	acc.y = 0.0;
	
	int tilesize = tilemap.get_tilesize();
	Vector2D to_move = {moved_x, moved_y};
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

void Entity::add_acceleration(const double dx, const double dy) 
{
	acc.x += dx;
	acc.y += dy;
}

void Entity::add_velocity(const double dx, const double dy) 
{
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
	
	Corner prev_pos = {(pos.x + width / 2), (pos.y + height / 2)};
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
	for (auto it = grapple_points.rbegin(); it != grapple_points.rend(); it++) 
	{
		SDL_RenderDrawLine(gRenderer, prev_pos.x, prev_pos.y - cameraY, it->corner->x, it->corner->y - cameraY);
		prev_pos = *(it->corner);
	}

	grapple_hook.render(hook->x - 2, hook->y - cameraY - 2);
}

Vector2D Player::get_dynamic_acc(double x, double y, double dx, double dy) {
	Vector2D acc = {0.0, 0.0};
	// Air resistance
	acc.x -= dx * std::abs(dx) * AIR_RES_FACTOR;
	acc.y -= dy * std::abs(dy) * AIR_RES_FACTOR;
	// Rope
	if (grappling_mode == PLACED)
	{
		int i = 0;
		double len = 0.0;
		for (; i + 3 < grapple_points.size(); ++i) {
			const auto &p1 = grapple_points[i].corner; 
			const auto &p2 = grapple_points[i + 1].corner; 
			len += std::sqrt((p1->x - p2->x)  * (p1->x - p2->x) + (p1->y - p2->y) * (p1->y - p2->y));
		}
		const auto &p = grapple_points[i].corner;
		len += std::sqrt((p->x - x)  * (p->x - x) + (p->y - y) * (p->y - y));
		double diff = len - GRAPPLE_LENGTH;
		if (diff > 0) {
			printf("+");
			GrapplePoint &anchor = grapple_points[grapple_points.size() - 2];
			Vector2D v = Vector2D(anchor.corner->x - x, anchor.corner->y - y);
			v.normalize();
			double size = GRAPPLE_FORCE * diff * diff;
			acc.add_scaled(v, size);
		}
	}
	return acc;
	
}


void Player::tick(const double delta, const TileMap &tilemap, CornerList &corners)
{
	Vector2D old_pos = {pos.x + width / 2, pos.y + height / 2};
	Entity::tick(delta, tilemap, corners);
	//printf("delta : %f\n", pos.y - old_pos.y + height / 2);

	if (grappling_mode == TRAVELING || (grappling_mode == PLACED && (vel.x != 0 || vel.y != 0))) 
	{
		center_point->x = pos.x + width / 2;
		center_point->y = pos.y + height / 2;
		update_grapple(corners, old_pos, false);
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
		double new_x = hook->x, new_y = hook->y;
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
				place_grapple(new_x, new_y, move_step.x, move_step.y, tilesize, corners);
				return;
			}
		}
		new_x += to_move.x;
		new_y += to_move.y;
		if (tilemap.is_blocked_pixel(new_x, new_y)) {
			place_grapple(new_x, new_y, to_move.x, to_move.y, tilesize, corners);
			return;
		}
		Vector2D prev = {hook->x, hook->y};
		hook->x = new_x;
		hook->y = new_y;
		update_grapple(corners, prev, true);
	} 
}

void Player::place_grapple(const double x, const double y, const double dx, const double dy, const int tilesize, CornerList &corners) 
{
	grappling_mode = PLACED;
	int prev_tile_x  = (x - dx) / tilesize;
	int prev_tile_y  = (y - dy) / tilesize;
	int tile_x = x / tilesize;
	int tile_y = y / tilesize;
	Vector2D prev = {hook->x, hook->y};
	hook->x = (((int)x) / tilesize + 0.5 + prev_tile_x - tile_x) * tilesize;
	hook->y = (((int)y) / tilesize + 0.5 + prev_tile_y - tile_y) * tilesize;
	update_grapple(corners, prev, true);
}

void Player::fire_grapple(const int targetX, const int targetY) 
{
	if (grappling_mode == UNUSED) 
	{
		grappling_mode = TRAVELING;
		pull = false;
		grapple_length = 0;
		grapple_vel.x = targetX - (pos.x + width  / 2);
		grapple_vel.y = targetY - (pos.y + height / 2);
		grapple_vel.normalize();
		grapple_vel.scale(GRAPPLE_SPEED);
		hook = std::shared_ptr<Corner>(new Corner(pos.x + width / 2, pos.y + height / 2));
		center_point = std::shared_ptr<Corner>(new Corner(pos.x + width / 2, pos.y + height / 2));
		grapple_points.push_back({hook, false});
		grapple_points.push_back({center_point, false});
	}
	else if (grappling_mode != TRAVELING)
	{
		grapple_points.clear();
		grappling_mode = UNUSED;
	}
}

void Player::toggle_pull() {
	pull = !pull;
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
	double smallest_angle = 100.0;
	
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
	
	// 
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
