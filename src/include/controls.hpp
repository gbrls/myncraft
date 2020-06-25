#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bitset>

struct Camera {
	glm::vec3 pos;
	glm::vec3 foward;
	float yaw;

	Camera () {
		pos = glm::vec3(0.0, 0.0, -10.0);
		foward = glm::vec3(0.0, 0.0, -1.0);
	}

	glm::mat4 View(glm::vec3 center) {
		//return glm::lookAt(pos, center, glm::vec3(0, 1, 0));
		//                 pos   eye        up
		return glm::lookAt(pos, pos+foward, glm::vec3(0, 1, 0));
	}

	glm::mat4 Proj() {
		return glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 100.0f);
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
};

static const long long int BS_MAX = (1LL<<31);

struct Controls {
	//std::bitset<BS_MAX> keyboard;

	//void RegUp(unsigned int id) {
	//	keyboard.set(id);
	//}

	//void RegDown(unsigned int id) {
	//	keyboard.reset(id);
	//}

};
