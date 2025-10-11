#include "Chunk.hpp"

void Chunk::GenFaceGrids()
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
                    if (z > 0)
                    {
                        auto& pType = pxyFaceGrid.At({ x,y,z - 1 });
                        if (pType == 0)
                        {
                            mType = type;
                        }
                        else
                        {
                            pType = 0;
                            mType = 0;
                        }
                    }
                    else
                    {
                        mType = type;
                    }

                    pxyFaceGrid.At({ x, y, z }) = type; // positive face
                }

                { // yz Faces
                    auto& mType = nyzFaceGrid.At({ x, y, z }); // negative face
                    if (x > 0)
                    {
                        auto& pType = pyzFaceGrid.At({ x - 1,y,z });
                        if (pType == 0)
                        {
                            mType = type;
                        }
                        else
                        {
                            pType = 0;
                            mType = 0;
                        }
                    }
                    else
                    {
                        mType = type;
                    }

                    pyzFaceGrid.At({ x, y, z }) = type; // positive face
                }

                { // xz Faces
                    auto& mType = nxzFaceGrid.At({ x, y, z }); // negative face
                    if (y > 0)
                    {
                        auto& pType = pxzFaceGrid.At({ x,y - 1,z });
                        if (pType == 0)
                        {
                            mType = type;
                        }
                        else
                        {
                            pType = 0;
                            mType = 0;
                        }
                    }
                    else
                    {
                        mType = type;
                    }

                    pxzFaceGrid.At({ x, y, z }) = type; // positive face
                }
            }
}

void Chunk::CheckNeighborFaces(Chunk* neighbor)
{
    if (neighbor == nullptr)
        return;
    arraysGenerated = false;
    ivec3 offset = neighbor->coordinate - coordinate;
    if (taxiLen(offset) > 1)
        return;
    if (offset.x != 0) // YZ faces
    {
        int thisX;
        int otherX;
        Grid<u8, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN>* thisFaces = nullptr;
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
                    auto& otherType = neighbor->data.At({ otherX,y,z });
                    if (otherType != 0)
                    {
                        thisType = 0;
                    }
                }
            }
        return;
    }
    if (offset.y != 0) // XZ faces
    {
        int thisY;
        int otherY;
        Grid<u8, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN>* thisFaces = nullptr;
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
                    auto& otherType = neighbor->data.At({ x,otherY,z });
                    if (otherType != 0)
                    {
                        thisType = 0;
                    }
                }
            }
        return;
    }
    if (offset.z != 0) // XY faces
    {
        int thisZ;
        int otherZ;
        Grid<u8, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN>* thisFaces = nullptr;
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
                    auto& otherType = neighbor->data.At({ x,y,otherZ });
                    if (otherType != 0)
                    {
                        thisType = 0;
                    }
                }
            }
        return;
    }
}

void Chunk::GenFacesSimple()
{
    nxyFaces = {};
    pxyFaces = {};
    nyzFaces = {};
    pyzFaces = {};
    nxzFaces = {};
    pxzFaces = {};
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
                    ivec3* verts = new ivec3[]
                    {
                        iPos + ivec3{ 0,0,0 },
                        iPos + ivec3{ 1,0,0 },
                        iPos + ivec3{ 1,1,0 },
                        iPos + ivec3{ 0,1,0 }
                    };
                    nxyFaces.emplace_back(verts, nxyType);
                }

                // Positive XY faces
                auto& pxyType = pxyFaceGrid.At(iPos);
                if (pxyType != 0)
                {
                    ivec3* verts = new ivec3[]
                    {
                        iPos + ivec3{ 0,0,1 },
                        iPos + ivec3{ 1,0,1 },
                        iPos + ivec3{ 1,1,1 },
                        iPos + ivec3{ 0,1,1 }
                    };
                    pxyFaces.emplace_back(verts, pxyType);
                }

                // Negative YZ faces
                auto& nyzType = nyzFaceGrid.At(iPos);
                if (nyzType != 0)
                {
                    ivec3* verts = new ivec3[]
                    {
                        iPos + ivec3{ 0,0,0 },
                        iPos + ivec3{ 0,1,0 },
                        iPos + ivec3{ 0,1,1 },
                        iPos + ivec3{ 0,0,1 }
                    };
                    nyzFaces.emplace_back(verts, nyzType);
                }

                // Positive YZ faces
                auto& pyzType = pyzFaceGrid.At(iPos);
                if (pyzType != 0)
                {
                    ivec3* verts = new ivec3[]
                    {
                        iPos + ivec3{ 1,0,0 },
                        iPos + ivec3{ 1,1,0 },
                        iPos + ivec3{ 1,1,1 },
                        iPos + ivec3{ 1,0,1 }
                    };
                    pyzFaces.emplace_back(verts, pyzType);
                }

                // Negative XZ faces
                auto& nxzType = nxzFaceGrid.At(iPos);
                if (nxzType != 0)
                {
                    ivec3* verts = new ivec3[]
                    {
                        iPos + ivec3{ 0,0,0 },
                        iPos + ivec3{ 1,0,0 },
                        iPos + ivec3{ 1,0,1 },
                        iPos + ivec3{ 0,0,1 }
                    };
                    nxzFaces.emplace_back(verts, nxzType);
                }

                // Positive XZ faces
                auto& pxzType = pxzFaceGrid.At(iPos);
                if (pxzType != 0)
                {
                    ivec3* verts = new ivec3[]
                    {
                        iPos + ivec3{ 0,1,0 },
                        iPos + ivec3{ 1,1,0 },
                        iPos + ivec3{ 1,1,1 },
                        iPos + ivec3{ 0,1,1 }
                    };
                    pxzFaces.emplace_back(verts, nxzType);
                }
            }
}

void Chunk::GenFacesGreedy()
{
    nxyFaces = {};
    pxyFaces = {};
    nyzFaces = {};
    pyzFaces = {};
    nxzFaces = {};
    pxzFaces = {};
    ivec3 size = { CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN };

    Grid<bool, CHUNK_SPAN, CHUNK_SPAN, CHUNK_SPAN> meshedBlocks;

    // Greedy mesh negative XY faces
    meshedBlocks.Fill(false);
    for (int z = 0; z < size.z; z++)
    {
        ivec3 corner = { 0,0,z };
        bool endLayer = false;
        while (!endLayer)
        {
            // Find the next corner where data is not 0
            u8 currentType;
            while (true)
            {
                if (corner.x >= size.x)
                {
                    corner.x = 0;
                    corner.y += 1;
                }
                if (corner.y >= size.y)
                {
                    endLayer = true;
                    break;
                }
                auto& faceType = nxyFaceGrid.At(corner);
                if (faceType != 0 && !meshedBlocks.At(corner))
                {
                    currentType = faceType;
                    break;
                }
                corner.x++;
            }
            if (endLayer)
                break;

            // Extend the rectangle along the positive X direction
            int rectX = 0;
            while (corner.x + rectX < size.x)
            {
                auto& faceType = nxyFaceGrid.At(corner + ivec3{ rectX, 0, 0 });
                if (faceType != currentType || meshedBlocks.At(corner + ivec3{ rectX, 0,0 }))
                {
                    break;
                }
                meshedBlocks.At(corner + ivec3{ rectX,0,0 }) = true;
                rectX++;
            }

            // Extend the rectangle along the positive Y direction
            int rectY = 1;
            bool good = true;
            while (corner.y + rectY < size.y && good)
            {
                for (int x = corner.x; x < corner.x + rectX; x++)
                {
                    auto& faceType = nxyFaceGrid.At({ x,corner.y + rectY,z });
                    if (faceType != currentType || meshedBlocks.At({ x,corner.y + rectY, z }))
                    {
                        good = false;
                        break;
                    }
                }
                if (!good)
                    break;
                for (int x = corner.x; x < corner.x + rectX; x++)
                {
                    meshedBlocks.At({ x,corner.y + rectY, z }) = true;
                }
                rectY++;
            }

            rectX -= 1;
            rectY -= 1;

            ivec3* verts = new ivec3[]
            {
                corner + ivec3{ 0,0,0 },
                corner + ivec3{ rectX + 1,0,0 },
                corner + ivec3{ rectX + 1,rectY + 1,0 },
                corner + ivec3{ 0,rectY + 1,0 }
            };
            nxyFaces.emplace_back(verts, currentType);
            corner.x += rectX + 1;
        }
    }

    // Greedy mesh positive XY faces
    meshedBlocks.Fill(false);
    for (int z = 0; z < size.z; z++)
    {
        ivec3 corner = { 0,0,z };
        bool endLayer = false;
        while (!endLayer)
        {
            // Find the next corner where data is not 0
            u8 currentType;
            while (true)
            {
                if (corner.x >= size.x)
                {
                    corner.x = 0;
                    corner.y += 1;
                }
                if (corner.y >= size.y)
                {
                    endLayer = true;
                    break;
                }
                auto& faceType = pxyFaceGrid.At(corner);
                if (faceType != 0 && !meshedBlocks.At(corner))
                {
                    currentType = faceType;
                    break;
                }
                corner.x++;
            }
            if (endLayer)
                break;

            // Extend the rectangle along the positive X direction
            int rectX = 0;
            while (corner.x + rectX < size.x)
            {
                auto& faceType = pxyFaceGrid.At(corner + ivec3{ rectX, 0, 0 });
                if (faceType != currentType || meshedBlocks.At(corner + ivec3{ rectX, 0,0 }))
                {
                    break;
                }
                meshedBlocks.At(corner + ivec3{ rectX,0,0 }) = true;
                rectX++;
            }

            // Extend the rectangle along the positive Y direction
            int rectY = 1;
            bool good = true;
            while (corner.y + rectY < size.y && good)
            {
                for (int x = corner.x; x < corner.x + rectX; x++)
                {
                    auto& faceType = pxyFaceGrid.At({ x,corner.y + rectY,z });
                    if (faceType != currentType || meshedBlocks.At({ x,corner.y + rectY, z }))
                    {
                        good = false;
                        break;
                    }
                }
                if (!good)
                    break;
                for (int x = corner.x; x < corner.x + rectX; x++)
                {
                    meshedBlocks.At({ x,corner.y + rectY, z }) = true;
                }
                rectY++;
            }

            rectX -= 1;
            rectY -= 1;

            ivec3* verts = new ivec3[]
            {
                corner + ivec3{ 0,0,1 },
                corner + ivec3{ 0,rectY + 1,1 },
                corner + ivec3{ rectX + 1,rectY + 1,1 },
                corner + ivec3{ rectX + 1,0,1 },
            };
            pxyFaces.emplace_back(verts, currentType);
            corner.x += rectX + 1;
        }
    }

    // Greedy mesh negative YZ faces
    meshedBlocks.Fill(false);
    for (int x = 0; x < size.x; x++)
    {
        ivec3 corner = { x,0,0 };
        bool endLayer = false;
        while (!endLayer)
        {
            // Find the next corner where data is not 0
            u8 currentType;
            while (true)
            {
                if (corner.y >= size.y)
                {
                    corner.y = 0;
                    corner.z += 1;
                }
                if (corner.z >= size.z)
                {
                    endLayer = true;
                    break;
                }
                auto& faceType = nyzFaceGrid.At(corner);
                if (faceType != 0 && !meshedBlocks.At(corner))
                {
                    currentType = faceType;
                    break;
                }
                corner.y++;
            }
            if (endLayer)
                break;

            // Extend the rectangle along the positive Y direction
            int rectY = 0;
            while (corner.y + rectY < size.y)
            {
                auto& faceType = nyzFaceGrid.At(corner + ivec3{ 0, rectY, 0 });
                if (faceType != currentType || meshedBlocks.At(corner + ivec3{ 0, rectY,0 }))
                {
                    break;
                }
                meshedBlocks.At(corner + ivec3{ 0,rectY,0 }) = true;
                rectY++;
            }

            // Extend the rectangle along the positive Z direction
            int rectZ = 1;
            bool good = true;
            while (corner.z + rectZ < size.z && good)
            {
                for (int y = corner.y; y < corner.y + rectY; y++)
                {
                    auto& faceType = nyzFaceGrid.At({ x,y,corner.z + rectZ });
                    if (faceType != currentType || meshedBlocks.At({ x,y,corner.z + rectZ }))
                    {
                        good = false;
                        break;
                    }
                }
                if (!good)
                    break;
                for (int y = corner.y; y < corner.y + rectY; y++)
                {
                    meshedBlocks.At({ x,y,corner.z + rectZ }) = true;
                }
                rectZ++;
            }

            rectY -= 1;
            rectZ -= 1;

            ivec3* verts = new ivec3[]
            {
                corner + ivec3{ 0,0,0 },
                corner + ivec3{ 0,rectY + 1,0 },
                corner + ivec3{ 0,rectY + 1,rectZ + 1 },
                corner + ivec3{ 0,0,rectZ + 1 }
            };
            nyzFaces.emplace_back(verts, currentType);
            corner.y += rectY + 1;
        }
    }

    // Greedy mesh positive YZ faces
    meshedBlocks.Fill(false);
    for (int x = 0; x < size.x; x++)
    {
        ivec3 corner = { x,0,0 };
        bool endLayer = false;
        while (!endLayer)
        {
            // Find the next corner where data is not 0
            u8 currentType;
            while (true)
            {
                if (corner.y >= size.y)
                {
                    corner.y = 0;
                    corner.z += 1;
                }
                if (corner.z >= size.z)
                {
                    endLayer = true;
                    break;
                }
                auto& faceType = pyzFaceGrid.At(corner);
                if (faceType != 0 && !meshedBlocks.At(corner))
                {
                    currentType = faceType;
                    break;
                }
                corner.y++;
            }
            if (endLayer)
                break;

            // Extend the rectangle along the positive Y direction
            int rectY = 0;
            while (corner.y + rectY < size.y)
            {
                auto& faceType = pyzFaceGrid.At(corner + ivec3{ 0, rectY, 0 });
                if (faceType != currentType || meshedBlocks.At(corner + ivec3{ 0, rectY,0 }))
                {
                    break;
                }
                meshedBlocks.At(corner + ivec3{ 0,rectY,0 }) = true;
                rectY++;
            }

            // Extend the rectangle along the positive Z direction
            int rectZ = 1;
            bool good = true;
            while (corner.z + rectZ < size.z && good)
            {
                for (int y = corner.y; y < corner.y + rectY; y++)
                {
                    auto& faceType = pyzFaceGrid.At({ x,y,corner.z + rectZ });
                    if (faceType != currentType || meshedBlocks.At({ x,y,corner.z + rectZ }))
                    {
                        good = false;
                        break;
                    }
                }
                if (!good)
                    break;
                for (int y = corner.y; y < corner.y + rectY; y++)
                {
                    meshedBlocks.At({ x,y,corner.z + rectZ }) = true;
                }
                rectZ++;
            }

            rectY -= 1;
            rectZ -= 1;

            ivec3* verts = new ivec3[]
            {
                corner + ivec3{ 1,0,0 },
                corner + ivec3{ 1,0,rectZ + 1 },
                corner + ivec3{ 1,rectY + 1,rectZ + 1 },
                corner + ivec3{ 1,rectY + 1,0 },
            };
            pyzFaces.emplace_back(verts, currentType);
            corner.y += rectY + 1;
        }
    }

    // Greedy mesh negative XZ faces
    meshedBlocks.Fill(false);
    for (int y = 0; y < size.y; y++)
    {
        ivec3 corner = { 0,y,0 };
        bool endLayer = false;
        while (!endLayer)
        {
            // Find the next corner where data is not 0
            u8 currentType;
            while (true)
            {
                if (corner.x >= size.x)
                {
                    corner.x = 0;
                    corner.z += 1;
                }
                if (corner.z >= size.z)
                {
                    endLayer = true;
                    break;
                }
                auto& faceType = nxzFaceGrid.At(corner);
                if (faceType != 0 && !meshedBlocks.At(corner))
                {
                    currentType = faceType;
                    break;
                }
                corner.x++;
            }
            if (endLayer)
                break;

            // Extend the rectangle along the positive X direction
            int rectX = 0;
            while (corner.x + rectX < size.x)
            {
                auto& faceType = nxzFaceGrid.At(corner + ivec3{ rectX, 0, 0 });
                if (faceType != currentType || meshedBlocks.At(corner + ivec3{ rectX, 0,0 }))
                {
                    break;
                }
                meshedBlocks.At(corner + ivec3{ rectX,0,0 }) = true;
                rectX++;
            }

            // Extend the rectangle along the positive Z direction
            int rectZ = 1;
            bool good = true;
            while (corner.z + rectZ < size.z && good)
            {
                for (int x = corner.x; x < corner.x + rectX; x++)
                {
                    auto& faceType = nxzFaceGrid.At({ x,y,corner.z + rectZ });
                    if (faceType != currentType || meshedBlocks.At({ x,y,corner.z + rectZ }))
                    {
                        good = false;
                        break;
                    }
                }
                if (!good)
                    break;
                for (int x = corner.x; x < corner.x + rectX; x++)
                {
                    meshedBlocks.At({ x,y,corner.z + rectZ }) = true;
                }
                rectZ++;
            }

            rectX -= 1;
            rectZ -= 1;

            ivec3* verts = new ivec3[]
            {
                corner + ivec3{ 0,0,0 },
                corner + ivec3{ 0,0,rectZ + 1 },
                corner + ivec3{ rectX + 1,0,rectZ + 1 },
                corner + ivec3{ rectX + 1,0,0 },
            };
            nxzFaces.emplace_back(verts, currentType);
            corner.x += rectX + 1;
        }
    }

    // Greedy mesh positive XZ faces
    meshedBlocks.Fill(false);
    for (int y = 0; y < size.y; y++)
    {
        ivec3 corner = { 0,y,0 };
        bool endLayer = false;
        while (!endLayer)
        {
            // Find the next corner where data is not 0
            u8 currentType;
            while (true)
            {
                if (corner.x >= size.x)
                {
                    corner.x = 0;
                    corner.z += 1;
                }
                if (corner.z >= size.z)
                {
                    endLayer = true;
                    break;
                }
                auto& faceType = pxzFaceGrid.At(corner);
                if (faceType != 0 && !meshedBlocks.At(corner))
                {
                    currentType = faceType;
                    break;
                }
                corner.x++;
            }
            if (endLayer)
                break;

            // Extend the rectangle along the positive X direction
            int rectX = 0;
            while (corner.x + rectX < size.x)
            {
                auto& faceType = pxzFaceGrid.At(corner + ivec3{ rectX, 0, 0 });
                if (faceType != currentType || meshedBlocks.At(corner + ivec3{ rectX, 0,0 }))
                {
                    break;
                }
                meshedBlocks.At(corner + ivec3{ rectX,0,0 }) = true;
                rectX++;
            }

            // Extend the rectangle along the positive Z direction
            int rectZ = 1;
            bool good = true;
            while (corner.z + rectZ < size.z && good)
            {
                for (int x = corner.x; x < corner.x + rectX; x++)
                {
                    auto& faceType = pxzFaceGrid.At({ x,y,corner.z + rectZ });
                    if (faceType != currentType || meshedBlocks.At({ x,y,corner.z + rectZ }))
                    {
                        good = false;
                        break;
                    }
                }
                if (!good)
                    break;
                for (int x = corner.x; x < corner.x + rectX; x++)
                {
                    meshedBlocks.At({ x,y,corner.z + rectZ }) = true;
                }
                rectZ++;
            }

            rectX -= 1;
            rectZ -= 1;

            ivec3* verts = new ivec3[]
            {
                corner + ivec3{ 0,1,0 },
                corner + ivec3{ rectX + 1,1,0 },
                corner + ivec3{ rectX + 1,1,rectZ + 1 },
                corner + ivec3{ 0,1,rectZ + 1 }
            };
            pxzFaces.emplace_back(verts, currentType);
            corner.x += rectX + 1;
        }
    }
}

void Chunk::GenArrays()
{
    arraysGenerated = true;

    if (faceVertArray)
        delete faceVertArray;
    if (faceNormArray)
        delete faceNormArray;

    faceCount = (nxyFaces.size() + pxyFaces.size() + nyzFaces.size() + pyzFaces.size() + nxzFaces.size() + pxzFaces.size());
    if (faceCount == 0)
        return;
    int totalSize = 6 * faceCount;
    vec3* verts = new vec3[totalSize];
    vec3* norms = new vec3[totalSize];
    int vIndex = 0;
    vec3 norm;

    // Negative XY faces
    norm = { 0,0,-1 };
    for (auto& face : nxyFaces)
    {
        auto& c0 = face.verts[0];
        auto& c1 = face.verts[1];
        auto& c2 = face.verts[2];
        auto& c3 = face.verts[3];
        verts[vIndex + 0] = c0;
        verts[vIndex + 1] = c1;
        verts[vIndex + 2] = c2;
        verts[vIndex + 3] = c2;
        verts[vIndex + 4] = c3;
        verts[vIndex + 5] = c0;
        for (int i = 0; i < 6; i++)
        {
            norms[vIndex + i] = norm;
        }
        vIndex += 6;
    }

    // Positive XY faces
    norm = { 0,0,1 };
    for (auto& face : pxyFaces)
    {
        auto& c0 = face.verts[0];
        auto& c1 = face.verts[1];
        auto& c2 = face.verts[2];
        auto& c3 = face.verts[3];
        verts[vIndex + 0] = c0;
        verts[vIndex + 1] = c1;
        verts[vIndex + 2] = c2;
        verts[vIndex + 3] = c2;
        verts[vIndex + 4] = c3;
        verts[vIndex + 5] = c0;
        for (int i = 0; i < 6; i++)
        {
            norms[vIndex + i] = norm;
        }
        vIndex += 6;
    }

    // Negative YZ faces
    norm = { -1,0,0 };
    for (auto& face : nyzFaces)
    {
        auto& c0 = face.verts[0];
        auto& c1 = face.verts[1];
        auto& c2 = face.verts[2];
        auto& c3 = face.verts[3];
        verts[vIndex + 0] = c0;
        verts[vIndex + 1] = c1;
        verts[vIndex + 2] = c2;
        verts[vIndex + 3] = c2;
        verts[vIndex + 4] = c3;
        verts[vIndex + 5] = c0;
        for (int i = 0; i < 6; i++)
        {
            norms[vIndex + i] = norm;
        }
        vIndex += 6;
    }

    // Positive YZ faces
    norm = { -1,0,0 };
    for (auto& face : pyzFaces)
    {
        auto& c0 = face.verts[0];
        auto& c1 = face.verts[1];
        auto& c2 = face.verts[2];
        auto& c3 = face.verts[3];
        verts[vIndex + 0] = c0;
        verts[vIndex + 1] = c1;
        verts[vIndex + 2] = c2;
        verts[vIndex + 3] = c2;
        verts[vIndex + 4] = c3;
        verts[vIndex + 5] = c0;
        for (int i = 0; i < 6; i++)
        {
            norms[vIndex + i] = norm;
        }
        vIndex += 6;
    }

    // Negative XZ faces
    norm = { 0,-1,0 };
    for (auto& face : nxzFaces)
    {
        auto& c0 = face.verts[0];
        auto& c1 = face.verts[1];
        auto& c2 = face.verts[2];
        auto& c3 = face.verts[3];
        verts[vIndex + 0] = c0;
        verts[vIndex + 1] = c1;
        verts[vIndex + 2] = c2;
        verts[vIndex + 3] = c2;
        verts[vIndex + 4] = c3;
        verts[vIndex + 5] = c0;
        for (int i = 0; i < 6; i++)
        {
            norms[vIndex + i] = norm;
        }
        vIndex += 6;
    }

    // Positive XZ faces
    norm = { 0,1,0 };
    for (auto& face : pxzFaces)
    {
        auto& c0 = face.verts[0];
        auto& c1 = face.verts[1];
        auto& c2 = face.verts[2];
        auto& c3 = face.verts[3];
        verts[vIndex + 0] = c0;
        verts[vIndex + 1] = c1;
        verts[vIndex + 2] = c2;
        verts[vIndex + 3] = c2;
        verts[vIndex + 4] = c3;
        verts[vIndex + 5] = c0;
        for (int i = 0; i < 6; i++)
        {
            norms[vIndex + i] = norm;
        }
        vIndex += 6;
    }

    faceVertArray = new VArray(totalSize, 3, verts);
    faceNormArray = new VArray(totalSize, 3, norms);
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

void Chunk::Gen(Generator* gen)
{
    if (gen->IsChunkEmpty(coordinate))
        return;
    for(int x = 0; x < CHUNK_SPAN; x++)
        for(int y = 0; y < CHUNK_SPAN; y++)
            for (int z = 0; z < CHUNK_SPAN; z++)
            {
                data.At({ x,y,z }) = gen->GenBlock(coordinate, { x,y,z });
            }
}

u8 Chunk::GetData(ivec3 coord)
{
    return data.At(coord);
}

int taxiLen(ivec3 vec)
{
    return abs(vec.x) + abs(vec.y) + abs(vec.z);
}
