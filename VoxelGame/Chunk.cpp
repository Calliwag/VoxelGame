#include "Chunk.hpp"

void Chunk::GenFaceGrids()
{
    for(int z = 0; z < CHUNK_SPAN; z++)
        for(int y = 0; y < CHUNK_SPAN; y++)
            for (int x = 0; x < CHUNK_SPAN; x++)
            {
                auto& type = data.At({ x,y,z });
                if (type == 0)
                    continue;

                { // xy Faces
                    auto& mType = xyFaces.At({ x, y, z });
                    if (mType == 0)
                    {
                        mType = type;
                    }
                    else
                    {
                        mType = 0;
                    }

                    auto& pType = xyFaces.At({ x, y, z + 1 });
                    if (pType == 0)
                    {
                        pType = type;
                    }
                    else
                    {
                        pType = 0;
                    }
                }

                { // yz Faces
                    auto& mType = yzFaces.At({ x, y, z });
                    if (mType == 0)
                    {
                        mType = type;
                    }
                    else
                    {
                        mType = 0;
                    }

                    auto& pType = yzFaces.At({ x + 1, y, z });
                    if (pType == 0)
                    {
                        pType = type;
                    }
                    else
                    {
                        pType = 0;
                    }
                }

                { // xz Faces
                    auto& mType = xzFaces.At({ x, y, z });
                    if (mType == 0)
                    {
                        mType = type;
                    }
                    else
                    {
                        mType = 0;
                    }

                    auto& pType = xzFaces.At({ x, y + 1, z });
                    if (pType == 0)
                    {
                        pType = type;
                    }
                    else
                    {
                        pType = 0;
                    }
                }
            }
}

Chunk::Chunk(ivec3 coordinate) : coordinate(coordinate)
{

}

void Chunk::TestGenChunk()
{
    for(int z = 0; z < 8; z++)
        for(int y = 0; y < CHUNK_SPAN; y++)
            for (int x = 0; x < CHUNK_SPAN; x++)
            {
                data.At(ivec3{ x,y,z }) = 1;
            }
}

void Chunk::GenChunkLevel(int level)
{
    int relLevel = level - CHUNK_SPAN * coordinate.z;
    for (int z = 0; z < glm::clamp(relLevel,0,CHUNK_SPAN); z++)
        for (int y = 0; y < CHUNK_SPAN; y++)
            for (int x = 0; x < CHUNK_SPAN; x++)
            {
                data.At(ivec3{ x,y,z }) = 1;
            }
}

u8 Chunk::GetData(ivec3 coord)
{
    return data.At(coord);
}