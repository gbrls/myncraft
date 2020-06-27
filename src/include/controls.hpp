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
		return glm::normalize(glm::cross(foward, glm::vec3(0, 1, 0)));
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
};

#define NKEYS 256

struct Controls {
	int keys[NKEYS];
	int shift;
	Controls ();
	bool IsKeyPressed(SDL_Keycode k);
	void Input(Camera& cam);
	Input::event Process(SDL_Event e, Camera& cam);

};
