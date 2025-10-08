#pragma once

#include "Chunk.hpp"
#include <list>

using std::list;

class ChunkManager
{
public:
	list<Chunk> chunks = {};
	list<ivec3> chunkPositions = {};
	list<ivec3> toGenerateList = {};

	void UpdateList(vec3 pos, float radius);
	void UnloadDistant(vec3 pos, float radius);
	void GenerateOne(vec3 pos);
};

