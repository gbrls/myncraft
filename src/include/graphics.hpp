#pragma once

#include <string>
#include <vector>
#include <functional>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "controls.hpp"

GLuint createShader(GLenum shaderType, std::string& str_source);

struct Context {
	SDL_Window* window;
	SDL_GLContext context;

	GLuint shaderProgram, shaderFrag, shaderVert, texture, texArray;

	std::vector<GLuint> shaders;
	int cur_shader;

	int w, h;
	bool running, paused;
	unsigned int last_draw, last_update;
	int debug;

	Context (int _w, int _h, char* title);
	void update(Camera& cam, Controls& ctrl);
	void draw(Camera cam, const std::function <void ()>& f);
	GLuint& CurShader();
	void SwitchShader(int id);
	void loadShader(std::string& vert, std::string& frag);
	void loadMeshUVWithEBO(float* vert, int vsz, GLuint* el, int elsz);
	GLuint loadMeshUV(float* vert, int vsz);
	void setUniformMatrix(glm::mat4 mat, char* name);
	void setUniformFloat(float f, char* name);
	void setUniformVec3(glm::vec3 vec, char* name);

	void loadTexArray(char* file, int index);
	void loadTexture(char* file, char* name);
};
