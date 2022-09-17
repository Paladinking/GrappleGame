#ifndef GEOMETRY_00_H
#define GEOMETRY_00_H
#include "utilities.h"

class Triangle {
	private:
		double area2;
		double s1, s2, s3;
		double t1, t2, t3;

	public:
		Triangle(double x0, double y0, double x1, double y1, double x2, double y2);
		
		bool contains_point(double x, double y) const;
};

double length(double x1, double x2);

double distance(double x1, double y1, double x2, double y2);

bool is_clockwise(double x0, double y0, double x1, double y1, double x2, double y2);

double get_angle(double x0, double y0, double x1, double y1, double x2, double y2);

double get_angle(double v0x, double v0y, double v1x, double v1y);

Vector2D get_line_intersection(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);

#endif