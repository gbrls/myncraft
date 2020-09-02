/////////////////////////////////
// ** GABRIEL SCHNEIDER - 2020 //
/////////////////////////////////

/*
** TODO: remove wall between chunks
** TODO: tree leaves get cut by chunk borders
 * TODO: OpenGL "garbage collection" https://stackoverflow.com/questions/10573684/vertex-buffer-objects-delete-process-opengl
 */

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <cmath>

#include "include/graphics.hpp"
#include "include/cubes.hpp"
#include "include/object_mesh.hpp"
#include "include/world.hpp"

using namespace std;

string read_file(char *filename) {
    ifstream f(filename);
    string str;
    if (f) {
        ostringstream ss;
        ss << f.rdbuf();
        str = ss.str();
    }

    return str;
}


static glm::vec3 yaw(glm::vec3 orig, float angle) {
    glm::mat4 m = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    glm::vec4 tmp = glm::vec4(orig.x, orig.y, orig.z, 1);
    tmp = m * tmp;

    return glm::vec3(tmp.x, 0.0f, tmp.z);
}

int main(int argc, char *argv[]) {

    int w = 1400, h = 900;

    Context ctx = Context(w, h, (char *) "Myncraft");
    ObjectMesh::LoadDefaults();

    string vert = read_file((char *) "./src/shaders/cube.vert");
    string frag = read_file((char *) "./src/shaders/cube.frag");
    string wire_frag = read_file((char *) "./src/shaders/wireframe.frag");

    ctx.loadShader(vert, frag);

    Camera cam = Camera(w, h);
    Controls ctrl = Controls();

    cam.pos = glm::vec3(0, 50, 0);

    setUniformMatrix(glm::mat4(1.0), ctx.CurShader(), (char *) "model");
    setUniformMatrix(cam.View(), ctx.CurShader(), (char *) "view");
    setUniformMatrix(cam.Proj(), ctx.CurShader(), (char *) "proj");
    setUniformFloat(0.0f, ctx.CurShader(), (char *) "percentage");

    ctx.loadTexArray((char *) "./assets/block.png", GRASS);
    ctx.loadTexArray((char *) "./assets/wook_my_0.png", WOOD);
    ctx.loadTexArray((char *) "./assets/leaves.png", LEAVES);
    ctx.loadTexArray((char *) "./assets/rock.png", ROCK);

    World world = World({0, 0, 0}, cam);

    vector<std::shared_ptr<ObjectMesh>> meshes;
    //meshes.push_back(std::make_shared<SunMesh>());
    //meshes.push_back(std::make_shared<ObjectMesh>());
    auto text = std::make_shared<TextMesh>(10);
    meshes.push_back(text);

    auto fun = [&]() -> void {
        glUseProgram(ctx.CurShader());

        float t = (float) SDL_GetTicks() * 0.001f;

        glm::vec3 sunPos = glm::vec3(sinf(t * 0.01f), -1, cosf(t * 0.02f));
        setUniformVec3(sunPos, ctx.CurShader(), (char *) "Sun");

        for (auto const &p : world.loaded_chunks) {
            auto c = p.second;

            bool wire = (world.pos == (XYZ) {c->X, c->Y, c->Z});
            auto cpos = glm::vec3(c->X * 32, c->Y * 32, c->Z * 32);
            auto v1 = glm::vec3(0, 0, 1);

            auto front = cam.forward;
            front.y = 0;
            front = glm::normalize(front);

            auto left = yaw(front, -65);

            auto chunkVec = cpos - cam.pos;
            chunkVec.y = 0;
            chunkVec = glm::normalize(chunkVec);

            auto angle = glm::acos(glm::dot(chunkVec, front));
            auto langle = glm::acos(glm::dot(left, front));

            auto dist = glm::distance(cam.pos, cpos);
            if ((angle < langle || dist < 100.0f) && dist < 250.0f) c->Draw(wire);

        }

        sprintf(text->text, "(%0.2f, %0.2f, %0.2f)", cam.pos.x, cam.pos.y, cam.pos.z);
        text->Update();
        for (auto &obj : meshes) {
            obj->Draw(cam);
        }

        glUseProgram(ctx.CurShader());
    };

    GLenum err;
    while (ctx.running) {
        while ((err = glGetError()) != GL_NO_ERROR) {
            printf("Err 0x%x\n", err);
        }

        ctx.update(cam, ctrl);
        world.Update(cam);
        ctx.draw(cam, fun);
    }

    return 0;
}
