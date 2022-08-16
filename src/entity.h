#ifndef ENTITY_00_H
#define ENTITY_00_H
#include <string>
#include <stdio.h>
#include "utilities.h"
#include "texture.h"

class Entity {
	public:
		Entity(const double x, const double y, const double dx, const double dy, const int w, const int h) :
			pos(x, y), vel(dx, dy), width(w), height(h) {};
		
		/**
		 * Loads the texture of this entity from a file.
		 */
		void load_texture(std::string texture_path);
		
		/**
		 * Tick for the entity called every frame. The default implementation moves the entity by
		 * the velocity.
		 */
		virtual void tick(const double delta, const TileMap &tilemap);
		
		
		/**
		 * Renders the texture of this entity, considering the camera location.
		 */
		void render(const int cameraY);
		
		
		/** 
		 * Adds (dx, dy) to the velocity of this entity.
		 */
		void add_velocity(const double dx, const double dy);
		
		
		/**
		 * Returns true if this entity is standing on ground.
		 */
		bool on_ground(const TileMap &tilemap) const;
		

		Vector2D &get_velocity();
		

	protected:
		/**
		 * Protected constructor for subclasses.
		 */
		Entity() {};
		
		Vector2D pos;
		Vector2D vel;
		
		int width, height;
	
	private:
		Texture texture;
		/**
		* Tries to move the entity by (dx, dy), stopping if a wall is in the way. 
		* Requires that -tilesize < dx < tilesize and -tilesize < dy < tilesize
		*/
		void try_move(const double dx, const double dy, const int tilesize, const TileMap &tilemap);
		
	
	
};

class Player : public Entity {
	public:
		Player() {};
		
		void init(const double x, const double y, const int w, const int h);
		
};
#endif