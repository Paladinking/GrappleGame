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
	
	Corner prev_pos = {(pos.x + width / 2), (pos.y + height / 2)};
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
	for (auto it = grapple_points.rbegin(); it != grapple_points.rend(); it++) 
	{
		SDL_RenderDrawLine(gRenderer, prev_pos.x, prev_pos.y - cameraY, it->corner->x, it->corner->y - cameraY);
		prev_pos = *(it->corner);
	}

	grapple_hook.render(grapple_point->x - 2, grapple_point->y - cameraY - 2);
}




void Player::tick(const double delta, const TileMap &tilemap, CornerList &corners)
{
	Vector2D old_pos = {pos.x + width / 2, pos.y + height / 2};
	Entity::tick(delta, tilemap, corners);
	
	if (grappling_mode == TRAVELING || (grappling_mode == PLACED && (vel.x != 0 || vel.y != 0))) 
	{
		CornerList contained;
		update_grapple(corners, corners, contained, {pos.x + width / 2, pos.y + height / 2}, old_pos);
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
		double new_x = grapple_point->x, new_y = grapple_point->y;
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
		Vector2D prev = {grapple_point->x, grapple_point->y};
		grapple_point->x = new_x;
		grapple_point->y = new_y;
		CornerList contained;
		update_grapple2(corners, corners, contained, prev);
	} else if (grappling_mode == PLACED)
	{
		
	}
}

void Player::place_grapple(const double x, const double y, const double dx, const double dy, const int tilesize, CornerList &corners) 
{
	grappling_mode = PLACED;
	int prev_tile_x  = (x - dx) / tilesize;
	int prev_tile_y  = (y - dy) / tilesize;
	int tile_x = x / tilesize;
	int tile_y = y / tilesize;
	Vector2D prev = {grapple_point->x, grapple_point->y};
	grapple_point->x = (((int)x) / tilesize + 0.5 + prev_tile_x - tile_x) * tilesize;
	grapple_point->y = (((int)y) / tilesize + 0.5 + prev_tile_y - tile_y) * tilesize;
	CornerList contained;
	update_grapple2(corners, corners, contained, prev);
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
		grapple_point = std::shared_ptr<Corner>(new Corner(pos.x + width / 2, pos.y + height / 2));
		grapple_points.push_back({grapple_point, false});
	}
	else if (grappling_mode != TRAVELING)
	{
		grapple_points.clear();
		grappling_mode = UNUSED;
	}
}

void Player::update_grapple(CornerList &allCorners, CornerList &corners, CornerList &contained, Vector2D cur, Vector2D prev)
{
	int anchor_index = grapple_points.size() - 1;
	GrapplePoint &anchorPoint = grapple_points[anchor_index];
	std::shared_ptr<Corner> anchor = anchorPoint.corner;
	bool free_point = false, add_point = false;
	double smallest_angle = 100.0; // Invalidly big angle
	if (anchor_index > 0) {
		std::shared_ptr<Corner> prev_anchor = grapple_points[anchor_index - 1].corner;
		bool orientation = is_clockwise(prev_anchor->x, prev_anchor->y, anchor->x, anchor->y, cur.x, cur.y);
		if ((orientation && !anchorPoint.orientation) || (!orientation && anchorPoint.orientation))
		{
			smallest_angle = get_angle(anchor->x - prev_anchor->x, anchor->y - prev_anchor->y, prev.x - anchor->x, prev.y - anchor->y);
			free_point  = true;
		}
	}
	Triangle t = {prev.x, prev.y, cur.x, cur.y, anchor->x, anchor->y};
	anchor->ignored = true;
	for (std::shared_ptr<Corner> &corner : corners) {
		if (!corner->ignored && t.contains_point(corner->x, corner->y)) {
			contained.push_back(corner);
		}
	}

	anchor->ignored = false;
	std::shared_ptr<Corner> to_be_added;

	for (std::shared_ptr<Corner> &corner : contained) {
		double angle = get_angle(prev.x, prev.y, anchor->x, anchor->y, corner->x, corner->y);
		if (angle < smallest_angle) {
			smallest_angle = angle;
			free_point = false;
			add_point = true;
			to_be_added = corner;
		}
	}
	if (add_point) {
		bool orientation = is_clockwise(anchor->x, anchor->y, to_be_added->x, to_be_added->y, cur.x, cur.y);
		grapple_points.push_back({to_be_added, orientation});
		CornerList new_points;
		new_points.swap(contained);
		update_grapple(allCorners, new_points, contained, cur, prev);
	} else if (free_point) {
		grapple_points.pop_back();
		std::shared_ptr<Corner> &new_anchor = grapple_points[grapple_points.size() - 1].corner;
		Vector2D new_prev = get_line_intersection(prev.x, prev.y, cur.x, cur.y, anchor->x, anchor->y, new_anchor->x, new_anchor->y);
		contained.clear();
		anchor->ignored = true;
		update_grapple(allCorners, allCorners, contained, cur, new_prev);
		anchor->ignored = false;
	}
}

// TODO: combine with update_grapple()...
void Player::update_grapple2(CornerList &allCorners, CornerList &corners, CornerList &contained, Vector2D prev)
{
	
	Vector2D anchor;
	double prev_anchor_x, prev_anchor_y;
	
	bool free_point = false, add_point = false;
	double smallest_angle = 100.0;
	
	if (grapple_points.size() > 1) {
		GrapplePoint &anchorPoint = grapple_points[1];
		anchorPoint.corner->ignored = true;
		anchor.x = anchorPoint.corner->x;
		anchor.y = anchorPoint.corner->y;
		if (grapple_points.size() > 2) {
			std::shared_ptr<Corner> prev_anchor = grapple_points[2].corner;
			prev_anchor_x = prev_anchor->x;
			prev_anchor_y = prev_anchor->y;
		} else {
			prev_anchor_x = pos.x + width / 2;
			prev_anchor_y = pos.y + height / 2;
		}
		bool orientation = is_clockwise(grapple_point->x, grapple_point->y, anchor.x, anchor.y, prev_anchor_x, prev_anchor_y);
		if (orientation != anchorPoint.orientation) {
			smallest_angle = get_angle(anchor.x - prev_anchor_x, anchor.y - prev_anchor_y, prev.x - anchor.x, prev.y - anchor.y);
			free_point = true;
		}
	} else {
		anchor.x = pos.x + width / 2;
		anchor.y = pos.y + height / 2;
	}
	
	Triangle t = {prev.x, prev.y, grapple_point->x, grapple_point->y, anchor.x, anchor.y};
	for(std::shared_ptr<Corner> &corner : corners) {
		if (!corner->ignored && t.contains_point(corner->x, corner->y)) {
			contained.push_back(corner);
		}
	}
	
	if (grapple_points.size() > 1) {
		grapple_points[1].corner->ignored = false;
	}
	std::shared_ptr<Corner> to_be_added;
	for (std::shared_ptr<Corner> &corner : contained) {
		double angle = get_angle(prev.x, prev.y, anchor.x, anchor.y, corner->x, corner->y);
		if (angle < smallest_angle) {
			smallest_angle = angle;
			free_point = false;
			add_point = true;
			to_be_added = corner;
		}
	}
	
	if (add_point) {
		bool orientation = is_clockwise(grapple_point->x, grapple_point->y, to_be_added->x, to_be_added->y, anchor.x, anchor.y);
		grapple_points.insert(grapple_points.begin() + 1, {to_be_added, orientation});
		CornerList new_points;
		new_points.swap(contained);
		update_grapple2(allCorners, new_points, contained, prev);
	} else if (free_point) {
		grapple_points.erase(grapple_points.begin() + 1);
		Vector2D new_prev = get_line_intersection(prev.x, prev.y, grapple_point->x, grapple_point->y, anchor.x, anchor.y, prev_anchor_x, prev_anchor_y);
		contained.clear();
		grapple_points[1].corner->ignored = true;
		update_grapple2(allCorners, allCorners, contained, new_prev);
		grapple_points[1].corner->ignored = false;
	}
}
