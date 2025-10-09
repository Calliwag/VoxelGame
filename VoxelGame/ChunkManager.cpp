#include "ChunkManager.hpp"
#include <algorithm>

void ChunkManager::UpdateList(vec3 pos, float radius)
{
	toGenerateList = {};
	vec3 iPos = pos / (float)CHUNK_SPAN;
	float iRadius = radius / (float)CHUNK_SPAN;
	for(int x = iPos.x - iRadius; x < iPos.x + iRadius; x++)
		for (int y = iPos.y - iRadius; y < iPos.y + iRadius; y++)
			for (int z = iPos.z - iRadius; z < iPos.z + iRadius; z++)
			{
				ivec3 coord = { x,y,z };
				if (!chunks.contains(coord))
				{
					if (length((vec3)coord - iPos) < iRadius)
					{
						toGenerateList.push_back(coord);
					}
				}
			}
}

void ChunkManager::UnloadDistant(vec3 pos, float radius)
{
	vec3 iPos = pos / (float)CHUNK_SPAN;
	float iRadius = radius / (float)CHUNK_SPAN;
	auto it = chunks.begin();
	while (it != chunks.end())
	{
		ivec3 coord = it->first;
		if(length((vec3)coord - iPos) > iRadius)
		{
			chunks.erase(it++);
		}
		else
		{
			it++;
		}
	}
}

void ChunkManager::GenerateOne(vec3 pos)
{
	if (toGenerateList.size() == 0)
		return;
	ivec3 iPos = pos / (float)CHUNK_SPAN;
	ivec3 coord = *toGenerateList.begin();
	int minDist = taxiLen(coord - iPos);
	for (auto& toGen : toGenerateList)
	{
		int dist = taxiLen(toGen - iPos);
		if (dist < minDist)
		{
			coord = toGen;
			minDist = dist;
		}
	}

	Chunk chunk(coord);
	chunk.GenChunkLevel(-8);
	chunk.GenFaceGrids();

	if (chunks.contains(coord + ivec3{ 1,0,0 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ 1,0,0 });
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFacesSimple();
	}
	if (chunks.contains(coord + ivec3{ -1,0,0 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ -1,0,0 });
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFacesSimple();
	}

	if (chunks.contains(coord + ivec3{ 0,1,0 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ 0,1,0 });
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFacesSimple();
	}
	if (chunks.contains(coord + ivec3{ 0,-1,0 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ 0,-1,0 });
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFacesSimple();
	}

	if (chunks.contains(coord + ivec3{ 0,0,1 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ 0,0,1 });
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFacesSimple();
	}
	if (chunks.contains(coord + ivec3{ 0,0,-1 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ 0,0,-1 });
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFacesSimple();
	}

	chunk.GenFacesSimple();

	chunks.insert({ coord, chunk });
	toGenerateList.remove(coord);
}
