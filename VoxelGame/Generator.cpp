#include "Generator.hpp"
#include "Chunk.hpp"

u8 FlatGen::GenBlock(ivec3 chunkOffset, ivec3 blockOffset)
{
    int chunkZLevel = CHUNK_SPAN * chunkOffset.z;
    int relLevel = level - chunkZLevel;
    if (blockOffset.z < relLevel)
        return block;
    return 0;
}

bool FlatGen::IsChunkEmpty(ivec3 chunkOffset)
{
    if (level < chunkOffset.z * CHUNK_SPAN)
        return true;
    return false;
}

u8 WaveGen::GenBlock(ivec3 chunkOffset, ivec3 blockOffset)
{
    chunkOffset *= CHUNK_SPAN;
    float xComp = sin(2 * 3.1416 * (blockOffset.x + chunkOffset.x) / waveLengthX);
    float yComp = sin(2 * 3.1416 * (blockOffset.y + chunkOffset.y) / waveLengthY);
    int level = amplitude * (xComp + yComp) / 2.f + baseLevel;
    int relLevel = level - chunkOffset.z;
    if (blockOffset.z < relLevel)
        return block;
    return 0;
}

bool WaveGen::IsChunkEmpty(ivec3 chunkOffset)
{
    int waveTop = baseLevel + amplitude;
    if (waveTop < chunkOffset.z * CHUNK_SPAN)
        return true;
    return false;
}
