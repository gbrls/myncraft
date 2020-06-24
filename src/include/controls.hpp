#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Camera {
	glm::vec3 pos;
	float pitch, yaw, row;

	Camera () {
		pos = glm::vec3(0.0, 0.0, 0.0);
		pitch=0.0, yaw=0.0, row=0.0;
	}

	glm::mat4 View(glm::vec3 center) {
		return glm::lookAt(pos, center, glm::vec3(0, 1, 0));
	}

	glm::mat4 Proj() {
		return glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 100.0f);
	}
};
