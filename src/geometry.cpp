#include "geometry.h"


Triangle::Triangle(double x0, double y0, double x1, double y1, double x2, double y2) {
	double d1 = std::abs(y0 - y1);
	area2 = (-y1 * x2 + y0 * (-x1 + x2) + x0 * (y1 - y2) + x1 * y2);
	if (area2 < 0) {
		area2 *= -1;
		s1 = x0 * y2 - y0 * x2;
		s2 = y0 - y2;
		s3 = x2 - x0;
		t1 = y0 * x1 - x0 * y1;
		t2 = y1 - y0;
		t3 = x0 - x1;
	} else {
		s1 = y0 * x2 - x0 * y2;
		s2 = y2 - y0;
		s3 = x0 - x2;
		t1 = x0 * y1 - y0 * x1;
		t2 = y0 - y1;
		t3 = x1 - x0;
	}
}

bool Triangle::contains_point(double x, double y) const
{
	double sp = s1 + s2 * x + s3 * y;
	double tp = t1 + t2 * x + t3 * y;
	return sp >= 0 && tp >= 0 && area2 >= sp + tp;
}


bool is_clockwise(double x0, double y0, double x1, double y1, double x2, double y2) {
	return (-y1 * x2 + y0 * (-x1 + x2) + x0 * (y1 - y2) + x1 * y2) > 0;
}

double get_angle(double x0, double y0, double x1, double y1, double x2, double y2) {
	double dx1 = x0 - x1;
	double dy1 = y0 - y1;
	double dx2 = x2 - x1;
	double dy2 = y2 - y1;
	double len1 = std::sqrt(dx1 * dx1 + dy1 * dy1);
	double len2 = std::sqrt(dx2 * dx2 + dy2 * dy2);
	return std::acos(dx1 / len1 * dx2 / len2 + dy1 / len1 * dy2 /len2);
}

double get_angle(double v0x, double v0y, double v1x, double v1y) {
	double len1 = std::sqrt(v0x * v0x + v0y * v0y);
	double len2 = std::sqrt(v1x * v1x + v1y * v1y);
	return std::acos(v0x / len1 * v1x / len2 + v0y / len1 * v1y /len2);
}

Vector2D get_line_intersection(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
	double denom = (double)((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
	if (denom == 0) {
		printf("Warning: 0!");
		return {x1, y1};
	}
	double x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / denom;
	double y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / denom;
	return {x, y};
}