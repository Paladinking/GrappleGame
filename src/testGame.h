#ifndef TEST_GAME_00_H
#define TEST_GAME_00_H
#include <cmath>
#include <vector>
#include <algorithm>
#include "geometry.h"

struct Point {
	double x, y;
};


class TestGame : public Game {
	public:
		TestGame() : Game(SCREEN_WIDTH, SCREEN_HEIGHT, "Test") {}
	protected:
	
		double max_length = 300.0;
		
		Texture circle1, circle2;
		Point anchor = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 200};
		Point ball = {SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 - 50};
		Point vel = {0.0, 0.0};
		Point projected;
		Point rotated;
		Point line_vector;
		bool toggle = false;
		
		virtual void init() override {
			circle1.load_from_file("assets/ball.png");
			circle2.load_from_file("assets/ball.png");
			circle1.set_dimensions(4, 4);
			circle2.set_dimensions(20, 20);
		}
		
		virtual void handle_mousepress(SDL_MouseButtonEvent e) override {
			if (e.button == SDL_BUTTON_MIDDLE){
				return;
			}
			vel.y = - 700.0; 
			//toggle = !toggle;
		}
		
		virtual void tick(Uint64 delta) {
			double dDelta = delta / 1000.0;
			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
			if (currentKeyStates[SDL_SCANCODE_UP]) {
				vel.y -= 300.0 * dDelta;
			}
			if (currentKeyStates[SDL_SCANCODE_DOWN]) {
				vel.y += 300.0 * dDelta;
			}
			if (currentKeyStates[SDL_SCANCODE_LEFT]) {
				vel.x -= 300.0 * dDelta;
			}
			if (currentKeyStates[SDL_SCANCODE_RIGHT]) {
				vel.x += 300.0 * dDelta;
			}
			if (currentKeyStates[SDL_SCANCODE_Q] && max_length > 10.0) {
				max_length -= 100.0 * dDelta;
			}
			if (currentKeyStates[SDL_SCANCODE_E]) {
				max_length += 100.0 * dDelta;
			}
			
			
			//vel.y += (toggle ? 0.0: GRAVITY_ACCELERATION) * dDelta; 
			//vel.x += (toggle ? 300.0 : 0.0) * dDelta;
			vel.x -= vel.x * std::abs(vel.x) * 0.001 * dDelta;
			vel.y -= vel.y * std::abs(vel.y) * 0.001 * dDelta;
			
			Point next = {ball.x + vel.x * dDelta, ball.y + vel.y * dDelta};
			line_vector = {anchor.x - ball.x, anchor.y - ball.y};
			rotated = {-line_vector.y, line_vector.x};
			double line_length = distance(next.x, next.y, anchor.x, anchor.y);
			
			if (line_length > max_length) {
				double angle = get_angle(vel.x, vel.y, line_vector.x, line_vector.y);
				if (angle > 3.141592 / 2) {
					double proj_scalar = (rotated.x * vel.x + rotated.y * vel.y)
						/ (rotated.x * rotated.x + rotated.y * rotated.y);
					
					projected = {rotated.x * proj_scalar, rotated.y * proj_scalar};
					vel = projected;
					
					next = {ball.x + vel.x * dDelta, ball.y + vel.y * dDelta};
					
					Point lv = {anchor.x - next.x, anchor.y - next.y};
					double line_len = length(lv.x, lv.y);
					
					next.x = anchor.x + max_length * -lv.x / line_len;
					next.y = anchor.y + max_length * -lv.y / line_len;
				}
			}
			
			
			ball.x = next.x;
			ball.y = next.y;
		}
		
		virtual void handle_keyup(SDL_KeyboardEvent e) override {
			if (e.keysym.sym == SDLK_ESCAPE) {
				exit_game();
			}
		}
		
		virtual void render() override {
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(gRenderer);
			
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderDrawLine(gRenderer, ball.x, ball.y, anchor.x, anchor.y);
			
			circle1.render(anchor.x - 2, anchor.y - 2);
			circle2.render(ball.x - 10, ball.y - 10);
			
			double len = length(vel.x, vel.y);
			if (len != 0) { 
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0xFF, 0xFF);
				SDL_RenderDrawLine(gRenderer, ball.x, ball.y, ball.x + 20 * vel.x / len * 2, ball.y  + 20 * vel.y / len * 2);
			}
			len = length(projected.x, projected.y);
			if (len != 0) {
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
				SDL_RenderDrawLine(gRenderer, ball.x, ball.y, ball.x + 20 * projected.x / len * 2, ball.y + 20 * projected.y / len * 2);
			}
			len = length(line_vector.x, line_vector.y);
			if (len != 0) {
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x99, 0xFF);
				SDL_RenderDrawLine(gRenderer, ball.x, ball.y, ball.x + 20 * line_vector.x / len * 2, ball.y + 20 * line_vector.y / len * 2);
			}
			len = length(rotated.x, rotated.y);
			if (len != 0) {
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x99, 0xFF);
				SDL_RenderDrawLine(gRenderer, ball.x, ball.y, ball.x + 20 * rotated.x / len * 2, ball.y + 20 * rotated.y / len * 2);
			}

			SDL_RenderPresent(gRenderer);
		}
		
};

#endif