#include "Chunk.hpp"

void Chunk::GenFaceGrids(BlocksData& texData)
{
    arraysGenerated = false;

    nxyFaceGrid.Fill(0);
    pxyFaceGrid.Fill(0);
    nyzFaceGrid.Fill(0);
    pyzFaceGrid.Fill(0);
    nxzFaceGrid.Fill(0);
    pxzFaceGrid.Fill(0);
    for(int z = 0; z < CHUNK_SPAN; z++)
        for(int y = 0; y < CHUNK_SPAN; y++)
            for (int x = 0; x < CHUNK_SPAN; x++)
            {
                auto& type = data.At({ x,y,z });
                if (type == 0)
                    continue;

                { // xy Faces
                    auto& mType = nxyFaceGrid.At({ x, y, z }); // negative face
                    mType = type;
                    if (z != 0)
                    {
                        auto& pType = pxyFaceGrid.At({ x,y,z - 1 });
                        texData.ResolveFaces(mType, pType);
                    }

                    pxyFaceGrid.At({ x, y, z }) = type; // positive face
                }

                { // yz Faces
                    auto& mType = nyzFaceGrid.At({ x, y, z }); // negative face
                    mType = type;
                    if (x != 0)
                    {
                        auto& pType = pyzFaceGrid.At({ x - 1,y,z });
                        texData.ResolveFaces(mType, pType);
                    }

                    pyzFaceGrid.At({ x, y, z }) = type; // positive face
                }

                { // xz Faces
                    auto& mType = nxzFaceGrid.At({ x, y, z }); // negative face
                    mType = type;
                    if (y != 0)
                    {
                        auto& pType = pxzFaceGrid.At({ x,y - 1,z });
                        texData.ResolveFaces(mType, pType);
                    }

                    pxzFaceGrid.At({ x, y, z }) = type; // positive face
                }
            }
}

void Chunk::GenFaceGridsSide(ivec3 neighborCoord)
{
    ivec3 side = neighborCoord - coordinate;

    ivec3 layerVec = { 0,0,0 };
    ivec3 iVec = { 0,0,0 };
    ivec3 jVec = { 0,0,0 };
    Grid<blockType, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN>* faces = nullptr;

    if (side.x != 0)
    {
        iVec.y = 1;
        jVec.z = 1;
        if (side.x > 0)
        {
            layerVec.x = CHUNK_SPAN - 1;
            faces = &pyzFaceGrid;
        }
        else
        {
            faces = &nyzFaceGrid;
        }
    }
    else if (side.y != 0)
    {
        iVec.x = 1;
        jVec.z = 1;
        if (side.y > 0)
        {
            layerVec.y = CHUNK_SPAN - 1;
            faces = &pxzFaceGrid;
        }
        else
        {
            faces = &nxzFaceGrid;
        }
    }
    else if (side.z != 0)
    {
        iVec.x = 1;
        jVec.y = 1;
        if (side.z > 0)
        {
            layerVec.z = CHUNK_SPAN - 1;
            faces = &pxyFaceGrid;
        }
        else
        {
            faces = &nxyFaceGrid;
        }
    }

    for (int i = 0; i < CHUNK_SPAN; i++)
    {
        for (int j = 0; j < CHUNK_SPAN; j++)
        {
            auto vec = layerVec + i * iVec + j * jVec;
            faces->At(vec) = GetData(vec);
        }
    }
}

void Chunk::CheckNeighborFaces(Chunk* neighbor, BlocksData& texData)
{
    if (neighbor == nullptr)
        return;
    arraysGenerated = false;
    GenFaceGridsSide(neighbor->coordinate);
    ivec3 offset = neighbor->coordinate - coordinate;
    if (taxiLen(offset) > 1)
        return;
    if (offset.x != 0) // YZ faces
    {
        int thisX;
        int otherX;
        Grid<blockType, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN>* thisFaces = nullptr;
        if(offset.x > 0) // Positive YZ faces
        {
            thisX = CHUNK_SPAN - 1;
            otherX = 0;
            thisFaces = &pyzFaceGrid;
        }
        else // Negative YZ faces
        {
            thisX = 0;
            otherX = CHUNK_SPAN - 1;
            thisFaces = &nyzFaceGrid;
        }
        for(int y = 0; y < CHUNK_SPAN; y++)
            for (int z = 0; z < CHUNK_SPAN; z++)
            {
                auto& thisType = thisFaces->At({ thisX,y,z });
                if (thisType != 0)
                {
                    auto otherType = neighbor->data.At({ otherX,y,z });
                    //if (otherType != 0)
                    //{
                    //    thisType = 0;
                    //}
                    texData.ResolveFaces(thisType, otherType);
                }
            }
        return;
    }
    if (offset.y != 0) // XZ faces
    {
        int thisY;
        int otherY;
        Grid<blockType, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN>* thisFaces = nullptr;
        if (offset.y > 0) // Positive XZ faces
        {
            thisY = CHUNK_SPAN - 1;
            otherY = 0;
            thisFaces = &pxzFaceGrid;
        }
        else // Negative XZ faces
        {
            thisY = 0;
            otherY = CHUNK_SPAN - 1;
            thisFaces = &nxzFaceGrid;
        }
        for (int x = 0; x < CHUNK_SPAN; x++)
            for (int z = 0; z < CHUNK_SPAN; z++)
            {
                auto& thisType = thisFaces->At({ x,thisY,z });
                if (thisType != 0)
                {
                    auto otherType = neighbor->data.At({ x,otherY,z });
                    //if (otherType != 0)
                    //{
                    //    thisType = 0;
                    //}
                    texData.ResolveFaces(thisType, otherType);
                }
            }
        return;
    }
    if (offset.z != 0) // XY faces
    {
        int thisZ;
        int otherZ;
        Grid<blockType, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN>* thisFaces = nullptr;
        if (offset.z > 0) // Positive XY faces
        {
            thisZ = CHUNK_SPAN - 1;
            otherZ = 0;
            thisFaces = &pxyFaceGrid;
        }
        else // Negative XY faces
        {
            thisZ = 0;
            otherZ = CHUNK_SPAN - 1;
            thisFaces = &nxyFaceGrid;
        }
        for (int x = 0; x < CHUNK_SPAN; x++)
            for (int y = 0; y < CHUNK_SPAN; y++)
            {
                auto& thisType = thisFaces->At({ x,y,thisZ });
                if (thisType != 0)
                {
                    auto otherType = neighbor->data.At({ x,y,otherZ });
                    //if (otherType != 0)
                    //{
                    //    thisType = 0;
                    //}
                    texData.ResolveFaces(thisType, otherType);
                }
            }
        return;
    }
}

void Chunk::GenFaces(BlocksData& texData)
{
    faces = {};
    ivec3 size = { CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN };
    for(int x = 0; x < size.x; x++)
        for(int y = 0; y < size.y; y++)
            for (int z = 0; z < size.z; z++)
            {
                ivec3 iPos = { x,y,z };

                // Negative XY faces
                auto& nxyType = nxyFaceGrid.At(iPos);
                if(nxyType != 0)
                {
                    vec3* verts = new vec3[]
                    {
                        iPos + ivec3{ 0,0,0 },
                        iPos + ivec3{ 1,0,0 },
                        iPos + ivec3{ 1,1,0 },
                        iPos + ivec3{ 0,1,0 }
                    };
                    vec2* uvs = new vec2[]
                    {
                        {0,1},
                        {1,1},
                        {1,0},
                        {0,0},
                    };
                    faces.emplace_back(verts, uvs, vec3(0,0,-1), texData.GetBlockTexIndex(nxyType, 0));
                }

                // Positive XY faces
                auto& pxyType = pxyFaceGrid.At(iPos);
                if (pxyType != 0)
                {
                    vec3* verts = new vec3[]
                    {
                        iPos + ivec3{ 0,0,1 },
                        iPos + ivec3{ 0,1,1 },
                        iPos + ivec3{ 1,1,1 },
                        iPos + ivec3{ 1,0,1 }
                    };
                    vec2* uvs = new vec2[]
                    {
                        {0,1},
                        {0,0},
                        {1,0},
                        {1,1},
                    };
                    faces.emplace_back(verts, uvs, vec3(0, 0, 1), texData.GetBlockTexIndex(pxyType, 1));
                }

                // Negative YZ faces
                auto& nyzType = nyzFaceGrid.At(iPos);
                if (nyzType != 0)
                {
                    vec3* verts = new vec3[]
                    {
                        iPos + ivec3{ 0,0,0 },
                        iPos + ivec3{ 0,1,0 },
                        iPos + ivec3{ 0,1,1 },
                        iPos + ivec3{ 0,0,1 }
                    };
                    vec2* uvs = new vec2[]
                    {
                        {1,1},
                        {0,1},
                        {0,0},
                        {1,0},
                    };
                    faces.emplace_back(verts, uvs, vec3(-1, 0, 0), texData.GetBlockTexIndex(nyzType, 2));
                }

                // Positive YZ faces
                auto& pyzType = pyzFaceGrid.At(iPos);
                if (pyzType != 0)
                {
                    vec3* verts = new vec3[]
                    {
                        iPos + ivec3{ 1,0,0 },
                        iPos + ivec3{ 1,0,1 },
                        iPos + ivec3{ 1,1,1 },
                        iPos + ivec3{ 1,1,0 }
                    };
                    vec2* uvs = new vec2[]
                    {
                        {0,1},
                        {0,0},
                        {1,0},
                        {1,1},
                    };
                    faces.emplace_back(verts, uvs, vec3(1, 0, 0), texData.GetBlockTexIndex(pyzType, 3));
                }

                // Negative XZ faces
                auto& nxzType = nxzFaceGrid.At(iPos);
                if (nxzType != 0)
                {
                    vec3* verts = new vec3[]
                    {
                        iPos + ivec3{ 0,0,0 },
                        iPos + ivec3{ 0,0,1 },
                        iPos + ivec3{ 1,0,1 },
                        iPos + ivec3{ 1,0,0 }
                    };
                    vec2* uvs = new vec2[]
                    {
                        {0,1},
                        {0,0},
                        {1,0},
                        {1,1},
                    };
                    faces.emplace_back(verts, uvs, vec3(0, -1, 0), texData.GetBlockTexIndex(nxzType, 4));
                }

                // Positive XZ faces
                auto& pxzType = pxzFaceGrid.At(iPos);
                if (pxzType != 0)
                {
                    vec3* verts = new vec3[]
                    {
                        iPos + ivec3{ 0,1,0 },
                        iPos + ivec3{ 1,1,0 },
                        iPos + ivec3{ 1,1,1 },
                        iPos + ivec3{ 0,1,1 }
                    };
                    vec2* uvs = new vec2[]
                    {
                        {1,1},
                        {0,1},
                        {0,0},
                        {1,0},
                    };
                    faces.emplace_back(verts, uvs, vec3(0, 1, 0), texData.GetBlockTexIndex(pxzType, 5));
                }
            }
}

void Chunk::GenArrays()
{
    arraysGenerated = true;

    faceCount = faces.size();
    if (faceCount == 0)
        return;
    vec3* verts = new vec3[6 * faceCount];
    vec3* norms = new vec3[6 * faceCount];
    vec2* uvs = new vec2[6 * faceCount];
    int* texIndices = new int[6 * faceCount];
    int vIndex = 0;

    for (auto& face : faces)
    {
        auto& c0 = face.verts[0];
        auto& c1 = face.verts[1];
        auto& c2 = face.verts[2];
        auto& c3 = face.verts[3];

        auto& uv0 = face.uvs[0];
        auto& uv1 = face.uvs[1];
        auto& uv2 = face.uvs[2];
        auto& uv3 = face.uvs[3];

        verts[vIndex + 0] = c0;
        uvs[vIndex + 0] = uv0;

        verts[vIndex + 1] = c1;
        uvs[vIndex + 1] = uv1;

        verts[vIndex + 2] = c2;
        uvs[vIndex + 2] = uv2;

        verts[vIndex + 3] = c2;
        uvs[vIndex + 3] = uv2;

        verts[vIndex + 4] = c3;
        uvs[vIndex + 4] = uv3;

        verts[vIndex + 5] = c0;
        uvs[vIndex + 5] = uv0;

        for (int i = 0; i < 6; i++)
        {
            norms[vIndex + i] = face.norm;
            texIndices[vIndex + i] = face.texIdx;
        }
        vIndex += 6;
    }

    faceVertArray.Destroy();
    faceVertArray = VArray<float>(6 * faceCount, 3, (float*)verts);
    faceNormArray = VArray<float>(6 * faceCount, 3, (float*)norms);
    faceUVArray = VArray<float>(6 * faceCount, 2, (float*)uvs);
    faceTexArray = VArray<int>(6 * faceCount, 1, texIndices);
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
    data.At({ 0,0,relLevel + 0 }) = 1;
    //data.At({ 0,0,relLevel + 1 }) = 1;
    //data.At({ 0,0,relLevel + 2 }) = 1;
    data.At({ 0,0,relLevel + 3 }) = 1;
}

bool Chunk::Gen(Generator* gen)
{
    if (gen->IsChunkEmpty(coordinate))
        return false;
    for(int x = 0; x < CHUNK_SPAN; x++)
        for(int y = 0; y < CHUNK_SPAN; y++)
            for (int z = 0; z < CHUNK_SPAN; z++)
            {
                data.At({ x,y,z }) = gen->GenBlock(coordinate, { x,y,z });
            }
    return true;
}

blockType Chunk::GetData(ivec3 coord)
{
    return data.At(coord);
}

int taxiLen(ivec3 vec)
{
    return abs(vec.x) + abs(vec.y) + abs(vec.z);
}