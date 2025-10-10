#pragma once

#include "Chunk.hpp"
#include <list>
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"


using std::list;
using std::unordered_map;

class ChunkManager
{
public:
	std::unordered_map<ivec3, Chunk> chunks = {};
	list<ivec3> toGenerateList = {};

	void UpdateList(vec3 pos, float radius, float verticalRadius);
	void UnloadDistant(vec3 pos, float radius, float verticalRadius);
	void GenerateOne(vec3 pos);
};

