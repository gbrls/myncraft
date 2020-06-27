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
