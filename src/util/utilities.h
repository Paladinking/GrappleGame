#ifndef UTILITIES_00_H
#define UTILITIES_00_H
#include <memory>
#include <cmath>
#include <vector>
#include <string>

/**
 * Class containing a 2-dimensional vector using double.
 */
class Vector2D {
	public:
		/**
		 * Creates a new vector.
		 */
		Vector2D() : x(0.0), y(0.0) {}
		Vector2D(double x, double y) : x(x), y(y) {}
		
		/**
		 * Gets the length of this vector.
		 */
		double length() const
		{
			return std::sqrt(x * x + y * y);
		}
		
		
		/**
		 * Gets the squared length of this vector.
		 */
		double length_squared() const
		{
			return x * x + y * y;
		}
		
		/**
		 * Gets the distance from this vector to other.
		 */
		double distance(const Vector2D &other) const 
		{
			return std::sqrt((other.x - x) * (other.x - x) + (other.y - y) * (other.y - y));
		}
		
		/**
		 * Gets the squared distance from this vector to other.
		 */
		double distance_squared(const Vector2D &other) const
		{
			return (other.x - x) * (other.x - x) + (other.y - y) * (other.y - y);
		}
		
		/**
		 * Adds this vector to other.
		 */
		void add(Vector2D &other) 
		{
			x += other.x;
			y += other.y;
		}
		
		/**
		 * Adds this vector to other using a scaling factor.
		 */
		void add_scaled(Vector2D &other, double factor) 
		{
			x += other.x * factor;
			y += other.y * factor;
		}
		
		/**
		 * Scales this vector.
		 */
		void scale(double factor) 
		{
			x *= factor;
			y *= factor;
		}
		
		/**
		 * Nomalizes this vector.
		 */
		void normalize() 
		{
			double len = length();
			if (len == 0.0) {
				return;
			}
			x /= len;
			y /= len;
		}
		
		/**
		 * Returns the dot-product of this vector and other.
		 */
		double dot(Vector2D &other)
		{
			return x * other.x + y * other.y;
		}

		/**
		 * Reduces the length of this vector by amount, stopping at 0.
		 */
		void decreace(double amount) {
			double len = length();
			double diff = len - amount;
			if (diff <= 0) {
				x = 0;
				y = 0;
				return;
			}
			x = (x / len) * diff;
			y = (y / len) * diff;
		}

		/**
		 * Subtracts other vector from this.
		 */
		void subtract(Vector2D &other) {
			x -= other.x;
			y -= other.y;
		}

		double x, y;
};

/**
 * A tilemap for collisions.
 */
class TileMap {
	public:
		TileMap() {}

		TileMap(const int width, const int height) : width(width), height(height) {} 

		/**
		 * Returns true if a tile contained in the pixel rectangle (x, y, w, h) contains a blocked tile.
		 */
		bool is_blocked_rect(int x, int y, int w, int h) const
		{
			for (int i = x / tile_size; i <= (x + w - 1) / tile_size; i++) 
			{
				for (int j = y / tile_size; j <= (y + h - 1) / tile_size; j++)
				{
					if (is_blocked(i, j))
					{
						return true;
					}
				}					
			}
			return false;
		}

		/**
		 * Returns true if some point at a vertical line is blocked.
		 */
		bool is_blocked_line_v(int x_tile, double y_start, double length) const
		{
			for (int i = static_cast<int>(y_start) / tile_size; i <= (y_start + length - 1) / tile_size; ++i)
			{
				if (is_blocked(x_tile, i)) 
				{
					return true;
				}
			}
			return false;
		}

		/**
		 * Returns true if some point at a horizontal line is blocked.
		 */
		bool is_blocked_line_h(int y_tile, double x_start, double length) const
		{
			for (int i = static_cast<int>(x_start) / tile_size; i <= (x_start + length - 1) / tile_size; ++i)
			{
				if (is_blocked(i, y_tile))
				{
					return true;
				}
			}
			return false;
		}

		/**
		 * Returns true if the pixel (x, y) is blocked or not, returning true if the pixel is out of bounds.
		 */
		bool is_blocked_pixel(const int x, const int y) const {
			return is_blocked(x / tile_size, y / tile_size);
		}
		bool is_blocked_pixel(const double x, const double y) const {
			return is_blocked(static_cast<int>(x) / tile_size, static_cast<int>(y) / tile_size);
		}
		/**
		 * Returns true if the tile (x, y) is blocked or not, returning true if tile is out of bounds.
		 */
		bool is_blocked(const int x, const int y) const {
			if (x < 0 || x >= width || y < 0 || y >= height)
			{
				return true;
			}
			return map[x + width * y];
		}
		
		/**
		 * Returns the value of the tile at (x, y) same as is_blocked without bounds check.
		 */
		bool get_tile(const int x, const int y) const {
			return map[x + width * y];
		}
		
		/**
		 * Returns the height of the tilemap.
		 */
		int get_width() const {
			return width;
		}
		 
		/**
		 * Returns the width of the tilemap.
		 */
		int get_height() const {
			return height;
		}
		
		/**
		 * Returns the tile_size.
		 */
		 int get_tilesize() const {
			 return tile_size;
		 }
		 
		 void set_tilesize(int tilesize) {
			 tile_size = tilesize;
		 }
		 
	protected:
		std::unique_ptr<bool[]> map;
		
		int width = 0, height = 0;
		
		int tile_size = 1;
		
};

#endif