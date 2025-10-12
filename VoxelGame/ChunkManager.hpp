#pragma once

#include "Chunk.hpp"
#include <list>
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include "Generator.hpp"
#include <queue>
#include <vector>

using std::list;
using std::unordered_map;
using std::priority_queue;

class Compare
{
public:
	bool operator() (ivec3 vec1, ivec3 vec2)
	{
		return (vec1.x * vec1.x + vec1.y * vec1.y + vec1.z * vec1.z) > (vec2.x * vec2.x + vec2.y * vec2.y + vec2.z * vec2.z);
	}
};

class ChunkManager
{
public:
	std::unordered_map<ivec3, Chunk> chunks = {};
	//list<ivec3> toGenerateList = {};
	priority_queue<ivec3, std::vector<ivec3>, Compare> toGenerateList = {};
	ivec3 toGenerateCenter;
	Generator* gen;

	ChunkManager(Generator* gen) : gen(gen) {}
	void UpdateList(vec3 pos, float radius, float verticalRadius);
	void UnloadDistant(vec3 pos, float radius, float verticalRadius);
	void GenerateOne(vec3 pos);

	ivec3 GetLastEmptyBlockOnRay(vec3 start, vec3 end, bool& success);
};

