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
            thisX = yzFaces.SizeX() - 1;
            otherX = 0;
        }
        else
        {
            thisX = 0;
            otherX = yzFaces.SizeX() - 1;
        }
        for(int y = 0; y < yzFaces.SizeY(); y++)
            for (int z = 0; z < yzFaces.SizeZ(); z++)
            {
                auto& thisType = yzFaces.At({ thisX,y,z });
                if (thisType != 0)
                {
                    auto& otherType = neighbor->yzFaces.At({ otherX,y,z });
                    if (otherType != 0)
                    {
                        thisType = 0;
                        otherType = 0;
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
            thisY = xzFaces.SizeY() - 1;
            otherY = 0;
        }
        else
        {
            thisY = 0;
            otherY = xzFaces.SizeY() - 1;
        }
        for (int x = 0; x < xzFaces.SizeX(); x++)
            for (int z = 0; z < xzFaces.SizeZ(); z++)
            {
                auto& thisType = xzFaces.At({ x,thisY,z });
                if (thisType != 0)
                {
                    auto& otherType = neighbor->xzFaces.At({ x,otherY,z });
                    if (otherType != 0)
                    {
                        thisType = 0;
                        otherType = 0;
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
            thisZ = xyFaces.SizeZ() - 1;
            otherZ = 0;
        }
        else
        {
            thisZ = 0;
            otherZ = xyFaces.SizeZ() - 1;
        }
        for(int x = 0; x < xyFaces.SizeX(); x++)
            for (int y = 0; y < xyFaces.SizeY(); y++)
            {
                auto& thisType = xyFaces.At({ x,y,thisZ });
                if (thisType != 0)
                {
                    auto& otherType = neighbor->xyFaces.At({ x,y,otherZ });
                    if (otherType != 0)
                    {
                        thisType = 0;
                        otherType = 0;
                    }
                }
            }
    }
}

void Chunk::GenFacesSimple()
{
    faces = {};
    ivec3 size = { CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN };
    for (int z = 0; z < xyFaces.SizeZ(); z++)
        for (int x = 0; x < xyFaces.SizeX(); x++)
            for (int y = 0; y < xyFaces.SizeY(); y++)
                {
                    ivec3 iPos = { x,y,z };
                    auto& type = xyFaces.At(iPos);
                    if (type == 0)
                        continue;
                    ivec3* verts = new ivec3[]{
                        iPos + ivec3{ 0,0,0 },
                        iPos + ivec3{ 1,0,0 },
                        iPos + ivec3{ 1,1,0 },
                        iPos + ivec3{ 0,1,0 }
                    };
                    faces.emplace_back(XY, verts, type);
                }
    for (int x = 0; x < yzFaces.SizeX(); x++)
        for (int y = 0; y < yzFaces.SizeY(); y++)
            for (int z = 0; z < yzFaces.SizeZ(); z++)
            {
                ivec3 iPos = { x,y,z };
                auto& type = yzFaces.At(iPos);
                if (type == 0)
                    continue;
                ivec3* verts = new ivec3[]{
                    iPos + ivec3{ 0,0,0 },
                    iPos + ivec3{ 0,1,0 },
                    iPos + ivec3{ 0,1,1 },
                    iPos + ivec3{ 0,0,1 }
                };
                faces.emplace_back(YZ, verts, type);
            }
    for (int y = 0; y < xzFaces.SizeY(); y++)
        for (int x = 0; x < xzFaces.SizeX(); x++)
            for (int z = 0; z < xzFaces.SizeZ(); z++)
            {
                ivec3 iPos = { x,y,z };
                auto& type = xzFaces.At(iPos);
                if (type == 0)
                    continue;
                ivec3* verts = new ivec3[]{
                    iPos + ivec3{ 0,0,0 },
                    iPos + ivec3{ 1,0,0 },
                    iPos + ivec3{ 1,0,1 },
                    iPos + ivec3{ 0,0,1 }
                };
                faces.emplace_back(XZ, verts, type);
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
