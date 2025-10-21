#include "TexData.hpp"

BlocksData::BlocksData(std::vector<std::string> textures)
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

void BlocksData::LinkBlockTextures(blockType type, int* texIds)
{
    for (int i = 0; i < 6; i++)
    {
        blockTexture[type][i] = texIds[i];
    }
}

void BlocksData::MarkBlockTransparent(blockType type)
{
    blockTransparent[type] = true;
}

int BlocksData::GetBlockTexIndex(blockType type, int face)
{
    return blockTexture[type][face];
}

bool BlocksData::IsBlockTransparent(blockType type)
{
    return blockTransparent[type];
}

void BlocksData::ResolveFaces(blockType& a, blockType& b)
{
    if (a == 0)
    {
        return;
    }
    if (b == 0)
    {
        return;
    }
    auto aV = !IsBlockTransparent(a);
    auto bV = !IsBlockTransparent(b);
    if (aV == bV)
    {
        a = 0;
        b = 0;
    }
    else if (aV && !bV)
    {
        b = 0;
    }
    else if (!aV && bV)
    {
        a = 0;
    }
}

UIData::UIData(std::vector<std::string> texNames)
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

void UIData::SetQuadNNAligned(vec2 corner, vec2 size)
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

void UIData::SetQuadNPAligned(vec2 corner, vec2 size)
{
    corner.y -= size.y;
    SetQuadNNAligned(corner, size);
}

void UIData::SetQuadPNAligned(vec2 corner, vec2 size)
{
    corner.x -= size.x;
    SetQuadNNAligned(corner, size);
}

void UIData::SetQuadPPAligned(vec2 corner, vec2 size)
{
    corner -= size;
    SetQuadNNAligned(corner, size);
}

void UIData::SetQuadCentered(vec2 center, vec2 size)
{
    vec2 corner = center - size / 2.f;
    SetQuadNNAligned(corner, size);
}

void UIData::BindQuad(ShaderProgram& uiShader, GLint posLoc, GLint uvLoc)
{
    uiShader.BindArray(quad, posLoc);
    uiShader.BindArray(uvs, uvLoc);
}

void UIData::BindTex(ShaderProgram& uiShader, int texIndex)
{
    uiShader.BindTexture(textures[texIndex]);
}

void UIData::SetLayerTexture(TextureArray& texArray, int layer)
{
    layerTexture = Texture::FromTextureArray(texArray, layer);
}

void UIData::BindLayerTexture(ShaderProgram& uiShader)
{
    uiShader.BindTexture(layerTexture);
}
