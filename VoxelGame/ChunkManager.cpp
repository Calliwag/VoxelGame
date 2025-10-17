#include "ChunkManager.hpp"
#include <algorithm>
#include <limits>

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
	chunks.insert({ coord, chunk });
	MeshChunk(chunk.coordinate);
	toGenerateList.pop();
}

void ChunkManager::MeshChunk(ivec3 coord)
{
	if (!chunks.contains(coord))
		return;

	auto& chunk = chunks.at(coord);
	chunk.GenFaceGrids();

	if (chunks.contains(coord + ivec3{ 1,0,0 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ 1,0,0 });
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFaces();
	}
	if (chunks.contains(coord + ivec3{ -1,0,0 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ -1,0,0 });
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFaces();
	}

	if (chunks.contains(coord + ivec3{ 0,1,0 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ 0,1,0 });
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFaces();
	}
	if (chunks.contains(coord + ivec3{ 0,-1,0 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ 0,-1,0 });
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFaces();
	}

	if (chunks.contains(coord + ivec3{ 0,0,1 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ 0,0,1 });
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFaces();
	}
	if (chunks.contains(coord + ivec3{ 0,0,-1 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ 0,0,-1 });
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFaces();
	}

	chunk.GenFaces();
}

void ChunkManager::MeshChunkModified(ivec3 coord)
{
	if (!chunks.contains(coord))
		return;

	auto& chunk = chunks.at(coord);
	chunk.GenFaceGrids();

	if (chunks.contains(coord + ivec3{ 1,0,0 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ 1,0,0 });
		neighbor.GenFaceGrids();
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFaces();
	}
	if (chunks.contains(coord + ivec3{ -1,0,0 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ -1,0,0 });
		neighbor.GenFaceGrids();
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFaces();
	}

	if (chunks.contains(coord + ivec3{ 0,1,0 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ 0,1,0 });
		neighbor.GenFaceGrids();
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFaces();
	}
	if (chunks.contains(coord + ivec3{ 0,-1,0 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ 0,-1,0 });
		neighbor.GenFaceGrids();
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFaces();
	}

	if (chunks.contains(coord + ivec3{ 0,0,1 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ 0,0,1 });
		neighbor.GenFaceGrids();
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFaces();
	}
	if (chunks.contains(coord + ivec3{ 0,0,-1 }))
	{
		auto& neighbor = chunks.at(coord + ivec3{ 0,0,-1 });
		neighbor.GenFaceGrids();
		chunk.CheckNeighborFaces(&neighbor);
		neighbor.CheckNeighborFaces(&chunk);
		neighbor.GenFaces();
	}

	chunk.GenFaces();
}

RayIter ChunkManager::GetRayIter(vec3 start, vec3 dir)
{
	float step;
	if (abs(dir.x) >= abs(dir.y) && abs(dir.x) >= abs(dir.z))
		step = abs(dir.x);
	else if (abs(dir.y) >= abs(dir.z))
		step = abs(dir.y);
	else
		step = abs(dir.z);

	dir /= step;

	return RayIter(*this, start, dir);
}

ivec3 ChunkManager::GetBlockCoord(vec3 pos)
{
	return ivec3{
		(((int)pos.x % CHUNK_SPAN) + CHUNK_SPAN) % CHUNK_SPAN,
		(((int)pos.y % CHUNK_SPAN) + CHUNK_SPAN) % CHUNK_SPAN,
		(((int)pos.z % CHUNK_SPAN) + CHUNK_SPAN) % CHUNK_SPAN
	};
}

ivec3 ChunkManager::GetChunkCoord(vec3 pos)
{
	return glm::floor(pos / (float)CHUNK_SPAN);
}

void ChunkManager::SetBlock(ivec3 coord, u8 value)
{
	auto chunkCoord = GetChunkCoord(coord);
	if (!chunks.contains(chunkCoord))
		return;
	auto blockCoord = GetBlockCoord(coord);
	chunks.at(chunkCoord).data.At(blockCoord) = value;
	MeshChunkModified(chunkCoord);
}

bool ChunkManager::IsChunkLoaded(ivec3 chunkCoord)
{
	return chunks.contains(chunkCoord);
}

bool ChunkManager::IsBlockSolid(ivec3 worldCoordinate)
{
	ivec3 chunkCoord = GetChunkCoord(worldCoordinate);
	if (!chunks.contains(chunkCoord))
		return false;
	ivec3 blockCoord = GetBlockCoord(worldCoordinate);
	return chunks.at(chunkCoord).data.At(blockCoord) != 0;
}

AABB ChunkManager::GetBlockAABB(ivec3 worldCoordinate)
{
	return AABB::NegativeAligned(worldCoordinate, {1,1,1});
}

RayIter::RayIter(ChunkManager& cm, vec3 pos, vec3 dir) : cm(cm)
{
	dir = normalize(dir);

	// Current voxel position (integer)
	this->pos = ivec3(floor(pos));

	steps = { sign(dir.x), sign(dir.y), sign(dir.z) };

	deltaDist.x = (dir.x == 0) ? std::numeric_limits<float>::max() : abs(1.0f / dir.x);
	deltaDist.y = (dir.y == 0) ? std::numeric_limits<float>::max() : abs(1.0f / dir.y);
	deltaDist.z = (dir.z == 0) ? std::numeric_limits<float>::max() : abs(1.0f / dir.z);

	// Calculate distance to next voxel boundary from startPos
	if (dir.x < 0)
		cellDist.x = (pos.x - this->pos.x) * deltaDist.x;
	else
		cellDist.x = (this->pos.x + 1.0f - pos.x) * deltaDist.x;

	if (dir.y < 0)
		cellDist.y = (pos.y - this->pos.y) * deltaDist.y;
	else
		cellDist.y = (this->pos.y + 1.0f - pos.y) * deltaDist.y;

	if (dir.z < 0)
		cellDist.z = (pos.z - this->pos.z) * deltaDist.z;
	else
		cellDist.z = (this->pos.z + 1.0f - pos.z) * deltaDist.z;

	ivec3 chunkCoord = glm::floor(vec3(this->pos) / (float)CHUNK_SPAN);
	if (!cm.chunks.contains(chunkCoord))
	{
		chunk = nullptr;
	}
	else
	{
		chunk = &cm.chunks.at(chunkCoord);
	}
	blockCoord = {
		((this->pos.x % CHUNK_SPAN) + CHUNK_SPAN) % CHUNK_SPAN,
		((this->pos.y % CHUNK_SPAN) + CHUNK_SPAN) % CHUNK_SPAN,
		((this->pos.z % CHUNK_SPAN) + CHUNK_SPAN) % CHUNK_SPAN
	};
}

void RayIter::Next()
{
	if (cellDist.x < cellDist.y && cellDist.x < cellDist.z) // YZ faces
	{
		cellDist.x += deltaDist.x;
		pos.x += steps.x;
		face = steps.x == 1 ? 2 : 3;
		faceNorm = steps.x == 1 ? vec3{-1, 0, 0} : vec3{1, 0, 0};
		faceDist = cellDist.x - deltaDist.x;
	}
	else if (cellDist.y < cellDist.z) // XZ faces
	{
		cellDist.y += deltaDist.y;
		pos.y += steps.y;
		face = steps.y == 1 ? 4 : 5;
		faceNorm = steps.y == 1 ? vec3{ 0, -1, 0 } : vec3{ 0, 1, 0 };
		faceDist = cellDist.y - deltaDist.y;
	}
	else // XY faces
	{
		cellDist.z += deltaDist.z;
		pos.z += steps.z;
		face = steps.z == 1 ? 0 : 1;
		faceNorm = steps.z == 1 ? vec3{ 0, 0, -1 } : vec3{ 0, 0, 1 };
		faceDist = cellDist.z - deltaDist.z;
	}

	ivec3 chunkCoord = glm::floor(vec3(pos) / (float)CHUNK_SPAN);
	if (!cm.chunks.contains(chunkCoord))
	{
		chunk = nullptr;
	}
	else
	{
		chunk = &cm.chunks.at(chunkCoord);
	}
	blockCoord = {
		((pos.x % CHUNK_SPAN) + CHUNK_SPAN) % CHUNK_SPAN,
		((pos.y % CHUNK_SPAN) + CHUNK_SPAN) % CHUNK_SPAN,
		((pos.z % CHUNK_SPAN) + CHUNK_SPAN) % CHUNK_SPAN
	};
}