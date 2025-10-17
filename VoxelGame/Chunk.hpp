#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include "Grid.hpp"
#include <vector>
#include "Face.hpp"
#include "SimView/SimView.hpp"
#include "Generator.hpp"
#include <string>
#include "TexData.hpp"

class Chunk
{
public:
	// General
	Grid<blockType, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> data;
	ivec3 coordinate;

	// Face grids
	Grid<blockType, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> nxyFaceGrid;
	Grid<blockType, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> pxyFaceGrid;

	Grid<blockType, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> nyzFaceGrid;
	Grid<blockType, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> pyzFaceGrid;

	Grid<blockType, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> nxzFaceGrid;
	Grid<blockType, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> pxzFaceGrid;
	void GenFaceGrids();
	void GenFaceGridsSide(ivec3 neighborCoord);
    void CheckNeighborFaces(Chunk* neighbor);

	// Face lists
	std::vector<Face> nxyFaces;
    std::vector<Face> pxyFaces;

	std::vector<Face> nyzFaces;
    std::vector<Face> pyzFaces;

	std::vector<Face> nxzFaces;
    std::vector<Face> pxzFaces;
	void GenFaces(TexData& texData);

	VArray<float> faceVertArray;
	VArray<int> faceNormArray;
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

	blockType GetData(ivec3 coord);

	bool operator ==(const Chunk& other) const
	{
		return coordinate == other.coordinate;
	}
};

int taxiLen(ivec3 vec);