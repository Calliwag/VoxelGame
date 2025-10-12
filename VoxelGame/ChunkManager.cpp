#include "ChunkManager.hpp"
#include <algorithm>

void ChunkManager::UpdateList(vec3 pos, float radius, float verticalRadius)
{
	vec3 iPos = pos / (float)CHUNK_SPAN;
	if ((ivec3)iPos == toGenerateCenter && toGenerateList.size() != 0)
		return;
	toGenerateList = {};
	float iRadius = radius / (float)CHUNK_SPAN;
	float ivRadius = verticalRadius / (float)CHUNK_SPAN;
	for(int x = iPos.x - iRadius; x < iPos.x + iRadius; x++)
		for (int y = iPos.y - iRadius; y < iPos.y + iRadius; y++)
			for (int z = iPos.z - ivRadius; z < iPos.z + ivRadius; z++)
			{
				ivec3 coord = { x,y,z };
				if (!chunks.contains(coord))
				{
					if (length((vec3)coord - iPos) < iRadius)
					{
						toGenerateList.push(coord - (ivec3)iPos);
					}
				}
			}
	toGenerateCenter = iPos;
}

void ChunkManager::UnloadDistant(vec3 pos, float radius, float verticalRadius)
{
	vec3 iPos = pos / (float)CHUNK_SPAN;
	float iRadius = radius / (float)CHUNK_SPAN;
	float ivRadius = verticalRadius / (float)CHUNK_SPAN;
	auto it = chunks.begin();
	while (it != chunks.end())
	{
		ivec3 coord = it->first;
		if(length((vec3)coord - iPos) > iRadius || abs(coord.z - iPos.z) > ivRadius)
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
	ivec3 coord = toGenerateList.top() + toGenerateCenter;

	Chunk chunk(coord);
	bool empty = !chunk.Gen(gen);
	chunk.GenFaceGrids();

	if (!empty)
	{
		if (chunks.contains(coord + ivec3{ 1,0,0 }))
		{
			auto& neighbor = chunks.at(coord + ivec3{ 1,0,0 });
			chunk.CheckNeighborFaces(&neighbor);
			neighbor.CheckNeighborFaces(&chunk);
			neighbor.GenFacesGreedy();
		}
		if (chunks.contains(coord + ivec3{ -1,0,0 }))
		{
			auto& neighbor = chunks.at(coord + ivec3{ -1,0,0 });
			chunk.CheckNeighborFaces(&neighbor);
			neighbor.CheckNeighborFaces(&chunk);
			neighbor.GenFacesGreedy();
		}

		if (chunks.contains(coord + ivec3{ 0,1,0 }))
		{
			auto& neighbor = chunks.at(coord + ivec3{ 0,1,0 });
			chunk.CheckNeighborFaces(&neighbor);
			neighbor.CheckNeighborFaces(&chunk);
			neighbor.GenFacesGreedy();
		}
		if (chunks.contains(coord + ivec3{ 0,-1,0 }))
		{
			auto& neighbor = chunks.at(coord + ivec3{ 0,-1,0 });
			chunk.CheckNeighborFaces(&neighbor);
			neighbor.CheckNeighborFaces(&chunk);
			neighbor.GenFacesGreedy();
		}

		if (chunks.contains(coord + ivec3{ 0,0,1 }))
		{
			auto& neighbor = chunks.at(coord + ivec3{ 0,0,1 });
			chunk.CheckNeighborFaces(&neighbor);
			neighbor.CheckNeighborFaces(&chunk);
			neighbor.GenFacesGreedy();
		}
		if (chunks.contains(coord + ivec3{ 0,0,-1 }))
		{
			auto& neighbor = chunks.at(coord + ivec3{ 0,0,-1 });
			chunk.CheckNeighborFaces(&neighbor);
			neighbor.CheckNeighborFaces(&chunk);
			neighbor.GenFacesGreedy();
		}

		chunk.GenFacesGreedy();
	}

	chunks.insert({ coord, chunk });
	toGenerateList.pop();
}

ivec3 ChunkManager::GetLastEmptyBlockOnRay(vec3 start, vec3 end, bool& success)
{
	float dx = end.x - start.x;
	float dy = end.y - start.y;
	float dz = end.z - start.z;

	float step;

	if (abs(dx) >= abs(dy) && abs(dx) >= abs(dz))
	{
		step = abs(dx);
	}
	else if (abs(dy) >= abs(dz))
	{
		step = abs(dy);
	}
	else
	{
		step = abs(dz);
	}

	dx /= step;
	dy /= step;
	dz /= step;

	vec3 pos = start;
	int i = 0;
	while (i < step)
	{
		ivec3 cPos = glm::floor(pos / (float)CHUNK_SPAN);
		if (!chunks.contains(cPos))
		{
			success = false;
			return ivec3();
		}
		Chunk& chunk = chunks.at(cPos);
		ivec3 bPos = glm::mod(pos, (float)CHUNK_SPAN);
		auto& type = chunk.data.At(bPos);
		if (type != 0)
		{
			success = true;
			pos -= vec3{dx, dy, dz};
			return pos;
		}
		pos += vec3{ dx,dy,dz };
		i++;
	}
	success = false;
}
