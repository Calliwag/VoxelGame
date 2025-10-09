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
                    auto& mType = xyFaceGrid.At({ x, y, z });
                    if (mType == 0)
                    {
                        mType = type;
                    }
                    else
                    {
                        mType = 0;
                    }

                    auto& pType = xyFaceGrid.At({ x, y, z + 1 });
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
                    auto& mType = yzFaceGrid.At({ x, y, z });
                    if (mType == 0)
                    {
                        mType = type;
                    }
                    else
                    {
                        mType = 0;
                    }

                    auto& pType = yzFaceGrid.At({ x + 1, y, z });
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
                    auto& mType = xzFaceGrid.At({ x, y, z });
                    if (mType == 0)
                    {
                        mType = type;
                    }
                    else
                    {
                        mType = 0;
                    }

                    auto& pType = xzFaceGrid.At({ x, y + 1, z });
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

void Chunk::CheckNeighborFaces(Chunk* neighbor)
{
    if (neighbor == nullptr)
        return;
    ivec3 offset = neighbor->coordinate - coordinate;
    if (taxiLen(offset) > 1)
        return;
    if (offset.x != 0)
    {
        int thisX;
        int otherX;
        if(offset.x > 0)
        {
            thisX = yzFaceGrid.SizeX() - 1;
            otherX = 0;
        }
        else
        {
            thisX = 0;
            otherX = data.SizeX() - 1;
        }
        for(int y = 0; y < yzFaceGrid.SizeY(); y++)
            for (int z = 0; z < yzFaceGrid.SizeZ(); z++)
            {
                auto& thisType = yzFaceGrid.At({ thisX,y,z });
                if (thisType != 0)
                {
                    auto& otherType = neighbor->data.At({ otherX,y,z });
                    if (otherType != 0)
                    {
                        thisType = 0;
                    }
                }
            }
        return;
    }
    if (offset.y != 0)
    {
        int thisY;
        int otherY;
        if (offset.y > 0)
        {
            thisY = xzFaceGrid.SizeY() - 1;
            otherY = 0;
        }
        else
        {
            thisY = 0;
            otherY = data.SizeY() - 1;
        }
        for (int x = 0; x < xzFaceGrid.SizeX(); x++)
            for (int z = 0; z < xzFaceGrid.SizeZ(); z++)
            {
                auto& thisType = xzFaceGrid.At({ x,thisY,z });
                if (thisType != 0)
                {
                    auto& otherType = neighbor->data.At({ x,otherY,z });
                    if (otherType != 0)
                    {
                        thisType = 0;
                    }
                }
            }
        return;
    }
    if (offset.z != 0)
    {
        int thisZ;
        int otherZ;
        if (offset.z > 0)
        {
            thisZ = xyFaceGrid.SizeZ() - 1;
            otherZ = 0;
        }
        else
        {
            thisZ = 0;
            otherZ = data.SizeZ() - 1;
        }
        for(int x = 0; x < xyFaceGrid.SizeX(); x++)
            for (int y = 0; y < xyFaceGrid.SizeY(); y++)
            {
                auto& thisType = xyFaceGrid.At({ x,y,thisZ });
                if (thisType != 0)
                {
                    auto& otherType = neighbor->data.At({ x,y,otherZ });
                    if (otherType != 0)
                    {
                        thisType = 0;
                    }
                }
            }
    }
}

void Chunk::GenFacesSimple()
{
    xyFaces = {};
    yzFaces = {};
    xzFaces = {};
    ivec3 size = { CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN };
    for (int z = 0; z < xyFaceGrid.SizeZ(); z++)
        for (int x = 0; x < xyFaceGrid.SizeX(); x++)
            for (int y = 0; y < xyFaceGrid.SizeY(); y++)
                {
                    ivec3 iPos = { x,y,z };
                    auto& type = xyFaceGrid.At(iPos);
                    if (type == 0)
                        continue;
                    ivec3* verts = new ivec3[]{
                        iPos + ivec3{ 0,0,0 },
                        iPos + ivec3{ 1,0,0 },
                        iPos + ivec3{ 1,1,0 },
                        iPos + ivec3{ 0,1,0 }
                    };
                    xyFaces.emplace_back(verts, type);
                }
    for (int x = 0; x < yzFaceGrid.SizeX(); x++)
        for (int y = 0; y < yzFaceGrid.SizeY(); y++)
            for (int z = 0; z < yzFaceGrid.SizeZ(); z++)
            {
                ivec3 iPos = { x,y,z };
                auto& type = yzFaceGrid.At(iPos);
                if (type == 0)
                    continue;
                ivec3* verts = new ivec3[]{
                    iPos + ivec3{ 0,0,0 },
                    iPos + ivec3{ 0,1,0 },
                    iPos + ivec3{ 0,1,1 },
                    iPos + ivec3{ 0,0,1 }
                };
                yzFaces.emplace_back(verts, type);
            }
    for (int y = 0; y < xzFaceGrid.SizeY(); y++)
        for (int x = 0; x < xzFaceGrid.SizeX(); x++)
            for (int z = 0; z < xzFaceGrid.SizeZ(); z++)
            {
                ivec3 iPos = { x,y,z };
                auto& type = xzFaceGrid.At(iPos);
                if (type == 0)
                    continue;
                ivec3* verts = new ivec3[]{
                    iPos + ivec3{ 0,0,0 },
                    iPos + ivec3{ 1,0,0 },
                    iPos + ivec3{ 1,0,1 },
                    iPos + ivec3{ 0,0,1 }
                };
                xzFaces.emplace_back(verts, type);
            }
}

Chunk::Chunk(ivec3 coordinate) : coordinate(coordinate)
{

}

Chunk::~Chunk()
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
    if (relLevel >= CHUNK_SPAN)
    {
        data.Fill(1);
        return;
    }
    else if (relLevel <= 0)
    {
        data.Fill(0);
        return;
    }
    for (int z = 0; z < glm::clamp(relLevel,0,CHUNK_SPAN); z++)
        for (int y = 0; y < CHUNK_SPAN; y++)
            for (int x = 0; x < CHUNK_SPAN; x++)
            {
                data.At({ x,y,z }) = 1;
            }
    data.At({ 8,8,relLevel }) = 1;
}

u8 Chunk::GetData(ivec3 coord)
{
    return data.At(coord);
}

int taxiLen(ivec3 vec)
{
    return abs(vec.x) + abs(vec.y) + abs(vec.z);
}
