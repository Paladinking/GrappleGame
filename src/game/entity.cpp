#include "entity.h"

#include <memory>
#include "engine/engine.h"
#include "util/geometry.h"
#include "config.h"

constexpr double GRAVITY_ACCELERATION = 3000.0;
constexpr double FRICTION_FACTOR = 700.0;
constexpr double AIR_RES_FACTOR = 0.004;

constexpr int GRAPPLE_LENGTH = 600;
constexpr double GRAPPLE_SPEED = 1300.0;
constexpr double GRAPPLE_PULL = 5000.0;
constexpr double GRAPPLE_RELEASE = 200.0;
constexpr double JUMP_VEL = 800.0;

constexpr int SPIKE_DAMAGE = 5;
constexpr double INV_DURATION = 0.6;

void texture_form_template(Texture& t, const JsonObject& text) {
	if (
		!text.has_key_of_type<std::string>("Path") ||
		!text.has_key_of_type<int>("Width") || 
		!text.has_key_of_type<int>("Height")
	) {
		throw json_exception("Bad entity template texture");
	}
	t.load_from_file(config::get_asset_path(text.get<std::string>("Path")));
	t.set_dimensions(
		text.get<int>("Width"),
		text.get<int>("Height")
	);
}

EntityTemplate* EntityTemplate::from_json(const JsonObject& obj) {
	if (
		!obj.has_key_of_type<std::string>("Type") || 
		!obj.has_key_of_type<JsonObject>("Texture") || 
		!obj.has_key_of_type<int>("Width") || 
		!obj.has_key_of_type<int>("Height")
	) {
		throw json_exception("Bad entity template");
	}
	const JsonObject& text = obj.get<JsonObject>("Texture");
	int width = obj.get<int>("Width");
	int height = obj.get<int>("Height");

	const std::string& type = obj.get<std::string>("Type");
	if (type == "Player") {
		if (!obj.has_key_of_type<JsonObject>("HookTexture") || !obj.has_key_of_type<int>("Hp"))
            throw json_exception("Bad player template");
        Texture grappleTexture, texture;
		texture_form_template(texture, text);
		texture_form_template(grappleTexture, obj.get<JsonObject>("HookTexture"));
        const int hp = obj.get<int>("Hp");
		return new PlayerTemplate(width, height, hp, std::move(texture), std::move(grappleTexture));
	} else {
		Texture texture;
		texture_form_template(texture, text);
		return new EntityTemplate(width, height, std::move(texture));
	}
}

Entity::~Entity() = default;


void Entity::init(EntityTemplate &entity_template) {
	width = entity_template.w;
	height = entity_template.h;
	texture = &entity_template.texture;
}

void Entity::tick(const double delta, Level& level) 
{
	int tile_size = level.get_tile_size();
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
		Vector2D move_step = {(to_move.x / len) * tile_size, (to_move.y / len) * tile_size};
		int steps = static_cast<int>(len / tile_size);
		to_move.x -= steps * move_step.x;
		to_move.y -= steps * move_step.y;
		for (; steps > 0; steps--) 
		{
			try_move(move_step.x, move_step.y, tile_size, level);
		}
		try_move(to_move.x, to_move.y, tile_size, level);
	}
}

void Entity::render(const int cameraY) 
{
	int x = static_cast<int>(pos.x), y = static_cast<int>(pos.y - cameraY);
	texture->render(x, y);
}




void Entity::try_move(const double dx, const double dy, int tile_size, const Level &level)
{
	double new_x = pos.x, new_y = pos.y;
	new_x += dx;

	int x_tile = static_cast<int>(std::floor((dx > 0 ? (new_x + width - 1) : new_x) / tile_size));
	if (level.has_tile_line_v(x_tile, new_y, height, Tile::BLOCKED)) 
	{
		vel.x = 0;
		new_x = dx > 0 ? (x_tile * tile_size - width) : (x_tile + 1) * tile_size;
	} else if (level.has_tile_line_v(x_tile, new_y, height, Tile::SPIKES)) {
        this->hurt(SPIKE_DAMAGE);
    }

	new_y += dy;
	int y_tile = static_cast<int>(std::floor((dy > 0 ? (new_y + height - 1) : new_y) / tile_size));
	if (level.has_tile_line_h(y_tile, new_x, width, Tile::BLOCKED))
	{
		vel.y = 0;
		new_y = dy > 0 ? y_tile * tile_size - height : (y_tile + 1) * tile_size;
	} else if (level.has_tile_line_h(y_tile, new_x, width, Tile::SPIKES)) {
        this->hurt(SPIKE_DAMAGE);
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

void Entity::set_position(const double x, const double y) {
	pos.x = x;
	pos.y = y;
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
	int tile_size = level.get_tile_size();
	int y_tile =  static_cast<int>((pos.y + height) / tile_size);
	for (int i = static_cast<int>(pos.x / tile_size); i <= (pos.x + width - 1) / tile_size; ++i) //level.line...?
	{
		if (level.get_tile(i, y_tile) == Tile::BLOCKED)
		{
			return true;
		}
	}
	return false;
}

void Entity::hurt(int damage) {}

bool Entity::is_alive() const {
    return true;
}

Player::~Player() = default;

void Player::init(EntityTemplate &entity_template) {
	Entity::init(entity_template);
	const PlayerTemplate& pt = static_cast<const PlayerTemplate&>(entity_template); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
	vel.x = 0;
	vel.y = 0;
    hp = pt.hp;
	grapple_max_len = GRAPPLE_LENGTH;
	grapple_hook = &pt.grappleTexture;
    inv_time = 0.0;
}

void Player::render(const int cameraY) 
{
    Entity::render(cameraY);
    if (grappling_mode == UNUSED) return;
	
	Corner prev_pos = {(pos.x + width / 2), (pos.y + height / 2)}; // NOLINT(bugprone-integer-division)
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

	grapple_hook->render(static_cast<int>(hook->x) - 2, static_cast<int>(hook->y - cameraY) - 2);
}



void Player::tick(const double delta, Level &level)
{
    if (inv_time > 0.0) {
        inv_time -= delta;
        texture->set_color_mod(128, 255, 0);
    } else {
        texture->set_color_mod(255, 255, 255);
    }
	Vector2D old_pos = {pos.x + width / 2, pos.y + height / 2}; // NOLINT(bugprone-integer-division)
	if (is_on_ground) {
		double factor = delta * FRICTION_FACTOR;
		if (vel.x > 0) {
			acc.x += factor > vel.x ? -vel.x : -FRICTION_FACTOR;
		} else {
			acc.x += factor > -vel.x ? -vel.x : FRICTION_FACTOR;
		}
	}
	acc.y += GRAVITY_ACCELERATION;
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
			// If after a movement, the length of the grapple exceeds the maximum, the player has to be pulled by the rope.
			// This is done by projecting the vector onto the orthogonal vector to the line vector
			// This is not perfect, the regular movement should be made up to the point reaching the max_length
			// Current solution can lead to stuttering on low framerates
			double angle = get_angle(line_vector.x, line_vector.y, to_move.x, to_move.y);
			if (angle > PI_HALF) {
				const double rotated_x = -line_vector.y, rotated_y = line_vector.x;
				const double vel_scalar = (rotated_x * vel.x + rotated_y * vel.y) /
						(rotated_x * rotated_x + rotated_y * rotated_y);
				vel.x = vel_scalar * rotated_x;
				vel.y = vel_scalar * rotated_y;
				const double desired_length = grapple_max_len - grapple_length + prev_len;

				to_move.x = -(center_point->x + (new_line_vector.x / new_len) * desired_length - anchor->x);
				to_move.y = -(center_point->y + (new_line_vector.y / new_len) * desired_length - anchor->y);
			}
		}
	}


	int tile_size = level.get_tile_size();
	double len = to_move.length();
	if (len > 0) 
	{
		// Get vector in movement direction of length TILE_SIZE.
		// Calculate how many of those steps needed for full movement + the remainder movement.
		// Then try_move won't skip any potential blocked tiles, even at low framerates.
		Vector2D move_step = {(to_move.x / len) * tile_size, (to_move.y / len) * tile_size};
		int steps = static_cast<int>(len / tile_size);
		to_move.x -= steps * move_step.x;
		to_move.y -= steps * move_step.y;
		for (; steps > 0; steps--) 
		{
			try_move(move_step.x, move_step.y, tile_size, level);
		}
		try_move(to_move.x, to_move.y, tile_size, level);
	}
	is_on_ground = on_ground(level);

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
		Vector2D to_move_grapple = {grapple_vel.x * delta, grapple_vel.y * delta};
		
		
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

            to_move_grapple.x = -(hook->x + (line_vector.x / new_len) * (grapple_max_len - (grapple_length - prev_len)) - anchor->x);
            to_move_grapple.y = -(hook->y + (line_vector.y / new_len) * (grapple_max_len - (grapple_length - prev_len)) - anchor->y);
			
		}
		
		double new_x = hook->x, new_y = hook->y;
		double to_move_grapple_len = to_move_grapple.length();
		Vector2D move_step = {(to_move_grapple.x / to_move_grapple_len), (to_move_grapple.y / to_move_grapple_len)};
		int steps = static_cast<int>(to_move_grapple_len);
        to_move_grapple.x -= steps * move_step.x;
        to_move_grapple.y -= steps * move_step.y;
		for (; steps > 0; steps--) 
		{
			new_x += move_step.x;
			new_y += move_step.y;
			if (level.get_tile_pixel(new_x, new_y) == Tile::BLOCKED)
			{
				place_grapple(new_x, new_y, move_step.x, move_step.y, tile_size, level.get_corners());
				return;
			}
		}
		new_x += to_move_grapple.x;
		new_y += to_move_grapple.y;
		if (level.get_tile_pixel(new_x, new_y) == Tile::BLOCKED) {
			place_grapple(new_x, new_y, to_move_grapple.x, to_move_grapple.y, tile_size, level.get_corners());
			return;
		}
		Vector2D prev = {hook->x, hook->y};
		hook->x = new_x;
		hook->y = new_y;
		update_grapple(level.get_corners(), prev, true);
	} 
}

void Player::place_grapple(const double x, const double y, const double dx, const double dy, const int tile_size, CornerList &corners)
{
	grappling_mode = PLACED;
	int prev_tile_x  = static_cast<int>((x - dx) / tile_size);
	int prev_tile_y  = static_cast<int>((y - dy) / tile_size);
	int tile_x = static_cast<int>(x / tile_size);
	int tile_y = static_cast<int>(y / tile_size);
	Vector2D prev = {hook->x, hook->y};
	hook->x = (static_cast<int>(x) / tile_size + 0.5 + prev_tile_x - tile_x) * tile_size; // NOLINT(bugprone-integer-division)
	hook->y = (static_cast<int>(y) / tile_size + 0.5 + prev_tile_y - tile_y) * tile_size; // NOLINT(bugprone-integer-division)
	update_grapple(corners, prev, true);
}

void Player::return_grapple() {
	if (grappling_mode == TRAVELING ||  grappling_mode == PLACED) {
		grapple_points.clear();
		grappling_mode = UNUSED;
	}
}

void Player::jump() {
	if (is_on_ground) {
		vel.y = -JUMP_VEL;
	} else if (grappling_mode == PLACED && grapple_points[grapple_points.size() - 2].corner->y < center_point->y) {
		grapple_points.clear();
		grappling_mode = UNUSED;
		vel.y = -JUMP_VEL;
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
		grapple_vel.x = targetX - (pos.x + width  / 2); // NOLINT(bugprone-integer-division)
		grapple_vel.y = targetY - (pos.y + height / 2); // NOLINT(bugprone-integer-division)
		grapple_vel.normalize();
		grapple_vel.scale(GRAPPLE_SPEED);
		hook = std::make_shared<Corner>(pos.x + width / 2, pos.y + height / 2); // NOLINT(bugprone-integer-division)
		center_point = std::make_shared<Corner>(pos.x + width / 2, pos.y + height / 2); // NOLINT(bugprone-integer-division)
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
		mp_index = static_cast<int>(grapple_points.size()) - 1;
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
	// If the point is free, potentially it should be removed from the vector.
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

void Player::hurt(int damage) {
    if (inv_time <= 0) {
        hp -= damage;
        inv_time = INV_DURATION;
    }
}

bool Player::is_alive() const {
    return hp > 0;
}
