#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <GL/gl.h>

#include <SDL2/SDL_ttf.h>

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
		virtual void Draw(Camera& cam);
};

class SunMesh : public ObjectMesh {
	public:
		SunMesh();
		void Draw(Camera& cam) override;
};

class TextMesh : public ObjectMesh {
	private:
		TTF_Font* font;
		float ratio;
	public:
		char text[50];
		TextMesh();
		void LoadFont();
		void Draw(Camera& cam);
};
