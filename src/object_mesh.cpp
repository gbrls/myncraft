#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GL/gl.h>

#include "include/object_mesh.hpp"
#include "include/graphics.hpp"

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
	std::vector<float> verts = {0.5, 1.0, 0.0, 0.0, 0.0,
					 1.0, 0.0, 0.0, 0.0, 0.0,
					 0.0, 0.0, 0.0, 0.0, 0.0};


	std::string vert = read_file((char*)"./src/shaders/default.vert");
	std::string frag = read_file((char*)"./src/shaders/default.frag");

	LoadShader(vert, frag);
	LoadGeometry(verts);
}

void ObjectMesh::Draw() {

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
