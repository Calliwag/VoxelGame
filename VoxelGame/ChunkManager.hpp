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

class RayIter;

class ChunkManager
{
public:
	std::unordered_map<ivec3, Chunk> chunks = {};
	//list<ivec3> toGenerateList = {};
	priority_queue<ivec3, std::vector<ivec3>, Compare> toGenerateList = {};
	ivec3 toGenerateCenter = { 0,0,0 };
	Generator* gen;

	ChunkManager(Generator* gen) : gen(gen) {}
	void UpdateList(vec3 pos, float radius, float verticalRadius);
	void UnloadDistant(vec3 pos, float radius, float verticalRadius);
	void GenerateOne(vec3 pos);
	void MeshChunk(ivec3 coord);
	void MeshChunkModified(ivec3 coord);

	RayIter GetRayIter(vec3 start, vec3 dir);
};

class RayIter
{
public:
	ChunkManager& cm;
	ivec3 pos;
	ivec3 steps;
	vec3 cellDist;
	vec3 deltaDist;

	// Important stuff
	Chunk* chunk;
	ivec3 blockCoord;
	float faceDist;
	int face;

	RayIter(ChunkManager& cm, vec3 pos, vec3 dir);
	void Next();
};