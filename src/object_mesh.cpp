#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <cmath>

#include "include/object_mesh.hpp"
#include "include/graphics.hpp"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

static std::string read_file(char* filename) {
	std::ifstream f(filename);
	std::string str;
	if(f) {
		std::ostringstream ss;
		ss << f.rdbuf();
		str = ss.str();
	}

	return str;
}

ObjectMesh::ObjectMesh() {
	std::vector<float> verts = {0.5, 1.0, 0.0, 0.5, 1.0,
					 1.0, 0.0, 0.0, 1.0, 0.0,
					 0.0, 0.0, 0.0, 0.0, 0.0};

	std::string vert = read_file((char*)"./src/shaders/default.vert");
	std::string frag = read_file((char*)"./src/shaders/default.frag");

	LoadShader(vert, frag);
	LoadGeometry(verts);

	LoadTexture((char*)"./assets/default.png", (char*)"tex");
}

void ObjectMesh::Draw(Camera& cam) {

	glUseProgram(shader);
	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, nvert);
}

void ObjectMesh::LoadShader(std::string& vert, std::string& frag) {
	shader = glCreateProgram();

	GLuint shaderVert = createShader(GL_VERTEX_SHADER, vert);
	GLuint shaderFrag = createShader(GL_FRAGMENT_SHADER, frag);


	glAttachShader(shader, shaderVert);
	glAttachShader(shader, shaderFrag);

	glLinkProgram(shader);
	glUseProgram(shader);
}

// the default mesh has (x,y,z,u,v)
void ObjectMesh::LoadGeometry(std::vector<float> verts) {
	GLuint vbo;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(float), &verts[0],
				 GL_STATIC_DRAW);

	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,5*sizeof(float),0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	nvert = verts.size()/5;
}

void ObjectMesh::LoadTexture(char*file, char* name) {
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D,texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	int width, height, nchannels;
	unsigned char* data = stbi_load(file, &width, &height, &nchannels, STBI_rgb_alpha);

	if(data==NULL) {
		printf("The image (%s) does not exist\n", file);
		exit(-1);
	}

	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	glUniform1i(glGetUniformLocation(shader, name), 0);
}

SunMesh::SunMesh() {
	puts("Sunmesh creaing");
	std::vector<float> verts = {0.5, 1.0, 0.0, 0.5, 1.0,
					 1.0, 0.0, 0.0, 1.0, 0.0,
					 0.0, 0.0, 0.0, 0.0, 0.0};

	std::string vert = read_file((char*)"./src/shaders/sun.vert");
	std::string frag = read_file((char*)"./src/shaders/default.frag");

	LoadShader(vert, frag);
	LoadGeometry(verts);

	LoadTexture((char*)"./assets/default.png", (char*)"tex");

	//setuniformmatrix(glm::translate(glm::mat4(1.0), glm::vec3(0.3, -1.0, -0.2) * 1.f), shader, (char*)"model");
	//setUniformMatrix(glm::rotate(glm::mat4(1.0), 2.0f, glm::vec3(0, 1, 0)), shader, (char*)"model");
}

void SunMesh::Draw(Camera& cam) {


	glUseProgram(shader);

	float t = (float)SDL_GetTicks()*0.001f;

	setUniformVec3(glm::vec3(sinf(t), 0.1, 0.3), shader, (char*)"offset");

	auto pos = glm::translate(glm::mat4(1), glm::vec3(115, 70, 50));
	setUniformMatrix(glm::rotate(pos, t, glm::vec3(0.2, 0.5, 0.8)), shader, (char*)"model");

	setUniformMatrix(cam.View(), shader, (char*)"view");
	setUniformMatrix(cam.Proj(), shader, (char*)"proj");

	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, nvert);
}
