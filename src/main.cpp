#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "include/controls.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

using namespace std;

const float A = 0.0f;
const float B = 1.0f * (1.0f/4.0f);
const float C = 1.0f * (2.0f/4.0f);
const float D = 1.0f * (3.0f/4.0f);
const float E = 1.0f;

GLfloat cube_vertices[] = {
	//  x    y     z     u  v
	-0.5f, -0.5f, -0.5f, B, A,
	0.5f, -0.5f, -0.5f, C, A,
	0.5f,  0.5f, -0.5f, C, E,

	0.5f,  0.5f, -0.5f, C, E,
	-0.5f,  0.5f, -0.5f, B, E,
	-0.5f, -0.5f, -0.5f, B, A,

	// -------------------------------------------------

	-0.5f, -0.5f,  0.5f, B, A,
	0.5f, -0.5f,  0.5f, C, A,
	0.5f,  0.5f,  0.5f, C, E,

	0.5f,  0.5f,  0.5f, C, E,
	-0.5f,  0.5f,  0.5f, B, E,
	-0.5f, -0.5f,  0.5f, B, A,

	// ----------------------

	-0.5f,  0.5f,  0.5f, C,E,
	-0.5f,  0.5f, -0.5f, B,E,
	-0.5f, -0.5f, -0.5f, B,A,

	-0.5f, -0.5f, -0.5f, B,A,
	-0.5f, -0.5f,  0.5f, C,A,
	-0.5f,  0.5f,  0.5f, C,E,

	// -----------------

	0.5f,  0.5f,  0.5f, C,E,
	0.5f,  0.5f, -0.5f, B,E,
	0.5f, -0.5f, -0.5f, B,A,

	0.5f, -0.5f, -0.5f, B,A,
	0.5f, -0.5f,  0.5f, C,A,
	0.5f,  0.5f,  0.5f, C,E,

	// -------------------

	-0.5f, -0.5f, -0.5f, A,A,
	0.5f, -0.5f, -0.5f, B,A,
	0.5f, -0.5f,  0.5f, B,E,

	0.5f, -0.5f,  0.5f, B,E,
	-0.5f, -0.5f,  0.5f, A,E,
	-0.5f, -0.5f, -0.5f, A,A,

	//-----------------------------------------------

	-0.5f,  0.5f, -0.5f, C, A,
	0.5f,  0.5f, -0.5f, D, A,
	0.5f,  0.5f,  0.5f, D, E,

	0.5f,  0.5f,  0.5f, D, E,
	-0.5f,  0.5f,  0.5f, C, E,
	-0.5f,  0.5f, -0.5f, C, A
};

string read_file(char* filename) {
	ifstream f(filename);
	string str;
	if(f) {
		ostringstream ss;
		ss << f.rdbuf();
		str = ss.str();
	}

	return str;
}

GLuint createShader(GLenum shaderType, string& str_source) {


	GLuint shader=glCreateShader(shaderType);

	str_source.push_back('\0');
	const GLchar *source = (const GLchar *)str_source.c_str();

	cout << "FILE:" << source << '\n';

	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if(status != GL_TRUE) {

		char buf[512];
		glGetShaderInfoLog(shader, 512, NULL, buf);

		puts("Error compiling shader");
		puts(buf);
		exit(-1);
	}

	return shader;
}

struct Context {
	SDL_Window* window;
	SDL_GLContext context;

	GLuint shaderProgram, shaderFrag, shaderVert, texture;

	std::vector<GLuint> shaders;
	int cur_shader = 0;

	int w, h;
	bool running;
	unsigned int last_draw = 0, last_update = 0;

	Context (int _w, int _h, char* title) {
		w = _w, h = _h;
		window = SDL_CreateWindow(title,SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
								  w, h, SDL_WINDOW_OPENGL);
		if(window==NULL) {
			puts(SDL_GetError());
			exit(-1);
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		context = SDL_GL_CreateContext(window);
		glewExperimental = GL_TRUE;
		glewInit();

		glEnable(GL_DEPTH_TEST);

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D,texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	void update(Camera& cam, Controls& ctrl) {

		if(SDL_GetTicks()-last_update < 10) return;

		SDL_Event e;


		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) running = false;

			else if(e.type==SDL_KEYDOWN) {
				float cam_speed = 1.0f;
				printf("KEY (%u)\n", e.key.keysym.sym);

				//ctrl.RegUp(e.key.keysym.sym);

				switch(e.key.keysym.sym) {
					case SDLK_ESCAPE:
						running = false;
						break;

						// basic movement
					case SDLK_SPACE:
						cam.pos.y += cam_speed;
						break;
					case SDLK_z:
						cam.pos.y -= cam_speed;
						break;
					case SDLK_w:
						cam.pos += cam.foward * cam_speed;
						break;
					case SDLK_s:
						cam.pos -= cam.foward * cam_speed;
						break;
					case SDLK_a:
						cam.pos -= cam.Right() * cam_speed;
						break;
					case SDLK_d:
						cam.pos += cam.Right() * cam_speed;
						break;

						//    rotating foward vector
					case SDLK_e:
						cam.RotateYaw(-2.0f);
						break;
					case SDLK_q:
						cam.RotateYaw(2.0f);
						break;
				}
			}
		}

		last_update = SDL_GetTicks();
	}

	void draw(Camera cam) {
		if(SDL_GetTicks()-last_draw < 15) return;

		// moving the matrix to the camera
		setUniformMatrix(cam.View(glm::vec3(0,0,0)), (char*)"view");

		float t = (float)SDL_GetTicks()/1000.0f;

		//setUniformMatrix(glm::rotate(glm::mat4(1.0f), t, glm::vec3(0.0, 1, 0)), (char*)"model");

		glViewport(0,0,w,h);
		glClearColor(0.1f,0.2f,0.4f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES,0,36);

		//auto mat = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
		//mat = glm::rotate(mat, t, glm::vec3(0.0, 1, 0));
		auto mat = glm::mat4(1.0f);
		for(int i=0;i<10;i++) {
			for(int j=0;j<10; j++) {
				setUniformMatrix(glm::translate(mat, glm::vec3(j, i, 0)), (char*)"model");

				glDrawArrays(GL_TRIANGLES,0,36);
				//glDrawArrays(GL_LINE_STRIP,0,36);
			}
		}

		SDL_GL_SwapWindow(window);

		last_draw = SDL_GetTicks();
	}

	GLuint CurShader() {
		return shaders[cur_shader];
	}

	void loadShader(string& vert, string& frag) {

		shaderVert = createShader(GL_VERTEX_SHADER, vert);
		shaderFrag = createShader(GL_FRAGMENT_SHADER, frag);

		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, shaderVert);
		glAttachShader(shaderProgram, shaderFrag);

		glLinkProgram(shaderProgram);
		glUseProgram(shaderProgram);
	}

	void loadMeshWithEBO(float* vert, int vsz, GLuint* el, int elsz) {

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vsz, vert, GL_STATIC_DRAW);

		GLuint vao;
		glGenVertexArrays(1,&vao);
		glBindVertexArray(vao);

		GLuint ebo;
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elsz, el, GL_STATIC_DRAW);

		GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,5*sizeof(float),0);

		GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
		glEnableVertexAttribArray(colAttrib);
		glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,5*sizeof(float),(void*)(2*sizeof(float)));
	}

	void loadMeshUV(float* vert, int vsz) {

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vsz, vert, GL_STATIC_DRAW);

		GLuint vao;
		glGenVertexArrays(1,&vao);
		glBindVertexArray(vao);

		GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,5*sizeof(float),0);

		GLint coordAttrib = glGetAttribLocation(shaderProgram, "texcoord");
		glEnableVertexAttribArray(coordAttrib);
		glVertexAttribPointer(coordAttrib, 2, GL_FLOAT, GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
	}

	void setUniformMatrix(glm::mat4 mat, char* name) {
		GLint pos = glGetUniformLocation(shaderProgram, name);
		glUniformMatrix4fv(pos, 1, GL_FALSE, glm::value_ptr(mat));
	}

	void loadTexture(char* file, char* name) {
		int width, height, nchannels;
		unsigned char* data = stbi_load(file, &width, &height, &nchannels,0);

		if(data==NULL) {
			puts("Image doesn't exist");
			exit(-1);
		}

		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
		glUniform1i(glGetUniformLocation(shaderProgram, name), 0);
	}
};

int main(int argc, char *argv[]) {

	Context ctx = Context(800, 800, (char*)"Hello OpenGL");

	string vert = read_file((char*)"./src/shaders/main.vert");
	string frag = read_file((char*)"./src/shaders/main.frag");
	string wire_frag = read_file((char*)"./src/shaders/wireframe.frag");

	ctx.loadShader(vert, wire_frag);

	Camera cam;
	Controls ctrl;

	cam.pos = glm::vec3(0.0,0,5);

	//ctx.setUniformMatrix( glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0,0,1)), (char*)"model");
	ctx.setUniformMatrix(cam.View(glm::vec3(0,0,0)), (char*)"view");
	ctx.setUniformMatrix(cam.Proj(), (char*)"proj");

	ctx.loadTexture((char*)"./assets/block.jpg", (char*)"texBlock");

	ctx.loadMeshUV(cube_vertices, sizeof(cube_vertices));

	SDL_Event event;
	while(ctx.running) {

		ctx.update(cam, ctrl);
		ctx.draw(cam);
	}

	return 0;
}
