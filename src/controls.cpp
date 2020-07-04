#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "include/controls.hpp"

Camera::Camera (int w, int h) {

	W = w, H = h;
	ratio = (float)w/(float)h;

	pos = glm::vec3(0.0, 0.0, -10.0);
	foward = glm::vec3(0.0, 0.0, -1.0);
}

glm::mat4 Camera::View() {
	//                 pos   eye        up
	return glm::lookAt(pos, pos+foward, glm::vec3(0, 1, 0));
}

glm::mat4 Camera::Proj() {
	return glm::perspective(glm::radians(60.0f), ratio, 1.0f, 500.0f);
}

glm::vec3 Camera::Right() {
	return glm::normalize(glm::cross(foward, glm::vec3(0, 1, 0)));
}

void Camera::RotateYaw(float angle) {
	glm::mat4 m = glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3(0, 1, 0));
	glm::vec4 tmp = glm::vec4(foward.x, foward.y, foward.z, 1);
	tmp = m * tmp;
	foward.x = tmp.x, foward.y = tmp.y, foward.z = tmp.z;
}

void Camera::RotatePitch(float angle) {
	glm::mat4 m = glm::rotate(glm::mat4(1.0), glm::radians(angle), Right());
	glm::vec4 tmp = glm::vec4(foward.x, foward.y, foward.z, 1);
	tmp = m * tmp;
	foward.x = tmp.x, foward.y = tmp.y, foward.z = tmp.z;
}

glm::vec3 Camera::ChunkPos() {
	return pos / 32.0f;
}

Controls::Controls () {
	shift = 0;
	memset(keys,0,sizeof(keys));
}

bool Controls::IsKeyPressed(SDL_Keycode k) {

	if(SDLK_LSHIFT == k) return shift==1;

	if(k >= NKEYS || k < 0) return false;

	return keys[k]==1;
}

void Controls::Input(Camera& cam) {
	float cam_speed = 0.2f;

	if(IsKeyPressed(SDLK_SPACE)) cam.pos.y += cam_speed;
	if(IsKeyPressed(SDLK_LSHIFT)) cam.pos.y -= cam_speed;
	if(IsKeyPressed(SDLK_w)) cam.pos += cam.foward * cam_speed;
	if(IsKeyPressed(SDLK_s)) cam.pos -= cam.foward * cam_speed;
	if(IsKeyPressed(SDLK_a)) cam.pos -= cam.Right() * cam_speed;
	if(IsKeyPressed(SDLK_d)) cam.pos += cam.Right() * cam_speed;


	if(IsKeyPressed(SDLK_c)) printf("(%0.2f, %0.2f, %0.2f)\n", cam.pos.x, cam.pos.y, cam.pos.z);
}

Input::event Controls::Process(SDL_Event e, Camera& cam, bool paused) {

	switch (e.type) {
		case SDL_QUIT: { return Input::QUIT; }

		case SDL_MOUSEMOTION: {
			if(!paused) {
				int x = e.motion.xrel, y = e.motion.yrel;

				cam.RotateYaw((float)x * -0.1);
				cam.RotatePitch((float)y * -0.1);
			}

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

			if(k == SDLK_TAB) return Input::TOGGLE_DEBUG;
			if(k == SDLK_ESCAPE) return Input::QUIT;
			if(k == SDLK_p) return Input::PAUSE;

			break;
		}
		default:
			return Input::NONE;
			break;
	}

	return Input::NONE;
}
