#ifndef ENTITY_00_H
#define ENTITY_00_H
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <stdio.h>
#include "util/utilities.h"
#include "engine/texture.h"
#include "level.h"
#include "globals.h"
#include "file/json.h"

class EntityTemplate {
	public:
		EntityTemplate(const int w, const int h, Texture&& t) : w(w), h(h), texture(std::move(t)) {};
	
		const int w;
		const int h;
		const Texture texture;
		
		static EntityTemplate* from_json(const JsonObject& obj);
};

class PlayerTemplate : public EntityTemplate {
	public:
		PlayerTemplate(const int w, const int h, Texture&& t, Texture&& gt) : EntityTemplate(w, h, std::move(t)), grappleTexture(std::move(gt)) {};

		const Texture grappleTexture;
};

class Entity {
	public:
		Entity(const double x, const double y, const double dx, const double dy, const int w, const int h) :
			pos(x, y), vel(dx, dy), width(w), height(h) {};
			
		/**
		 * Destructor for freeing texture.
		 */
		virtual ~Entity();

		/**
		 * Initializes this entity based on a template.
		 */
		virtual void init(const EntityTemplate& entity_template);
		
		/**
		 * Tick for the entity called every frame. The default implementation moves the entity by
		 * the velocity.
		 */
		virtual void tick(const double delta, Level &level);
		
		
		/**
		 * Renders the texture of this entity, considering the camera location.
		 */
		virtual void render(const int cameraY);
		
		/**
		 * Adds (dx, dy) to the acceleration of this entity.
		 */
		void add_acceleration(const double dx, const double dy);
		
		/**
		 * Adds (dx, dy) to the velocity of this entity.
		 */
		void add_velocity(const double dx, const double dy);
		
		/**
		 * Sets the position of this entity to (x, y).
		 */
		void set_position(const double x, const double y);
		
		/**
		 * Returns true if this entity is standing on ground.
		 */
		bool on_ground(const Level &level) const;
		
		/**
		 * Returns the velocity vector of this entity.
		 */
		const Vector2D &get_velocity() const;
		
		/**
		 * Returns the position of this entity, read only.
		 */
		const Vector2D &get_position() const;

	protected:
		/**
		 * Protected constructor for subclasses.
		 */
		Entity() {};
		/**
		* Tries to move the entity by (dx, dy), stopping if a wall is in the way. 
		* Requires that -tilesize < dx < tilesize and -tilesize < dy < tilesize
		*/
		void try_move(const double dx, const double dy, const int tilesize, const Level &level);
		
		Vector2D pos;
		Vector2D vel;
		Vector2D acc;
		
		int width, height;
	
	private:
		// Textures are owned by the game, not the entities.
		const Texture* texture;
		
	
	
};

class GrapplePoint {
	public:
		std::shared_ptr<Corner> corner;
		bool orientation;
};


class Player : public Entity {
	public:
		virtual ~Player();
		Player() {};

		virtual void init(const EntityTemplate& entity_template) override;

		virtual void render(const int cameraY) override;

		virtual void tick(const double delta, Level &level) override;

		void fire_grapple(const int target_x, const int target_y);

		void return_grapple();

		void jump();

		void set_pull(bool pull);

		void set_release(bool release);

	private:
		
		typedef std::vector<std::shared_ptr<Corner>> CornerList;
		
		void place_grapple(const double x, const double y, const double dx, const double dy, const int tilesize, CornerList &corners);
		
		/**
		 * Updates the grapple_points vector after either the first or last element has moved.
		 * The previous position is given as prev. Adds and removes points from grapple_points as needed.
		 */
		void update_grapple(CornerList &corners, Vector2D prev, bool first);
		 
		/**
		 * Recursive helper for updating grapple points.
		 */
		void update_grapple(CornerList &allCorners, CornerList &corners, CornerList &contained, Vector2D prev, bool first);
	
		enum GrapplingMode
		{
			UNUSED, TRAVELING, PLACED, PULLING, RETURNING
		};
		
		const Texture* grapple_hook;
		
		GrapplingMode grappling_mode = UNUSED;

		double grapple_length;
		double grapple_max_len;
		
		bool pull, release, is_on_ground;

		Vector2D grapple_vel;
		
		std::shared_ptr<Corner> hook;
		std::shared_ptr<Corner> center_point;

		std::vector<GrapplePoint> grapple_points;
		
};
#endif