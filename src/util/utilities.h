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
template<class T>
class TileMap {
	public:
		TileMap(const T out_of_bounds) : out_of_bounds(out_of_bounds) {}

		TileMap(const int tile_size, const int width, const int height, const T out_of_bounds) : tile_size(tile_size), width(width), height(height), out_of_bounds(out_of_bounds) {}

		/**
		 * Returns true if a tile contained in the pixel rectangle (x, y, w, h) contains tile.
		 */
		bool has_tile_rect(const int x, const int y, const int w, const int h, const T tile) const
		{
			for (int i = x / tile_size; i <= (x + w - 1) / tile_size; i++) 
			{
				for (int j = y / tile_size; j <= (y + h - 1) / tile_size; j++)
				{
					if (get_tile(i, j) == tile)
					{
						return true;
					}
				}					
			}
			return false;
		}

		/**
		 * Returns true if some point at a vertical line has the tile tile.
		 */
		bool has_tile_line_v(const int x_tile, const double y_start, const double length, const T tile) const
		{
			for (int i = static_cast<int>(std::floor(y_start / tile_size)); i <= (y_start + length - 1) / tile_size; ++i)
			{
				if (get_tile(x_tile, i) == tile) 
				{
					return true;
				}
			}
			return false;
		}

		/**
		 * Returns true if some point at a horizontal line has the tile tile.
		 */
		bool has_tile_line_h(const int y_tile, const double x_start, const double length, const T tile) const
		{
			for (int i = static_cast<int>(std::floor(x_start / tile_size)); i <= (x_start + length - 1) / tile_size; ++i)
			{
				if (get_tile(i, y_tile) == tile)
				{
					return true;
				}
			}
			return false;
		}

		/**
		 * Returns the tile at pixel (x, y), returning out_of_bounds if the pixel is out of bounds.
		 */
		T get_tile_pixel(const int x, const int y) const {
			return get_tile(x / tile_size, y / tile_size);
		}
		T get_tile_pixel(const double x, const double y) const {
			return get_tile(static_cast<int>(std::floor(x / tile_size)), static_cast<int>(std::floor(y / tile_size)));
		}

		/**
		 * Returns the tile (x, y), returning out_of_bounds if tile is out of bounds.
		 */
		T get_tile(const int x, const int y) const {
			if (x < 0 || x >= width || y < 0 || y >= height)
			{
				return out_of_bounds;
			}
			return map[x + width * y];
		}
		
		/**
		 * Returns the value of the tile at (x, y) same as get_tile without bounds check.
		 */
		T at(const int x, const int y) const {
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
		 
		 void set_tilesize(const int tilesize) {
			 tile_size = tilesize;
		 }
		 
	protected:
		std::unique_ptr<T[]> map;
		
		int width = 0, height = 0;
		
		int tile_size = 1;
		
	private:
		const T out_of_bounds;
		
};

#endif