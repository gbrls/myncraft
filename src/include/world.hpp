#pragma once

#include <map>
#include <utility>
#include <future>
#include <vector>
#include <queue>

#include "cubes.hpp"
#include "controls.hpp"


struct World {
	std::map<std::pair<int, std::pair<int,int>>,Chunk*> loaded_chunks;
	std::unique_ptr<std::thread> loader_thread;
	XYZ pos;

	std::queue<Chunk*> loader_queue;

	World(XYZ _pos, Camera& cam);
	~World();

	void Update(Camera& cam);
	void load(Camera& cam);
};
