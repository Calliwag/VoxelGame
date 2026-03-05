#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include "Grid.hpp"
#include <vector>
#include "Face.hpp"
#include "SimView/SimView.hpp"
#include "Generator.hpp"
#include <string>
#include "Resources.hpp"

class Chunk
{
public:
	// General
	Grid<blockID, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> data;
	ivec3 coordinate;

	// Face grids
	Grid<blockID, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> nxyFaceGrid;
	Grid<blockID, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> pxyFaceGrid;
	Grid<blockID, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> nyzFaceGrid;
	Grid<blockID, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> pyzFaceGrid;
	Grid<blockID, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> nxzFaceGrid;
	Grid<blockID, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> pxzFaceGrid;
	void GenFaceGrids(BlockResources& blockResources);
	void GenFaceGridsSide(ivec3 neighborCoord);
    void CheckNeighborFaces(Chunk* neighbor, BlockResources& blockResources);

	// Face lists
	std::vector<Face> faces;
	void GenFaces(BlockResources& blockResources);

	VArray<float> faceVertArray;
	VArray<float> faceNormArray;
	VArray<float> faceUVArray;
	VArray<int> faceTexArray;
	int faceCount;
	bool arraysGenerated = false;
	void GenArrays();

public:
	Chunk(ivec3 coordinate);
	~Chunk();
	void TestGenChunk();
	void GenChunkLevel(int level);
	bool Gen(Generator* gen);

	blockID GetBlockGlobalID(BlockResources& blockResources, blockID blockID);

	blockID GetData(ivec3 coord);

	bool operator ==(const Chunk& other) const
	{
		return coordinate == other.coordinate;
	}
};

int taxiLen(ivec3 vec);