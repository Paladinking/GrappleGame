#ifndef TEST_GAME_00_H
#define TEST_GAME_00_H
#include <cmath>
#include <vector>

struct Point {
	int x, y;
	int orientation = -1;
};

bool is_clockwise(int x0, int y0, int x1, int y1, int x2, int y2) {
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

class Triangle {
	private:
		double area2;
		double s1, s2, s3;
		double t1, t2, t3;
		
	public:
		Triangle(int x0, int y0, int x1, int y1, int x2, int y2) {
			int d1 = std::abs(y0 - y1);
			
			
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
		
		bool contains_point(int x, int y) 
		{
			double sp = s1 + s2 * x + s3 * y;
			double tp = t1 + t2 * x + t3 * y;
			return sp >= 0 && tp >= 0 && area2 >= sp + tp;
		}
};

bool triangle_contains(Point p0, Point p1, Point p2, int x, int y) 
{
	double Area = 0.5 * (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
	double s = 1 / (2 * Area) * (p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * x + (p0.x - p2.x) * y);
	double t = 1 / (2 *Area) * (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * x + (p1.x - p0.x) * y);
	
	return s>0 && t>0 && 1-s-t>0;
} 

bool triangle_contains2(Point p0, Point p1, Point p2, int x, int y) 
{
	double Area = 0.5 * (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
	
	double sp = ((p0.y * p2.x - p0.x * p2.y) + (p2.y - p0.y) * x + (p0.x - p2.x) * y);
	double tp = (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * x + (p1.x - p0.x) * y);
	if (Area < 0)  {
		Area *= -1;
		sp *= -1;
		tp *= -1;
	};
	return sp > 0 && tp > 0 && Area > sp + tp;
} 


void add_containing(Triangle &t, std::vector<Point> &source, std::vector<Point> &dest, Point &ignored) {
	for (Point &p: source) {
		if ((p.x != ignored.x || p.y != ignored.y) && t.contains_point(p.x, p.y)) dest.push_back({p.x, p.y});
	}
}


bool free_anchor_point(Point &next, int anchor_index, std::vector<Point> &points) {
	if (anchor_index == 0) return false;
	Point &anchor = points[anchor_index];
	Point &prev_anchor = points[anchor_index - 1];
	bool orientation = is_clockwise(prev_anchor.x, prev_anchor.y, anchor.x, anchor.y, next.x, next.y);
	if ((!orientation && anchor.orientation == 1) || (orientation && anchor.orientation == 0)) {
		points.erase(points.begin() + anchor_index);
		return true;
	}
	return false;
}


class TestGame : public Game {
	public:
		TestGame() : Game(SCREEN_WIDTH, SCREEN_HEIGHT, "Test") {}
	protected:
		
		std::vector<Point> points;
		std::vector<Point> rope_points;
		Texture circle;
		Point p1 = {30, 300};
		Point prev = {-1, -1};
		Point cur = {-1, -1};
		
		virtual void init() override {
			points.push_back({10, 10});
			points.push_back({123, 42});
			points.push_back({600, 242});
			points.push_back({300, 300});
			points.push_back({123, 556});
			points.push_back({444, 232});
			points.push_back({232, 430});
			points.push_back({588, 333});
			circle.load_from_file("assets/ball.png");
			circle.set_dimensions(4, 4);
			
			rope_points.push_back({300, 500});
			
		}
		
		virtual void handle_mousepress(SDL_MouseButtonEvent e) override {
			if (e.button == SDL_BUTTON_MIDDLE){
				points.push_back({e.x, e.y});
				
				return;
			}
			if (prev.x == -1) {
				prev = {e.x, e.y};
				return;
			}
			prev = {cur.x, cur.y};
			cur = {e.x, e.y};
			int anchor_index = rope_points.size() - 1;
			Point anchor = rope_points[anchor_index];
			
			Triangle t = {prev.x, prev.y, cur.x, cur.y, anchor.x, anchor.y};
			std::vector<Point> contained;
			
			bool freed = false;
			
			Point to_ignore;
			
			add_containing(t, points, contained, anchor);
			if (contained.size() == 0) {
				Point next = {e.x, e.y};
				if (free_anchor_point(next, anchor_index, rope_points)) {
					Point &prev_anchor = rope_points[anchor_index -1];
					t = {anchor.x, anchor.y, cur.x, cur.y, prev_anchor.x, prev_anchor.y};
					to_ignore = anchor;
					anchor = prev_anchor;
					anchor_index--;
					contained.clear();
					add_containing(t, points, contained, anchor);
				} else {
					return;
				}
			}

			do {
				Point to_be_added = {-1, -1};
				double smallest_angle = 10.0;
				for (Point &p : contained) {
					if (p.x == to_ignore.x && p.y == to_ignore.y) continue;
					double angle = get_angle(prev.x, prev.y, anchor.x, anchor.y, p.x, p.y);
					if (angle < smallest_angle) {
						smallest_angle = angle;
						to_be_added = p;
					}
				}
				if (smallest_angle != 10.0) {
					to_be_added.orientation = is_clockwise(anchor.x, anchor.y, to_be_added.x, to_be_added.y, e.x, e.y);
					rope_points.push_back(to_be_added);
					std::vector<Point> new_contained;
					t = {prev.x, prev.y, cur.x, cur.y, to_be_added.x, to_be_added.y};
					add_containing(t, contained, new_contained, to_be_added);
					contained = new_contained;
				} else {
					break;
				}
			} while (true);
			
			if (anchor_index < rope_points.size() - 1) {
				free_anchor_point(rope_points[anchor_index + 1], anchor_index, rope_points);
			}
			
		
			
		}
		
		virtual void render() override {
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(gRenderer);
			
			
			for (const Point &point : points) {
				circle.render(point.x, point.y);
				
			}
			
			
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
			Point *prev_p = &prev;
			for (int i = rope_points.size() - 1; i >=0; --i) {
				
				Point &p = rope_points[i];
				SDL_RenderDrawLine(gRenderer, p.x, p.y, prev_p->x, prev_p->y);
				prev_p = &rope_points[i];
			}
			
			if (cur.x != -1 && prev.x != -1) {
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
				Point &p1 = rope_points[rope_points.size() - 1];
				SDL_RenderDrawLine(gRenderer, p1.x, p1.y, prev.x, prev.y);
				SDL_RenderDrawLine(gRenderer, p1.x, p1.y, cur.x,  cur.y);
				SDL_RenderDrawLine(gRenderer, cur.x, cur.y, prev.x, prev.y);
			}
			
			SDL_RenderPresent(gRenderer);
		}
		
	private:
		
};

#endif