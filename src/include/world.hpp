#pragma once

#include <map>
#include <utility>

#include "cubes.hpp"
#include "controls.hpp"


struct World {
	std::map<std::pair<int, std::pair<int,int>>,Chunk*> loaded_chunks;
	XYZ pos;

	World(XYZ _pos);
	~World();

	void Update(Camera& cam);
	void load();

};
