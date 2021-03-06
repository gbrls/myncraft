#pragma once

#include <vector>
#include <functional>

#include <GL/glew.h>
#include <GL/gl.h>

#include "graphics.hpp"
#include "object_mesh.hpp"

#define SZ  32
#define SZP (SZ+2)

enum VoxelType {GRASS=0, WOOD=1, LEAVES=2, ROCK=3};

struct XYZ {
	int x,y,z;

	inline bool operator==(const XYZ& other) {
		return ((x == other.x) && (y == other.y) && (z == other.z));
	}

	inline bool operator!=(const XYZ& other) {
		return !((x == other.x) && (y == other.y) && (z == other.z));
	}
};

struct Voxel {
	unsigned char type;
	unsigned char light; // 0 - 10
};

/*
** I don't like the idea of storing vertices data in the cpu
** so I was creating it and sending it to de gpu without storing it
** in memory. This has one drawback, I cannot multithread it, because
** the code that sends the vertex data to the gpu does not parallelize well.
**
**  So, I'm going to...
 */
struct Chunk {
	Voxel mat[SZP][SZP][SZP];
	char visited[SZP][SZP][SZP];
	int X,Y,Z;

	int nvert;

	// max_height holds the height of the topmost block.
	// it's used for optimizations on the mesh algorithm.
	int max_height;

	std::pair<bool, GLuint> vao_cached;

	// box that encloses the chunk
	GLuint boxVao;
	ObjectMesh boxObj;


	Chunk (int x, int y , int z);
	// a dfs like function
	std::vector<float>* mesh_p;
	bool done_meshing;
	//std::vector<float>* Mesh();

	void StoreMeshCPU();

	void _mesh(int i, int j, int k, int id, int sig);
	GLuint Vao();

	void Draw(bool wire);

	void gen_terrain();
	void bake_light();
};
