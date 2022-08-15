#ifndef UTILITIES_00_H
#define UTILITIES_00_H
#include <math.h> 

class Vector2D {
	public:
		Vector2D(double x, double y) : x(x), y(y) {}
		
		double length() const
		{
			return sqrt(x * x + y * y);
		}
		
		double length_squared() const
		{
			return x * x + y * y;
		}
		
		double distance(const Vector2D &other) const 
		{
			return sqrt((other.x - x) * (other.x - x) + (other.y - y) * (other.y - y));
		}
		
		double distance_squared(const Vector2D &other) const
		{
			return (other.x - x) * (other.x - x) + (other.y - y) * (other.y - y);
		}
		
		void add(Vector2D &other) 
		{
			x += other.x;
			y += other.y;
		}
		
		void add_scaled(Vector2D &other, double factor) 
		{
			x += other.x * factor;
			y += other.y * factor;
		}
		
		void scale(double factor) 
		{
			x *= factor;
			y *= factor;
		}
		
		void normalize() 
		{
			double len = length();
			if (len == 0.0) {
				return;
			}
			x /= len;
			y /= len;
		}
		
		double dot(Vector2D &other)
		{
			return x * other.x + y * other.y;
		}
		
		// Makes it so that this->length() reduces by amount (stops at 0), amount >= 0
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
		
		void subtract(Vector2D &other) {
			x -= other.x;
			y -= other.y;
		}
		
		double x, y;
	
	
	
	
};



#endif