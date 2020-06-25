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

	Camera () {
		pos = glm::vec3(0.0, 0.0, -10.0);
		foward = glm::vec3(0.0, 0.0, -1.0);
	}

	glm::mat4 View(glm::vec3 center) {
		//                 pos   eye        up
		return glm::lookAt(pos, pos+foward, glm::vec3(0, 1, 0));
	}

	glm::mat4 Proj() {
		return glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 500.0f);
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
	int keys[NKEYS];
};

using namespace Input;

struct Controls {

	Controls () {}

	event Process(SDL_Event e, Camera& cam) {

		switch (e.type) {
			case SDL_QUIT: { return QUIT; }

			case SDL_MOUSEMOTION: {
				int x = e.motion.xrel, y = e.motion.yrel;

				cam.RotateYaw((float)x * -0.1);
				cam.RotatePitch((float)y * -0.1);

				break;
			}
			case SDL_KEYDOWN: {
				float cam_speed = 1.0f;
				switch(e.key.keysym.sym) {
					case SDLK_ESCAPE:
						return QUIT;
						break;

						// basic movement
					case SDLK_SPACE:
						cam.pos.y += cam_speed;
						break;
					case SDLK_z:
						cam.pos.y -= cam_speed;
						break;
					case SDLK_w:
						cam.pos += cam.foward * cam_speed;
						break;
					case SDLK_s:
						cam.pos -= cam.foward * cam_speed;
						break;
					case SDLK_a:
						cam.pos -= cam.Right() * cam_speed;
						break;
					case SDLK_d:
						cam.pos += cam.Right() * cam_speed;
						break;

						//    rotating foward vector
					case SDLK_e:
						cam.RotateYaw(-2.0f);
						break;
					case SDLK_q:
						cam.RotateYaw(2.0f);
						break;

					case SDLK_TAB:
						return TOGGLE_DEBUG;
						break;
				}
				break;
			}
			default:
				return NONE;
				break;
		}


		puts("ERR CASE INPU");
		exit(-1);
	}

};

	//std::bitset<BS_MAX> keyboard;

	//void RegUp(unsigned int id) {
	//	keyboard.set(id);
	//}

	//void RegDown(unsigned int id) {
	//	keyboard.reset(id);
	//}

