#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include "Grid.hpp"
#include <vector>
#include "Face.hpp"
#include "SimView/SimView.hpp"
#include "Generator.hpp"

using namespace glm;
using namespace SimView;

constexpr int CHUNK_SPAN = 16;
constexpr int CHUNK_AREA = CHUNK_SPAN * CHUNK_SPAN;
constexpr int CHUNK_VOLUME = CHUNK_AREA * CHUNK_SPAN;

class Chunk
{
public:
	// General
	Grid<u8, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> data;
	ivec3 coordinate;

	// Face grids
	Grid<u8, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> nxyFaceGrid;
	Grid<u8, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> pxyFaceGrid;

	Grid<u8, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> nyzFaceGrid;
	Grid<u8, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> pyzFaceGrid;

	Grid<u8, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> nxzFaceGrid;
	Grid<u8, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> pxzFaceGrid;
	void GenFaceGrids();
    void CheckNeighborFaces(Chunk* neighbor);

	// Face lists
	std::vector<Face> nxyFaces;
    std::vector<Face> pxyFaces;

	std::vector<Face> nyzFaces;
    std::vector<Face> pyzFaces;

	std::vector<Face> nxzFaces;
    std::vector<Face> pxzFaces;
	void GenFaces();
	//void GenFacesGreedy();

	VArray<float> faceVertArray;
	VArray<int> faceNormArray;
	VArray<float> faceUVArray;
	VArray<int> faceTypeArray;
	int faceCount;
	bool arraysGenerated = false;
	void GenArrays();

public:
	Chunk(ivec3 coordinate);
	~Chunk();
	void TestGenChunk();
	void GenChunkLevel(int level);
	bool Gen(Generator* gen);

	u8 GetData(ivec3 coord);

	bool operator ==(const Chunk& other) const
	{
		return coordinate == other.coordinate;
	}
};

int taxiLen(ivec3 vec);