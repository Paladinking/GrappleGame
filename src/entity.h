#ifndef ENTITY_00_H
#define ENTITY_00_H
#include <string>
#include <vector>
#include <memory>
#include <stdio.h>
#include "utilities.h"
#include "level.h"
#include "texture.h"

class Entity {
	public:
		Entity(const double x, const double y, const double dx, const double dy, const int w, const int h) :
			pos(x, y), vel(dx, dy), width(w), height(h) {};
			
		/**
		 * Destructor for freeing texture.
		 */
		virtual ~Entity();
		
		/**
		 * Loads the texture of this entity from a file.
		 */
		void load_texture(std::string texture_path);
		
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
		 * Adds (dx, dy) to the velocity of this entity.
		 */
		void add_velocity(const double dx, const double dy);
		
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
		
		int width, height;
	
	private:
		Texture texture;
		
	
	
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
		
		void init(const double x, const double y, const int w, const int h);
		
		virtual void render(const int cameraY) override;
		
		virtual void tick(const double delta, Level &level) override;
		
		void fire_grapple(const int target_x, const int target_y);
		
		void return_grapple();
		
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
		
		Texture grapple_hook;
		
		GrapplingMode grappling_mode = UNUSED;

		double grapple_length;
		double grapple_max_len = GRAPPLE_LENGTH;
		
		bool pull, release;

		Vector2D grapple_vel;
		
		std::shared_ptr<Corner> hook;
		std::shared_ptr<Corner> center_point;

		std::vector<GrapplePoint> grapple_points;
		
};
#endif