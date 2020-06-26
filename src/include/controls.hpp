#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bitset>
#include <cstdio>

#include <SDL2/SDL.h>


struct Camera {
	glm::vec3 pos;
	glm::vec3 foward;
	float yaw;
	float ratio;

	Camera (int w, int h) {

		ratio = (float)w/(float)h;

		pos = glm::vec3(0.0, 0.0, -10.0);
		foward = glm::vec3(0.0, 0.0, -1.0);
	}

	glm::mat4 View(glm::vec3 center) {
		//                 pos   eye        up
		return glm::lookAt(pos, pos+foward, glm::vec3(0, 1, 0));
	}

	glm::mat4 Proj() {
		return glm::perspective(glm::radians(60.0f), ratio, 1.0f, 500.0f);
	}

	glm::vec3 Right() {
		return glm::cross(foward, glm::vec3(0, 1, 0));
	}

	void RotateYaw(float angle) {
		glm::mat4 m = glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3(0, 1, 0));
		glm::vec4 tmp = glm::vec4(foward.x, foward.y, foward.z, 1);
		tmp = m * tmp;
		foward.x = tmp.x, foward.y = tmp.y, foward.z = tmp.z;
	}

	void RotatePitch(float angle) {
		glm::mat4 m = glm::rotate(glm::mat4(1.0), glm::radians(angle), Right());
		glm::vec4 tmp = glm::vec4(foward.x, foward.y, foward.z, 1);
		tmp = m * tmp;
		foward.x = tmp.x, foward.y = tmp.y, foward.z = tmp.z;
	}
};

static const long long int BS_MAX = (1LL<<31);

namespace Input {
	enum event {QUIT, PAUSE, TOGGLE_DEBUG, NONE};
	const int NKEYS = 256;
	int keys[NKEYS] = {0};
	int shift = 0;
};

using namespace Input;

struct Controls {

	Controls () {}

	bool IsKeyPressed(SDL_Keycode k) {

		if(SDLK_LSHIFT == k) return shift==1;

		if(k >= NKEYS || k < 0) return false;

		return keys[k]==1;
	}

	void Input(Camera& cam) {
		float cam_speed = 0.2f;

		if(IsKeyPressed(SDLK_SPACE)) cam.pos.y += cam_speed;
		if(IsKeyPressed(SDLK_LSHIFT)) cam.pos.y -= cam_speed;
		if(IsKeyPressed(SDLK_w)) cam.pos += cam.foward * cam_speed;
		if(IsKeyPressed(SDLK_s)) cam.pos -= cam.foward * cam_speed;
		if(IsKeyPressed(SDLK_a)) cam.pos -= cam.Right() * cam_speed;
		if(IsKeyPressed(SDLK_d)) cam.pos += cam.Right() * cam_speed;


		if(IsKeyPressed(SDLK_c)) printf("(%0.2f, %0.2f, %0.2f)\n", cam.pos.x, cam.pos.y, cam.pos.z);
	}

	event Process(SDL_Event e, Camera& cam) {

		switch (e.type) {
			case SDL_QUIT: { return QUIT; }

			case SDL_MOUSEMOTION: {
				int x = e.motion.xrel, y = e.motion.yrel;

				cam.RotateYaw((float)x * -0.1);
				cam.RotatePitch((float)y * -0.1);

				break;
			}
			case SDL_KEYUP: {
				auto k = e.key.keysym.sym;
				if(k < NKEYS && k >= 0) {
					keys[k] = 0;
				}

				if(k == SDLK_LSHIFT) shift = 0;

				break;
			}
			case SDL_KEYDOWN: {

				auto k = e.key.keysym.sym;
				if(k < NKEYS && k >= 0) {
					keys[k] = 1;
				}

				if(k == SDLK_LSHIFT) shift = 1;

				if(k == SDLK_TAB) return TOGGLE_DEBUG;
				if(k == SDLK_ESCAPE) return QUIT;

				break;
			}
			default:
				return NONE;
				break;
		}


		return NONE;
	}

};

	//std::bitset<BS_MAX> keyboard;

	//void RegUp(unsigned int id) {
	//	keyboard.set(id);
	//}

	//void RegDown(unsigned int id) {
	//	keyboard.reset(id);
	//}

