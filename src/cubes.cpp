#include <string.h>
#include <vector>

#include "include/perlin.hpp"
#include "include/cubes.hpp"
#include "include/graphics.hpp"

static const float A = 0.0f;
static const float B = 1.0f * (1.0f/4.0f);
static const float C = 1.0f * (2.0f/4.0f);
static const float D = 1.0f * (3.0f/4.0f);
static const float E = 1.0f;

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


Chunk::Chunk (int x, int y , int z) {
	X=x,Y=y,Z=z;
	siv::PerlinNoise perlin(1292);
	float s = 25.0f;

	memset(mat, 0, sizeof(mat));

	cache.first = false;

	for(int i=0; i<SZ; i++) {
		for(int j=0; j<SZ; j++) {
			for(int k=0; k<SZ; k++) {
				float prob = perlin.accumulatedOctaveNoise3D((i+X*32)/s, (j+Y*32)/s, (k+Z*32)/s, 2);
				int x = i - SZ/2, y = j - SZ/2, z = k - SZ/2;

				mat[i][j][k] = 1;
				//if((x*x)+(y*y)+(z*z) > 200) mat[i][j][k] = 0;
				if(prob < -0.2) mat[i][j][k]=0;
			}
		}
	}
}

// a dfs like function
std::vector<float> Chunk::Mesh() {
	memset(visited,0,sizeof(visited));
	std::vector<float> vec;

	_mesh(-1, -1, -1, 1, 0, vec);
	printf("Created %d verts, %d faces\n", ((vec.size())/5), (vec.size())/(5*6));

	return vec;
}

void Chunk::_mesh(int i, int j, int k, int id, int sig, std::vector<float>& vec) {
	if(i < -1 || j < -1 || k < -1 || i > SZ || j > SZ || k > SZ) return;

	if(i < 0 || j < 0 || k < 0 || i >= SZ || j >= SZ || k >= SZ) {

		auto proc = [](int x) -> int {
			if(x==-1) return SZ+1;
			else return x;
		};

		int ni = proc(i), nj = proc(j), nk = proc(k);

		if(visited[ni][nj][nk] == 1) return;
		visited[ni][nj][nk]=1;

		_mesh(i-1, j, k, 0, -1, vec);
		_mesh(i+1, j, k, 0, 1, vec);
		_mesh(i, j-1, k, 1, -1, vec);
		_mesh(i, j+1, k, 1, 1, vec);
		_mesh(i, j, k-1, 2, -1, vec);
		_mesh(i, j, k+1, 2, 1, vec);

		return;
	}

	int I = i + X*32, J = j + Y*32, K = k + Z*32;

	if(mat[i][j][k]==1) {
		std::vector<float> v;

		if(id==1 && sig == 1) {
			v = topFace(I, J-1, K, true);
			vec.insert(vec.end(), v.begin(), v.end());
		}

		if(id==1 && sig == -1) {
			v = topFace(I, J, K, false);
			vec.insert(vec.end(), v.begin(), v.end());
		}

		if(id==0 && sig == 1) {
			v = sideFace(I, J, K);
			vec.insert(vec.end(), v.begin(), v.end());
		}

		if(id==0 && sig == -1) {
			v = sideFace(I+1, J, K);
			vec.insert(vec.end(), v.begin(), v.end());
		}

		if(id==2 && sig == -1) {
			v = frontFace(I, J, K);
			vec.insert(vec.end(), v.begin(), v.end());
		}

		if(id==2 && sig == 1) {
			v = frontFace(I, J, K-1);
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

GLuint Chunk::Vao(Context& ctx) {
	if(cache.first == false) {

		std::vector<float> mesh = Mesh();
		cache.second = ctx.loadMeshUV(&mesh[0], mesh.size()*sizeof(float));
		nvert = mesh.size()/5; // x,y,z,u,v

		cache.first = true;
	}

	return cache.second;
}
