/////////////////////////////////
// ** GABRIEL SCHNEIDER - 2020 //
/////////////////////////////////

/*
** TODO: remove wall between chunks
** TODO: tree leafs get cut by chunk borders
** TODO: fix rendering order to allow good alpha blending
** TODO: maybe have a single thread to load all the chunks linearly
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
#include <thread>
#include <memory>

#include "include/graphics.hpp"
#include "include/cubes.hpp"
#include "include/object_mesh.hpp"
#include "include/world.hpp"

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

	cam.pos = glm::vec3(0,0,0);

	setUniformMatrix(glm::mat4(1.0), ctx.CurShader(), (char*)"model");
	setUniformMatrix(cam.View(), ctx.CurShader(), (char*)"view");
	setUniformMatrix(cam.Proj(), ctx.CurShader(), (char*)"proj");
	setUniformFloat(0.0f, ctx.CurShader(), (char*)"percentage");


	ctx.loadTexArray((char*)"./assets/block.png", GRASS);
	ctx.loadTexArray((char*)"./assets/wook_my_0.png", WOOD);
	ctx.loadTexArray((char*)"./assets/leaves.png", LEAVES);
	ctx.loadTexArray((char*)"./assets/rock.png", ROCK);

	World world = World({0, 0, 0}, cam);
	//vector<Chunk> chunks;

	vector<std::shared_ptr<ObjectMesh>> meshes;
	//meshes.push_back(std::make_shared<SunMesh>());
	auto text = std::make_shared<TextMesh>();
	meshes.push_back(text);
	//meshes.push_back(std::make_shared<ObjectMesh>());

	int I=4,J=4,K=3;

	auto fun = [&]() -> void {
		glUseProgram(ctx.CurShader());

		float t = (float)SDL_GetTicks() * 0.001f;

		glm::vec3 sunPos = glm::vec3(sinf(t * 0.01), -1, cosf(t * 0.02));
		setUniformVec3(sunPos, ctx.CurShader(), (char*)"Sun");

		for(auto const& p : world.loaded_chunks) {
			auto c = p.second;

			bool wire = (world.pos == (XYZ){c->X, c->Y, c->Z});
			c->Draw(wire);
		}

		sprintf(text->text, "(%0.2f, %0.2f, %0.2f)", cam.pos.x, cam.pos.y, cam.pos.z);
		text->LoadFont();
		for(auto& obj : meshes) {
			obj->Draw(cam);
		}

		glUseProgram(ctx.CurShader());
	};

	GLenum err;
	while(ctx.running) {
		while((err = glGetError()) != GL_NO_ERROR) {
			printf("Err 0x%x\n", err);
		}

		ctx.update(cam, ctrl);
		world.Update(cam);
		ctx.draw(cam, fun);
	}

	return 0;
}
