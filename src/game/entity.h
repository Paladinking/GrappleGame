#ifndef ENTITY_00_H
#define ENTITY_00_H
#include <string>
#include <vector>
#include <memory>
#include <utility>
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
        Texture texture;
		
		static EntityTemplate* from_json(const JsonObject& obj);
};

class PlayerTemplate : public EntityTemplate {
	public:
		PlayerTemplate(const int w, const int h, const int hp, Texture&& t, Texture&& gt)
            : EntityTemplate(w, h, std::move(t)), hp(hp), grappleTexture(std::move(gt)) {};

		const Texture grappleTexture;
        const int hp;
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
        virtual void init(EntityTemplate &entity_template);
		
		/**
		 * Tick for the entity called every frame. The default implementation moves the entity by
		 * the velocity.
		 */
		virtual void tick(double delta, Level &level);
		
		
		/**
		 * Renders the texture of this entity, considering the camera location.
		 */
		virtual void render(int cameraY);
		
		/**
		 * Adds (dx, dy) to the acceleration of this entity.
		 */
		void add_acceleration(double dx, double dy);
		
		/**
		 * Adds (dx, dy) to the velocity of this entity.
		 */
		void add_velocity(double dx, double dy);
		
		/**
		 * Sets the position of this entity to (x, y).
		 */
		void set_position(double x, double y);

        /**
         * Hurts this entity for damage.
         */
        virtual void hurt(int damage);
		
		/**
		 * Returns true if this entity is standing on ground.
		 */
		[[nodiscard]] bool on_ground(const Level &level) const;

        /**
         * Returns true if this entity is dead
         */
		[[nodiscard]] virtual bool is_alive() const;

		/**
		 * Returns the velocity vector of this entity.
		 */
		[[nodiscard]] const Vector2D &get_velocity() const;
		
		/**
		 * Returns the position of this entity, read only.
		 */
		[[nodiscard]] const Vector2D &get_position() const;

	protected:
		/**
		 * Protected constructor for subclasses.
		 */
		Entity() = default;
		/**
		* Tries to move the entity by (dx, dy), stopping if a wall is in the way. 
		* Requires that -tile_size < dx < tile_size and -tile_size < dy < tile_size
		*/
		void try_move(double dx, double dy, int tile_size, const Level &level);
		
		Vector2D pos;
		Vector2D vel;
		Vector2D acc;
		
		int width = 0, height = 0;

    protected:
		// Textures are owned by the game, not the entities.
        Texture* texture = nullptr;
		
	
	
};

class GrapplePoint {
	public:
		std::shared_ptr<Corner> corner;
		bool orientation;
};


class Player : public Entity {
	public:
		~Player() override;
		Player() = default;

		void init(EntityTemplate &entity_template) override;

		void render(int cameraY) override;

		void tick(double delta, Level &level) override;

        void hurt(int damage) override;

        bool is_alive() const override;

		void fire_grapple(int target_x, int target_y);

		void return_grapple();

		void jump();

		void set_pull(bool pull);

		void set_release(bool release);

	private:
		
		typedef std::vector<std::shared_ptr<Corner>> CornerList;
		
		void place_grapple(double x, double y, double dx, double dy, int tile_size, CornerList &corners);
		
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
		
		const Texture* grapple_hook = nullptr;
		
		GrapplingMode grappling_mode = UNUSED;

		double grapple_length = 0.0;
		double grapple_max_len = 0.0;

        int hp = 0;

        double inv_time = 0.0;
		
		bool pull = false, release = false, is_on_ground = false;

		Vector2D grapple_vel;
		
		std::shared_ptr<Corner> hook;
		std::shared_ptr<Corner> center_point;

		std::vector<GrapplePoint> grapple_points;
		
};
#endif