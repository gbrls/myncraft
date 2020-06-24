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

using namespace std;

#define W 800
#define H 800

int running = 1;

string read_file(char* filename) {
	ifstream f(filename);
	string str;
	if(f) {
		ostringstream ss;
		ss << f.rdbuf(); // reading data
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

	GLuint shaderProgram, shaderFrag, shaderVert;

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

		glViewport(0,0,w,h);
		glClearColor(0.1f,0.2f,0.4f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(window);

		last_draw = SDL_GetTicks();

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

	void loadMesh(float* vert, int vsz, GLuint* el, int elsz) {

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
};

int main(int argc, char *argv[]) {

	Context ctx = Context(800, 800, (char*)"Hello OpenGL");

	string vert = read_file((char*)"./src/shaders/main.vert");
	string frag = read_file((char*)"./src/shaders/main.frag");

	ctx.loadShader(vert, frag);

	float vertices[] = {
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
		0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
	};

	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0
	};

	ctx.loadMesh(vertices, sizeof(vertices), elements, sizeof(elements));

	SDL_Event event;
	while(ctx.running) {
		ctx.update();
		ctx.draw();
	}

	return 0;
}
