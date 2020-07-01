#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <GL/gl.h>

#include "graphics.hpp"

class ObjectMesh {
	private:
		GLuint shader, texture, vao;
		std::vector<float> verts;
		int nvert;
	public:
		ObjectMesh();
		void LoadTexture(char* file, char* name);
		void LoadShader(std::string& vert, std::string& frag);
		void LoadGeometry(std::vector<float> verts);
		void Draw();
};

class TestMesh : public ObjectMesh {
	public:
		TestMesh();
};
