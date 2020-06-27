#pragma once

#include <bitset>
#include <cstdio>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL2/SDL.h>

struct Camera {
	glm::vec3 pos;
	glm::vec3 foward;
	float yaw;
	float ratio;

	Camera (int w, int h);
	glm::mat4 View(glm::vec3 center);
	glm::mat4 Proj();
	glm::vec3 Right();
	void RotateYaw(float angle);
	void RotatePitch(float angle);
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
