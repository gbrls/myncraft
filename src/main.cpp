/////////////////////////////////
// ** GABRIEL SCHNEIDER - 2020 //
/////////////////////////////////

/*
** TODO: remove wall between chunks
 */

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
#include <functional>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "include/controls.hpp"
#include "include/cubes.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

using namespace std;

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
	int debug=0;


	Context (int _w, int _h, char* title) {
		w = _w, h = _h;
		running = true;
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

	void update(Camera& cam, Controls& ctrl) {

		if(SDL_GetTicks()-last_update < 10) return;

		SDL_Event e;


		while(SDL_PollEvent(&e)) {
			switch (ctrl.Process(e, cam)) {
				case QUIT:
					running = 0;
					break;
				case TOGGLE_DEBUG:
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

	void draw(Camera cam, const std::function <void ()>& f) {
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

	GLuint& CurShader() {
		return shaders[cur_shader];
	}

	void SwitchShader(int id) {
		cur_shader = id;
		glUseProgram(CurShader());
	}

	void loadShader(string& vert, string& frag) {

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

	void loadMeshUVWithEBO(float* vert, int vsz, GLuint* el, int elsz) {

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

	GLuint loadMeshUV(float* vert, int vsz) {

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

	void setUniformMatrix(glm::mat4 mat, char* name) {
		GLint pos = glGetUniformLocation(CurShader(), name);
		glUniformMatrix4fv(pos, 1, GL_FALSE, glm::value_ptr(mat));
	}

	void setUniformFloat(float f, char* name) {
		GLint pos = glGetUniformLocation(CurShader(), name);
		glUniform1f(pos, f);
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
		glUniform1i(glGetUniformLocation(CurShader(), name), 0);
	}
};


int main(int argc, char *argv[]) {

	int w = 1400, h = 900;

	Context ctx = Context(w, h, (char*)"Hello OpenGL");

	string vert = read_file((char*)"./src/shaders/main.vert");
	string frag = read_file((char*)"./src/shaders/main.frag");
	string wire_frag = read_file((char*)"./src/shaders/wireframe.frag");

	ctx.loadShader(vert, frag);

	Camera cam = Camera(w,h);
	Controls ctrl = Controls();

	cam.pos = glm::vec3(16,16,40);

	ctx.setUniformMatrix(glm::mat4(1.0), (char*)"model");
	ctx.setUniformMatrix(cam.View(glm::vec3(0,0,0)), (char*)"view");
	ctx.setUniformMatrix(cam.Proj(), (char*)"proj");
	ctx.setUniformFloat(0.0f, (char*)"percentage");

	ctx.loadTexture((char*)"./assets/block.jpg", (char*)"texBlock");

	//auto mesh = topFace(10,0,0);
	//ctx.loadMeshUV(&mesh[0], mesh.size()*sizeof(float));

	Chunk chunk = Chunk(0, 0, 0), chunk1 = Chunk(1, 0, 0);
	auto chunk_mesh = chunk.Mesh();
	auto chunk1_mesh = chunk1.Mesh();
	GLuint chunk0_vao = ctx.loadMeshUV(&chunk_mesh[0], chunk_mesh.size()*sizeof(float));
	GLuint chunk1_vao = ctx.loadMeshUV(&chunk1_mesh[0], chunk1_mesh.size()*sizeof(float));


	auto fun = [&chunk, &ctx]() -> void {
		for(int i=0;i<32;i++) {
			for(int j=0;j<32;j++) {
				for(int k=0;k<32;k++) {
					if(chunk.mat[i][j][k]==0) continue;
					ctx.setUniformMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(i, j, k)), (char*)"model");
					glDrawArrays(GL_TRIANGLES,0,36);
				}
			}
		}
	};

	auto fun2 = [&ctx, &chunk_mesh, &chunk1_mesh, &chunk0_vao, &chunk1_vao]() -> void {

		glBindVertexArray(chunk0_vao);
		glDrawArrays(GL_TRIANGLES,0,chunk_mesh.size()/5);
		glBindVertexArray(chunk1_vao);
		glDrawArrays(GL_TRIANGLES,0,chunk1_mesh.size()/5);
	};

	SDL_Event event;
	while(ctx.running) {

		ctx.update(cam, ctrl);
		ctx.draw(cam, fun2);
	}

	return 0;
}
