#include <iostream>
#include <cstdio>
#include <fstream>
#include <sstream>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "./include/hello.hpp"

std::string file_to_str(std::string filename) {
	std::ifstream f(filename);
	std::string str;
	if(f) {
		std::ostringstream ss;
		ss << f.rdbuf(); // reading data
		str = ss.str();
	}

	if(str.size() > 10) {
		char tmp = str[10];
		str[10] = '\0';
		str[10] = tmp;
		printf("File (%s) = (%s)", filename.begin(), str.begin());
	} else {
		printf("File (%s) = (%s)", filename.begin(), str.begin());
	}

	return str;
}

GLuint create_shader(std::string& src, GLenum type) {
	GLuint shader = glCreateShader(type);

	src.push_back('\0'); // idk

	const GLchar* c_src = (const GLchar *)src.c_str();
	glShaderSource(shader, 1, &c_src, NULL);
	glCompileShader(shader);

	GLint status;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if(status != GL_TRUE) {

		char buf[512];
		glGetShaderInfoLog(shader, 512, NULL, buf);

		std::cout<<"Error compiling shader "<<src<<'\n';
		puts(buf);
		exit(-1);
	}

	return shader;
}

struct Context {
	SDL_Window* window;
	SDL_GLContext glContext;

	int w,h;
	bool running;
	unsigned int last_draw = 0, last_update = 0;

	GLuint shaderProgram, shaderVert, shaderFrag;
	GLuint vbo, vao, ebo;

	Context (int w, int h, std::string title) {

		window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
		if(window==NULL) {
			puts(SDL_GetError());
			exit(-1);
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		glContext = SDL_GL_CreateContext(window);
		glewExperimental = GL_TRUE;
		glewInit();

		glViewport(0, 0, w, h); // resizing the viewport acording to the window's size
		shaderProgram = glCreateProgram();
	}

	void update() {

		if(SDL_GetTicks()-last_update < 50) return;

		SDL_Event e;

		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) running = false;

			else if(e.type==SDL_KEYDOWN) {
				switch(e.key.keysym.sym) {
					case SDLK_ESCAPE:
						running = false;
						break;
					default:
						break;
				}
			}
		}

		last_update = SDL_GetTicks();
	}

	void draw() {
		if(SDL_GetTicks()-last_draw < 15) return;

		glUseProgram(shaderProgram);

		glViewport(0, 0, w, h);
		glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(window);

		last_draw = SDL_GetTicks();
	}

	void createShader(std::string vert, std::string frag) {
		shaderVert = create_shader(vert, GL_VERTEX_SHADER);
		shaderFrag = create_shader(frag, GL_FRAGMENT_SHADER);

		glAttachShader(shaderProgram, shaderVert);
		glAttachShader(shaderProgram, shaderFrag);

		glLinkProgram(shaderProgram);
		glUseProgram(shaderProgram);
	}

	void loadVertices(float* vertices, int sz) {

		float vert[] = {
			-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
			0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
			-0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
		};

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		//glBufferData(GL_ARRAY_BUFFER, sz, vertices, GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

		glGenVertexArrays(1,&vao);
		glBindVertexArray(vao);

		GLuint elements[] = {
			0, 1, 2,
			2, 3, 0
		};

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

		GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,5*sizeof(float),0);

		GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
		glEnableVertexAttribArray(colAttrib);
		glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,5*sizeof(float),(void*)(2*sizeof(float)));

	}
};

int main() {

	std::cout<<hello_str()<<'\n';

	// only do stuff related to opengl after creating a context
	Context ctx = Context(800, 800, "Myncraft");


	std::string frag = file_to_str("./src/shaders/main.frag");
	std::string vert = file_to_str("./src/shaders/main.vert");
	ctx.createShader(vert, frag);

	ctx.loadVertices(NULL, 0);

	while(ctx.running) {
		ctx.update();
		ctx.draw();
	}

	return 0;
}
