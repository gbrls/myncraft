#include <cstdlib>
#include <string.h>
#include <vector>
#include <cstdio>
#include <queue>
#include <utility>
#include <cmath>
#include <functional>

#include <GL/glew.h>
#include <GL/gl.h>

#include "include/perlin.hpp"
#include "include/cubes.hpp"
#include "include/graphics.hpp"
#include "include/object_mesh.hpp"

/*
 * The mesh has little vertices and its fast to render,
 * but it's costly to generate it.
 */

static const float A = 0.0f;
static const float B = 1.0f * (1.0f/4.0f);
static const float C = 1.0f * (2.0f/4.0f);
static const float D = 1.0f * (3.0f/4.0f);
static const float E = 1.0f;

static std::vector<float> topFace(float x, float y, float z, bool bottom, float idx, float n, float l) {
	float a=A,b=B,c=C,d=D,e=E;
	if(bottom) a = c, b = d;

	return {
		-0.5f+x, 0.5f+y, 0.5f+z, a, a, idx, n, l,
		-0.5f+x, 0.5f+y, -0.5f+z, a, e, idx, n, l,
		0.5f+x, 0.5f+y, 0.5f+z, b, a, idx, n, l,

		-0.5f+x, 0.5f+y, -0.5f+z, a, e, idx, n, l,
		0.5f+x, 0.5f+y, -0.5f+z, b, e, idx, n, l,
		0.5f+x, 0.5f+y, 0.5f+z, b, a, idx, n, l
	};
}

static std::vector<float> sideFace(float x, float y, float z, float idx, float n, float l) {
	float a=A,b=B,c=C,e=E;

	return {
		-0.5f+x, -0.5f+y, -0.5f+z, b, e, idx, n, l,
		-0.5f+x, 0.5f+y, -0.5f+z, b, a, idx, n, l,
		-0.5f+x, -0.5f+y, 0.5f+z, c, e, idx, n, l,

		-0.5f+x, 0.5f+y, -0.5f+z, b, a, idx, n, l,
		-0.5f+x, 0.5f+y, 0.5f+z, c, a, idx, n, l,
		-0.5f+x, -0.5f+y, 0.5f+z, c, e, idx, n, l
	};
}

static std::vector<float> frontFace(float x, float y, float z, float idx, float n, float l) {
	float a=A,b=B,c=C,e=E;

	return {
		-0.5f+x, -0.5f+y, 0.5f+z, b, e, idx, n, l,
		-0.5f+x, 0.5f+y, 0.5f+z, b, a, idx, n, l,
		0.5f+x, -0.5f+y, 0.5f+z, c, e, idx, n, l,

		-0.5f+x, 0.5f+y, 0.5f+z, b, a, idx, n, l,
		0.5f+x, 0.5f+y, 0.5f+z, c, a, idx, n, l,
		0.5f+x, -0.5f+y, 0.5f+z, c, e, idx, n, l
	};
}

// returns a box that encloses the chunk
//static GLuint box(float X, float Y, float Z) {
//	X *= SZ, Y*=SZ, Z*=SZ;
//	std::vector<float> mesh = {
//            X + SZ, Y + SZ, Z + SZ, 0, 0, 0, 0, 10,
//            X + SZ, Y + SZ, Z + 0, 0, 0, 0, 0, 10,
//            X + SZ, Y + 0, Z + 0, 0, 0, 0, 0, 10,
//            X + SZ, Y + 0, Z + SZ, 0, 0, 0, 0, 10,
//            X + 0, Y + 0, Z + SZ, 0, 0, 0, 0, 10,
//            X + 0, Y + 0, Z + 0, 0, 0, 0, 0, 10,
//            X + 0, Y + SZ, Z + 0, 0, 0, 0, 0, 10,
//            X + 0, Y + SZ, Z + SZ, 0, 0, 0, 0, 10 };
//
//	return loadMeshUV(&mesh[0], mesh.size()*sizeof(float));
//}

static GLuint square(float x, float y, float z) {
    x *= SZ, y *= SZ, z *= SZ;
    std::vector<float> mesh = {
            -0.5f+x, -0.5f+y, -0.5f+z,  0.0f, 0.0f, 0.0f, 0.0f, 00.0f,
            0.5f +x, -0.5f+y, -0.5f+z,  1.0f, 0.0f, 0.0f, 0.0f, 00.0f,
            0.5f +x,  0.5f+y, -0.5f+z,  1.0f, 1.0f, 0.0f, 0.0f, 00.0f,
            0.5f +x,  0.5f+y, -0.5f+z,  1.0f, 1.0f, 0.0f, 0.0f, 00.0f,
            -0.5f+x,  0.5f+y, -0.5f+z,  0.0f, 1.0f, 0.0f, 0.0f, 00.0f,
            -0.5f+x, -0.5f+y, -0.5f+z,  0.0f, 0.0f, 0.0f, 0.0f, 00.0f,

            -0.5f+x, -0.5f+y,  0.5f+z,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f +x, -0.5f+y,  0.5f+z,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f +x,  0.5f+y,  0.5f+z,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.5f +x,  0.5f+y,  0.5f+z,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f+x,  0.5f+y,  0.5f+z,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f+x, -0.5f+y,  0.5f+z,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

            -0.5f+x,  0.5f+y,  0.5f+z,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f+x,  0.5f+y, -0.5f+z,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f+x, -0.5f+y, -0.5f+z,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f+x, -0.5f+y, -0.5f+z,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f+x, -0.5f+y,  0.5f+z,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f+x,  0.5f+y,  0.5f+z,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

            0.5f+x,  0.5f+y,  0.5f+z,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f+x,  0.5f+y, -0.5f+z,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.5f+x, -0.5f+y, -0.5f+z,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.5f+x, -0.5f+y, -0.5f+z,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.5f+x, -0.5f+y,  0.5f+z,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f+x,  0.5f+y,  0.5f+z,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

            -0.5f+x, -0.5f+y, -0.5f+z,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.5f +x, -0.5f+y, -0.5f+z,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.5f +x, -0.5f+y,  0.5f+z,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f +x, -0.5f+y,  0.5f+z,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f+x, -0.5f+y,  0.5f+z,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f+x, -0.5f+y, -0.5f+z,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

            -0.5f+x,  0.5f+y, -0.5f+z,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.5f +x,  0.5f+y, -0.5f+z,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.5f +x,  0.5f+y,  0.5f+z,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f +x,  0.5f+y,  0.5f+z,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f+x,  0.5f+y,  0.5f+z,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f+x,  0.5f+y, -0.5f+z,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };

	return loadMeshUV(&mesh[0], mesh.size()*sizeof(float));
}

Chunk::Chunk (int x, int y , int z) {
	X=x,Y=y,Z=z;
	//memset(mat, 0, sizeof(mat));
	vao_cached.first = false;

	mesh_p = NULL;

	//boxVao = square(x,y,z);
    //boxObj = ObjectMesh();
    //boxObj = ObjectMesh(square(x+x/2,y+y/2,z+z/2));
    //boxVao = square(x+0.5,y+0.5,z+0.5);

	//gen_terrain();
	done_meshing = false;
	//StoreMeshCPU();
	//TODO: //bake_light();
}

int tree(float _seed) {
	float seed = sinf(_seed*2000.87f + 10.0f);
	seed += 1.0f;
	seed *= 100;


	if(seed > 199.99f) {
		int s = ((int)(_seed * 20.0f))*19;
		int h = (s*23)%5 + 3;
		return h;
	}

	return 0;
}

void Chunk::gen_terrain() {

    max_height = 0;

	siv::PerlinNoise perlin(42242421);
	float s = 200.0f;

	const int ROCK_LEVEL = 20;

	for(int i=-1;i<SZ+1;i++) {
		for(int j=-1;j<SZ+1;j++) {
			float H = perlin.accumulatedOctaveNoise2D((i+X*32)/s, (j+Z*32)/s, 4);
			H += 1;
			H *= 40;

			for(int k=-1;k<SZ+1;k++) {

			    mat[i+1][k+1][j+1].type = 0;

				int tree_height = tree(H), y_coord = k+Y*32;
				if(y_coord < H) { // placing block
				    max_height = std::max(max_height, k+1);
					mat[i+1][k+1][j+1].type = 1;
					if(y_coord < ROCK_LEVEL) mat[i+1][k+1][j+1].type = 4;
				} else if(tree_height) { // placing wood
					int tip =  H + tree_height;
					if( y_coord < tip) {
                        max_height = std::max(max_height, k+1);
						mat[i+1][k+1][j+1].type = 2;
					} else {

						auto val = [](int x) -> bool {
							return (x >= 0 && x < SZ);
						};

						//placing leaves
						for(int x=-5; x<5;x++) {
							for(int y=0;y<5;y++) {
								int my = (y+y_coord)-tip;
								for(int z=-5; z<5;z++) {
									if((x*x + my*my + z*z < 10) && val(i+x) && val(y+k) && val(z+j)) {
                                        max_height = std::max(max_height, y+k);
									    mat[i+x+1][y+k+1][z+j+1].type = 3;
									}
								}
							}
						}

					}
				}
			}
		}
	}

}

void Chunk::bake_light() {

	auto val = [](int x) -> int {
		return std::max(std::min(x, SZ-1), 0);
	};

	for(int i=0; i<SZ;i++) {
		for(int j=0;j<SZ;j++) {
			for(int k=SZ-1; k>=0;k--) {

				mat[val(i+1)][k][j].light += 4;
				mat[i][k][val(j+1)].light += 4;
				mat[val(i-1)][k][j].light += 4;
				mat[i][k][val(j-1)].light += 4;

				mat[val(i+1)][val(k+1)][j].light += 4;
				mat[i][val(k+1)][val(j+1)].light += 4;
				mat[val(i-1)][k][j].light += 4;
				mat[i][val(k+1)][val(j-1)].light += 4;

				if(mat[i][k][j].type > 0) {
					mat[i][k][j].light = 10;
					break;
				}
			}
		}
	}
}

void Chunk::StoreMeshCPU() {
	if(mesh_p != NULL) return;
	done_meshing = false;
	mesh_p = new std::vector<float>;
	_mesh(-1, 0, 0, 0, 0);
	done_meshing = true;
}

void Chunk::_mesh(int i, int j, int k, int id, int sig) {

    if(max_height == 0) return;

	std::queue<std::pair<XYZ, std::pair<int, int>>> q;

	q.push({{i,j,k}, {id, sig}});

	auto proc = [](int x) -> int {
	    return x+1;
	};

	while(!q.empty()) {
		i = q.front().first.x, j = q.front().first.y, k = q.front().first.z;
		id = q.front().second.first, sig = q.front().second.second;

		q.pop();

        if(j > max_height+2) continue;
		if(i < -1 || j < -1 || k < -1 || i > SZ || j > SZ || k > SZ) continue;

		int ni = proc(i), nj = proc(j), nk = proc(k);


		if(i < 0 || j < 0 || k < 0 || i >= SZ || j >= SZ || k >= SZ) {

			if(visited[ni][nj][nk] == 1) continue;
			visited[ni][nj][nk]=1;

			q.push({{i-1, j, k},{ 0, -1}});
			q.push({{i+1, j, k},{0, 1}});
			q.push({{i, j-1, k},{1, -1}});

			q.push({{i, j+1, k},{1, 1}});
			q.push({{i, j, k-1},{ 2, -1}});
			q.push({{i, j, k+1},{2, 1}});

			continue;
		}


		int I = i + X*32, J = j + Y*32, K = k + Z*32;

		if(mat[i+1][j+1][k+1].type >0) {
			std::vector<float> v;

			float idx = mat[i+1][j+1][k+1].type-1, light = mat[i+1][j+1][k+1].light;
			//light = 2;

			if(id==1 && sig == 1) {
				// bottom
				v = topFace(I, J-1, K, true, idx, 5, light);
				mesh_p->insert(mesh_p->end(), v.begin(), v.end());
			}

			if(id==1 && sig == -1) {
				// top
				v = topFace(I, J, K, false, idx, 4, light);
				mesh_p->insert(mesh_p->end(), v.begin(), v.end());
			}

			if(id==0 && sig == 1) {
				// left
				v = sideFace(I, J, K, idx, 1, light);
				mesh_p->insert(mesh_p->end(), v.begin(), v.end());
			}

			if(id==0 && sig == -1) {
				// right
				v = sideFace(I+1, J, K, idx, 3, light);
				mesh_p->insert(mesh_p->end(), v.begin(), v.end());
			}

			if(id==2 && sig == -1) {
				// front
				v = frontFace(I, J, K, idx, 0, light);
				mesh_p->insert(mesh_p->end(), v.begin(), v.end());
			}

			if(id==2 && sig == 1) {
				//back
				v = frontFace(I, J, K-1, idx, 1, light);
				mesh_p->insert(mesh_p->end(), v.begin(), v.end());
			}

			continue;
		}

		if(visited[ni][nj][nk] == 1) continue;
		visited[ni][nj][nk]=1;

		q.push({{i-1, j, k},{ 0, -1}});
		q.push({{i+1, j, k},{0, 1}});
		q.push({{i, j-1, k},{1, -1}});

		q.push({{i, j+1, k},{1, 1}});
		q.push({{i, j, k-1},{ 2, -1}});
		q.push({{i, j, k+1},{2, 1}});
	}
}

GLuint Chunk::Vao() {
	if(!vao_cached.first) {

		if(mesh_p == NULL || !done_meshing || max_height == 0) {
			return -1;
		}
		vao_cached.second = loadMeshUV(&(*mesh_p)[0], mesh_p->size()*sizeof(float));
		nvert = mesh_p->size()/8; // x,y,z,u,v,n,l
		vao_cached.first = true;

		delete mesh_p;
		mesh_p = NULL;
	}

	return vao_cached.second;
}

void Chunk::Draw(bool wire) {
	auto vao = Vao();

	bool anything_to_draw = (vao != -1) && (nvert > 0);

	if(anything_to_draw) {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, nvert);
    }

	//if(wire) {
	//	glBindVertexArray(boxVao);
	//	glLineWidth(10);
	//	glDrawArrays(GL_LINE_STRIP, 0, 8);
	//	glLineWidth(1);
	//}

	//if(wire) {
    //    glBindVertexArray(boxObj.vao);
    //    glDrawArrays(GL_TRIANGLES, 0, boxObj.nvert);
    //}

    //if(wire) {
    //    glBindVertexArray(boxVao);
    //    glDrawArrays(GL_TRIANGLES, 0, 36);
    //}
}
