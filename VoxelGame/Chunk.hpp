#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include "Grid.hpp"
#include <vector>

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

enum fPlane
{
	XY,
	YZ,
	XZ,
};

struct Face
{
	fPlane plane;
	ivec3* verts;
	u8 type;

	Face(fPlane plane, ivec3* verts, u8 type) : plane(plane), verts(verts), type(type) {};
    Face(const Face& other)
        : plane(other.plane), type(other.type)
    {
        if (other.verts) {
            verts = new ivec3[4];
            for (int i = 0; i < 4; ++i)
                verts[i] = other.verts[i];
        }
        else {
            verts = nullptr;
        }
    }

    Face& operator=(const Face& other)
    {
        if (this == &other)
            return *this;
        plane = other.plane;
        type = other.type;
        delete[] verts;
        if (other.verts) {
            verts = new ivec3[4];
            for (int i = 0; i < 4; ++i)
                verts[i] = other.verts[i];
        }
        else {
            verts = nullptr;
        }
        return *this;
    }

    ~Face()
    {
        delete[] verts;
    }
};

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
    void CheckNeighborFaces(Chunk* neighbor);

	// Face lists
	std::vector<Face> faces;
	void GenFaces();

public:
	Chunk(ivec3 coordinate);
	~Chunk();
	void TestGenChunk();
	void GenChunkLevel(int level);

	u8 GetData(ivec3 coord);

	bool operator ==(const Chunk& other) const
	{
		return coordinate == other.coordinate;
	}
};

int taxiLen(ivec3 vec);