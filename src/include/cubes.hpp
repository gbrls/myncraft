#pragma once

#include <vector>
#include <string.h>

#include "perlin.hpp"

static const float A = 0.0f;
static const float B = 1.0f * (1.0f/4.0f);
static const float C = 1.0f * (2.0f/4.0f);
static const float D = 1.0f * (3.0f/4.0f);
static const float E = 1.0f;
static const int SZ = 32;

std::vector<float> topFace(float x, float y, float z, bool bottom) {
	float a=A,b=B,c=C,d=D,e=E;
	if(bottom) a = c, b = d;

	//e*=10;
	return {
		-0.5f+x, 0.5f+y, 0.5f+z, a, a,
		-0.5f+x, 0.5f+y, -0.5f+z, a, e,
		0.5f+x, 0.5f+y, 0.5f+z, b, a,

		-0.5f+x, 0.5f+y, -0.5f+z, a, e,
		0.5f+x, 0.5f+y, -0.5f+z, b, e,
		0.5f+x, 0.5f+y, 0.5f+z, b, a
	};
}

std::vector<float> sideFace(float x, float y, float z) {
	float a=A,b=B,c=C,e=E;

	//e*=10;
	return {
		-0.5f+x, -0.5f+y, -0.5f+z, b, e,
		-0.5f+x, 0.5f+y, -0.5f+z, b, a,
		-0.5f+x, -0.5f+y, 0.5f+z, c, e,

		-0.5f+x, 0.5f+y, -0.5f+z, b, a,
		-0.5f+x, 0.5f+y, 0.5f+z, c, a,
		-0.5f+x, -0.5f+y, 0.5f+z, c, e
	};
}

std::vector<float> frontFace(float x, float y, float z) {
	float a=A,b=B,c=C,e=E;

	//e*=10;
	return {
		-0.5f+x, -0.5f+y, 0.5f+z, b, e,
		-0.5f+x, 0.5f+y, 0.5f+z, b, a,
		0.5f+x, -0.5f+y, 0.5f+z, c, e,

		-0.5f+x, 0.5f+y, 0.5f+z, b, a,
		0.5f+x, 0.5f+y, 0.5f+z, c, a,
		0.5f+x, -0.5f+y, 0.5f+z, c, e
	};
}

struct Chunk {
	char mat[SZ][SZ][SZ];
	char visited[SZ][SZ][SZ];

	Chunk () {
		siv::PerlinNoise perlin(100);
		float s = 10.0f;

		memset(mat, 0, sizeof(mat));

		for(int i=0; i<SZ; i++) {
			for(int j=0; j<SZ; j++) {
				for(int k=0; k<SZ; k++) {
					float prob = perlin.accumulatedOctaveNoise3D(i/s, j/s, k/s, 2);
					int x = i - SZ/2, y = j - SZ/2, z = k - SZ/2;

					mat[i][j][k] = 1;
					if((x*x)+(y*y)+(z*z) > 200) mat[i][j][k] = 0;
					if(prob < 0) mat[i][j][k]=0;
				}
			}
		}
	}

	// a dfs like function
	std::vector<float> Mesh() {
		memset(visited,0,sizeof(visited));
		std::vector<float> vec;

		_mesh(0, 0, 0, 1, 0, vec);
		printf("Created %d verts, %d faces\n", ((vec.size())/5), (vec.size())/(5*6));

		return vec;
	}

	void _mesh(int i, int j, int k, int id, int sig, std::vector<float>& vec) {
		if(i < 0 || j < 0 || k < 0 || i >= SZ || j >= SZ || k >= SZ) return;

		if(mat[i][j][k]==1) {
			std::vector<float> v;

			if(id==1 && sig == 1) {
				v = topFace(i, j-1, k, true);
				vec.insert(vec.end(), v.begin(), v.end());
			}

			if(id==1 && sig == -1) {
				v = topFace(i, j, k, false);
				vec.insert(vec.end(), v.begin(), v.end());
			}

			if(id==0 && sig == 1) {
				v = sideFace(i, j, k);
				vec.insert(vec.end(), v.begin(), v.end());
			}

			if(id==0 && sig == -1) {
				v = sideFace(i+1, j, k);
				vec.insert(vec.end(), v.begin(), v.end());
			}

			if(id==2 && sig == -1) {
				v = frontFace(i, j, k);
				vec.insert(vec.end(), v.begin(), v.end());
			}

			if(id==2 && sig == 1) {
				v = frontFace(i, j, k-1);
				vec.insert(vec.end(), v.begin(), v.end());
			}

			return;
		}

		if(visited[i][j][k] == 1) return;
		visited[i][j][k]=1;

		_mesh(i-1, j, k, 0, -1, vec);
		_mesh(i+1, j, k, 0, 1, vec);
		_mesh(i, j-1, k, 1, -1, vec);
		_mesh(i, j+1, k, 1, 1, vec);
		_mesh(i, j, k-1, 2, -1, vec);
		_mesh(i, j, k+1, 2, 1, vec);
	}
};
