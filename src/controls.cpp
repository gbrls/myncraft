#include "include/controls.hpp"

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

Input::event Controls::Process(SDL_Event e, Camera& cam) {

	switch (e.type) {
		case SDL_QUIT: { return Input::QUIT; }

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

			if(k == SDLK_TAB) return Input::TOGGLE_DEBUG;
			if(k == SDLK_ESCAPE) return Input::QUIT;

			break;
		}
		default:
			return Input::NONE;
			break;
	}

	return Input::NONE;
}
