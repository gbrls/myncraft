#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <GL/gl.h>

#include "graphics.hpp"
#include "controls.hpp"

class ObjectMesh {
	public:
		GLuint shader, texture, vao;
		std::vector<float> verts;
		int nvert;
		ObjectMesh();
		void LoadTexture(char* file, char* name);
		void LoadShader(std::string& vert, std::string& frag);
		void LoadGeometry(std::vector<float> verts);
		virtual void Draw();
};

class SunMesh : public ObjectMesh {
	public:
		SunMesh();
		void Draw();
};
