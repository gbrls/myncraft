#include <vector>
#include <utility>
#include <thread>
#include <future>

#include "include/world.hpp"
#include "include/cubes.hpp"
#include "include/controls.hpp"

World::World(XYZ _pos) {
	pos = _pos;
	load();
}

World::~World() {
	for(auto const& p : loaded_chunks) {
		delete p.second;
	}
}

//static void create_chunk(std::pair<int,std::pair<int,int>> coord, std::map<std::pair<int, std::pair<int,int>>,Chunk*> loaded_chunks) {
//	int x = coord.first,y = coord.second.first,z = coord.second.second;
//	Chunk* c;
//	c = new Chunk(x,y,z);
//	loaded_chunks[coord] = c;
//}

static void create_chunk(Chunk* c) {
	c->StoreMeshCPU();
}

void World::load() {
	int I=1,J=1,K=1;

	std::vector<std::pair<int,std::pair<int,int>>> toLoad;

	for(int i=-I;i<=I;i++) {
		for(int j=-J;j<=J;j++) {
			for(int k=-K;k<=K;k++) {
				toLoad.push_back({i+pos.x,{j+pos.y,k+pos.z}});
			}
		}
	}

	for(auto& coord : toLoad) {
		if(!loaded_chunks.count(coord)) {

			int x = coord.first,y = coord.second.first,z = coord.second.second;
			Chunk* c;
			c = new Chunk(x,y,z);
			loaded_chunks[coord] = c;
			c_futures.push_back(std::async(std::launch::async, create_chunk, c));
		}
	}
}

void World::Update(Camera& cam) {

	XYZ _pos = {(int)cam.ChunkPos().x,
				(int)cam.ChunkPos().y,
				(int)cam.ChunkPos().z};

	if(_pos == pos) return;


	printf("Moved from (%d, %d, %d) to (%d, %d, %d)\n",
		   pos.x,pos.y,pos.z,_pos.x,_pos.y,_pos.z);
	pos = _pos;


	load();
}
