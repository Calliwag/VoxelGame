#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include "Grid.hpp"

typedef std::uint8_t u8;
typedef std::uint8_t u16;
typedef std::uint8_t u32;
typedef std::int8_t i8;
typedef std::int8_t i16;
typedef std::int8_t i32;

using namespace glm;

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
	Grid<u8, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN + 1> xyFaces;
	Grid<u8, CHUNK_SPAN + 1, CHUNK_SPAN, CHUNK_SPAN> yzFaces;
	Grid<u8, CHUNK_SPAN, CHUNK_SPAN + 1, CHUNK_SPAN> xzFaces;
	void GenFaceGrids();
public:
	Chunk(ivec3 coordinate);
	void TestGenChunk();
	void GenChunkLevel(int level);

	u8 GetData(ivec3 coord);
};

