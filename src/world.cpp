#include <vector>
#include <SDL2/SDL.h>
#include <utility>
#include <thread>
#include <future>

#include "include/world.hpp"
#include "include/cubes.hpp"
#include "include/controls.hpp"

static void create_chunk(Chunk* c) {
    c->gen_terrain();
    c->StoreMeshCPU();
}

static void _thread_fn(World* w) {
    auto v = true;
    unsigned int t;
    while(v) {
        if(!w->loader_queue.empty()) {
            t = SDL_GetTicks();
            create_chunk(w->loader_queue.front());
            printf("%lu vertices loaded in %d ms\n", w->loader_queue.front()->mesh_p->size(), SDL_GetTicks()-t);
            w->loader_queue.pop();
        }
    }
}

World::World(XYZ _pos, Camera& cam) {
	pos = _pos;
	load(cam);
	loader_thread = std::make_unique<std::thread> (_thread_fn, this);
}

World::~World() {
	for(auto const& p : loaded_chunks) {
		delete p.second;
	}

	// this allow us to safely delete it
	loader_thread->detach();
}

void World::load(Camera& cam) {
	int I=2,J=1,K=2;

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
		    loader_queue.push(c);
		}
	}
}

void World::Update(Camera& cam) {

    auto cpos = cam.ChunkPos();
	XYZ _pos = {(int)floor(cpos.x),
                (int)floor(cpos.y),
                (int)floor(cpos.z)};

	if(_pos == pos) return;


	printf("Moved from (%d, %d, %d) to (%d, %d, %d)\n",
		   pos.x,pos.y,pos.z,_pos.x,_pos.y,_pos.z);
	pos = _pos;


	load(cam);
}

