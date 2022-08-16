#ifndef TEST_GAME_00_H
#define TEST_GAME_00_H
#include <cmath>

struct Point {
	int x, y;
};

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
			return sp > 0 && tp > 0 && area2 > sp + tp;
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


class TestGame : public Game {
	public:
		TestGame() : Game(SCREEN_WIDTH, SCREEN_HEIGHT, "Test") {}
	protected:
	
		Point p1 = {30, 300};
		Point p2 = {150, 100};
		Point p3 = {120, 240};
		
		virtual void init() override {
			
			
		}
		
		virtual void handle_mousepress(SDL_MouseButtonEvent e) override {
			Triangle t = {p1.x, p1.y, p2.x, p2.y, p3.x, p3.y};
			bool contains = t.contains_point(e.x, e.y);
			if (contains) {
				printf("True\n");
			} else {
				printf("False\n");
			}
		}
		
		virtual void render() override {
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(gRenderer);
			
			
			for (int i = 0; i < FULL_TILE_WIDTH; i++) {
				for (int j = 0; j < FULL_TILE_WIDTH; j++) {
					
					
					
				}
			}
			
			
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderDrawLine(gRenderer, p1.x, p1.y, p2.x, p2.y);
			SDL_RenderDrawLine(gRenderer, p2.x, p2.y, p3.x, p3.y);
			SDL_RenderDrawLine(gRenderer, p1.x, p1.y, p3.x, p3.y);
			
			SDL_RenderPresent(gRenderer);
		}
		
	private:
		
};

#endif