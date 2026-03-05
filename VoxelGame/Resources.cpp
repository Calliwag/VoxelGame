#include "Resources.hpp"

BlockResources::BlockResources(std::vector<std::string> textures)
{
    int texCount = textures.size();
    texArray = TextureArray(16, 16, texCount, 5);
    int texIndex = 0;
    for (auto& texName : textures)
    {
        auto img = Bitmap::FromFile("Assets/" + texName + ".png");
        texArray.LayerFromBitmap(img, texIndex++);
    }
    texArray.GenMipmaps(0);
}

blockID BlockResources::AddBlock(Block* newBlock)
{
    blockID blockID = blocks.size();
    blocks.push_back(newBlock);
    return blockID;
}

void BlockResources::ResolveCoplanarFaces(blockID& a, blockID& b, blockID globalA, blockID globalB)
{
    if (a == 0)
    {
        return;
    }
    if (b == 0)
    {
        return;
    }

    auto aV = !GetBlock(globalA)->IsTransparent();
    auto bV = !GetBlock(globalB)->IsTransparent();
    if (aV == bV)
    {
        a = 0;
        b = 0;
    }
    else if (aV && !bV)
    {
        a = 0;
    }
    else if (!aV && bV)
    {
        b = 0;
    }
}

Block* BlockResources::GetBlock(blockID globalBlockID)
{
    return blocks[globalBlockID];
}

UIResources::UIResources(std::vector<std::string> texNames)
{
    layerTexture = Texture(16, 16, nullptr);

    for (auto& texName : texNames)
    {
        auto img = Bitmap::FromFile("Assets/" + texName + ".png");
        auto tex = Texture::FromBitmap(img);
        tex.GenMipmaps(1, 0);
        textures.push_back(tex);
    }

    vec2 quadPts[] =
    {
        {0,0},
        {1,0},
        {1,1},
        {0,1},
    };
    quad = VArray<float>(4, 2, (float*)quadPts);

    vec2 uvPts[] =
    {
        {0,1},
        {1,1},
        {1,0},
        {0,0},
    };
    uvs = VArray<float>(4, 2, (float*)uvPts);
}

void UIResources::SetQuadNNAligned(vec2 corner, vec2 size)
{
    vec2 quadPts[] =
    {
        corner,
        corner + vec2{size.x,0},
        corner + size,
        corner + vec2{0,size.y},
    };
    quad = VArray<float>(4, 2, (float*)quadPts);
}

void UIResources::SetQuadNPAligned(vec2 corner, vec2 size)
{
    corner.y -= size.y;
    SetQuadNNAligned(corner, size);
}

void UIResources::SetQuadPNAligned(vec2 corner, vec2 size)
{
    corner.x -= size.x;
    SetQuadNNAligned(corner, size);
}

void UIResources::SetQuadPPAligned(vec2 corner, vec2 size)
{
    corner -= size;
    SetQuadNNAligned(corner, size);
}

void UIResources::SetQuadCentered(vec2 center, vec2 size)
{
    vec2 corner = center - size / 2.f;
    SetQuadNNAligned(corner, size);
}

void UIResources::BindQuad(ShaderProgram& uiShader, GLint posLoc, GLint uvLoc)
{
    uiShader.BindArray(quad, posLoc);
    uiShader.BindArray(uvs, uvLoc);
}

void UIResources::BindTex(ShaderProgram& uiShader, int texIndex)
{
    uiShader.BindTexture(textures[texIndex]);
}

void UIResources::SetLayerTexture(TextureArray& texArray, int layer)
{
    layerTexture = Texture::FromTextureArray(texArray, layer);
}

void UIResources::BindLayerTexture(ShaderProgram& uiShader)
{
    uiShader.BindTexture(layerTexture);
}
