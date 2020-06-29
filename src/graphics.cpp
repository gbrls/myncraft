#include <iostream>
#include <cstdio>
#include <string>

#include <GL/glew.h>
#include <GL/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

#include "include/controls.hpp"
#include "include/graphics.hpp"

GLuint createShader(GLenum shaderType, std::string& str_source) {

	GLuint shader=glCreateShader(shaderType);

	str_source.push_back('\0');
	const GLchar *source = (const GLchar *)str_source.c_str();

	std::cout << "FILE:" << source << '\n';

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

Context::Context (int _w, int _h, char* title) {
	w = _w, h = _h;
	running = true;
	last_draw=0, last_update=0;
	debug=0, cur_shader=0;
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

	SDL_SetRelativeMouseMode(SDL_TRUE);

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


void Context::update(Camera& cam, Controls& ctrl) {

	if(SDL_GetTicks()-last_update < 10) return;

	SDL_Event e;


	while(SDL_PollEvent(&e)) {
		switch (ctrl.Process(e, cam)) {
			case Input::QUIT:
				running = 0;
				break;
			case Input::TOGGLE_DEBUG:
				debug = 1 - debug;
				setUniformFloat((float)debug, (char*)"percentage");
				if(debug) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
				else glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
				break;
			default:
				break;
		}
	}

	ctrl.Input(cam);

	last_update = SDL_GetTicks();
}

void Context::draw(Camera cam, const std::function <void ()>& f) {
	if(SDL_GetTicks()-last_draw < 15) return;

	// moving the matrix to the camera
	setUniformMatrix(cam.View(glm::vec3(0,0,0)), (char*)"view");

	float t = (float)SDL_GetTicks()/1000.0f;

	//setUniformMatrix(glm::rotate(glm::mat4(1.0f), t, glm::vec3(0.0, 1, 0)), (char*)"model");

	glViewport(0,0,w,h);
	glClearColor(0.1f,0.2f,0.4f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	f();

	SDL_GL_SwapWindow(window);

	last_draw = SDL_GetTicks();
}

GLuint& Context::CurShader() {
	return shaders[cur_shader];
}


void Context::SwitchShader(int id) {
	cur_shader = id;
	glUseProgram(CurShader());
}

void Context::loadShader(std::string& vert, std::string& frag) {

	shaders.push_back(0);
	int bef = cur_shader;
	cur_shader = shaders.size()-1;

	shaderVert = createShader(GL_VERTEX_SHADER, vert);
	shaderFrag = createShader(GL_FRAGMENT_SHADER, frag);

	CurShader() = glCreateProgram();

	glAttachShader(CurShader(), shaderVert);
	glAttachShader(CurShader(), shaderFrag);

	glLinkProgram(CurShader());
	glUseProgram(CurShader());

	cur_shader = bef;
	glUseProgram(CurShader());
}


void Context::loadMeshUVWithEBO(float* vert, int vsz, GLuint* el, int elsz) {

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vsz, vert, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	GLint posAttrib = glGetAttribLocation(CurShader(), "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,5*sizeof(float),0);

	GLint coordAttrib = glGetAttribLocation(CurShader(), "texcoord");
	glEnableVertexAttribArray(coordAttrib);
	glVertexAttribPointer(coordAttrib, 2, GL_FLOAT, GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elsz, el, GL_STATIC_DRAW);
}

GLuint Context::loadMeshUV(float* vert, int vsz) {

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vsz, vert, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	GLint posAttrib = glGetAttribLocation(CurShader(), "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,5*sizeof(float),0);

	GLint coordAttrib = glGetAttribLocation(CurShader(), "texcoord");
	glEnableVertexAttribArray(coordAttrib);
	glVertexAttribPointer(coordAttrib, 2, GL_FLOAT, GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));

	return vao;
}

void Context::setUniformMatrix(glm::mat4 mat, char* name) {
	GLint pos = glGetUniformLocation(CurShader(), name);
	glUniformMatrix4fv(pos, 1, GL_FALSE, glm::value_ptr(mat));
}

void Context::setUniformFloat(float f, char* name) {
	GLint pos = glGetUniformLocation(CurShader(), name);
	glUniform1f(pos, f);
}

void Context::loadTexture(char* file, char* name) {
	int width, height, nchannels;
	unsigned char* data = stbi_load(file, &width, &height, &nchannels, STBI_rgb_alpha);

	if(data==NULL) {
		printf("The image (%s) does not exist\n", file);
		exit(-1);
	}

	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	glUniform1i(glGetUniformLocation(CurShader(), name), 0);
}
