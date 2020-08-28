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


        static GLuint TEST_SHADER;
        static std::vector<GLuint> textures;

        static void LoadDefaults();
        static GLuint LoadShader(std::string& vert, std::string& frag);
        static void LoadGeometry(std::vector<float> verts, GLuint* vao, int* nvert);

        GLuint shader, texture, vao;
		std::vector<float> verts;
		int nvert;

		ObjectMesh();
        explicit ObjectMesh(std::vector<float> verts);

		void LoadTexture(char* file, char* name);
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
		int window_width, window_height;
	public:
		char text[50];
		explicit TextMesh(int size);
		void Update();
		void Draw(Camera& cam) override;
};
