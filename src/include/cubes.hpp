#pragma once

#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>

#include "graphics.hpp"

#define SZ  32
// szp = sz + 2
#define SZP 34

struct XYZ {
	int x,y,z;
};

struct Chunk {
	char mat[SZ][SZ][SZ];
	char visited[SZP][SZP][SZP];
	int X,Y,Z;

	int nvert;

	std::pair<bool, GLuint> cache;

	Chunk (int x, int y , int z);
	// a dfs like function
	std::vector<float> Mesh();
	void _mesh(int i, int j, int k, int id, int sig, std::vector<float>& vec);
	GLuint Vao(Context& ctx);

	void gen_world();
};
