#include <vector>
#include <utility>

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

void World::load() {
	int I=4,J=4,K=3;

	std::vector<std::pair<int,std::pair<int,int>>> toLoad;

	for(int i=0+pos.x;i<I+pos.x;i++) {
		for(int j=0+pos.y;j<J+pos.y;j++) {
			for(int k=0+pos.z;k<K+pos.z;k++) {
				toLoad.push_back({i,{j,k}});
			}
		}
	}

	for(auto& coord : toLoad) {
		if(!loaded_chunks.count(coord)) {
			int x = coord.first,y = coord.second.first,z = coord.second.second;
			Chunk* c = new Chunk(x,y,z);
			loaded_chunks[coord] = c;
		}
	}
}

void World::Update(Camera& cam) {

	XYZ _pos = {(int)cam.ChunkPos().x,
				(int)cam.ChunkPos().y,
				(int)cam.ChunkPos().z};

	if(_pos == pos) return;
}
