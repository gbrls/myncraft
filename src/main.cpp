/////////////////////////////////
// ** GABRIEL SCHNEIDER - 2020 //
/////////////////////////////////

/*
** TODO: remove wall between chunks
** TODO: segfault when the chunk is empty
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

#include "include/graphics.hpp"
#include "include/cubes.hpp"

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

int main(int argc, char *argv[]) {

	int w = 1400, h = 900;

	Context ctx = Context(w, h, (char*)"Myncraft");

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

	//ctx.loadTexture((char*)"./assets/block_my_0.png", (char*)"texBlock");
	ctx.loadTexture((char*)"./assets/wook_my_0.png", (char*)"texBlock");
	//ctx.loadTexArray((char*)"./assets/wook_my_0.png", 0);

	//Chunk chunk = Chunk(0, 0, 0), chunk1 = Chunk(1, 0, 0);
	vector<Chunk> chunks;
	for(int i=0;i<2;i++) {
		for(int j=0;j<2;j++) {
			for(int k=0;k<3;k++) {
				chunks.push_back(Chunk(i,k,j));
			}
		}
	}

	auto fun = [&ctx, &chunks]() -> void {

		for(Chunk& c : chunks) {
			glBindVertexArray(c.Vao(ctx));
			glDrawArrays(GL_TRIANGLES, 0, c.nvert);
		}
	};

	GLenum err;
	while(ctx.running) {
		while((err = glGetError()) != GL_NO_ERROR) {
			printf("%x\n", err);
		}

		ctx.update(cam, ctrl);
		ctx.draw(cam, fun);
	}

	return 0;
}
