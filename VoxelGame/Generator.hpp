#pragma once

#include <glm/glm.hpp>

using namespace glm;

class Generator
{
public:
	virtual u8 GenBlock(ivec3 chunkOffset, ivec3 blockOffset) = 0;
	virtual bool IsChunkEmpty(ivec3 chunkOffset) = 0;
};

class FlatGen : public Generator
{
public:
	int level;
	u8 block;

	FlatGen(int level, u8 block) : level(level), block(block) {}

	u8 GenBlock(ivec3 chunkOffset, ivec3 blockOffset) override;
	bool IsChunkEmpty(ivec3 chunkOffset) override;
};

class WaveGen : public Generator
{
public:
	int baseLevel;
	int amplitude;
	float waveLengthX;
	float waveLengthY;
	u8 block;

	WaveGen(u8 block, int baseLevel, int amplitude, float waveLengthX, float waveLengthY) : block(block), baseLevel(baseLevel), amplitude(amplitude), waveLengthX(waveLengthX), waveLengthY(waveLengthY) {}

	u8 GenBlock(ivec3 chunkOffset, ivec3 blockOffset) override;
	bool IsChunkEmpty(ivec3 chunkOffset) override;
};