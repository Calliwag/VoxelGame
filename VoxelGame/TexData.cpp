#include "TexData.hpp"

TexData::TexData(std::vector<std::string> textures)
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

void TexData::LinkBlockTextures(blockType type, int* texIds)
{
    for (int i = 0; i < 6; i++)
    {
        blockTexture[type][i] = texIds[i];
    }
}

void TexData::MarkBlockTransparent(blockType type)
{
    blockTransparent[type] = true;
}

int TexData::GetBlockTexIndex(blockType type, int face)
{
    return blockTexture[type][face];
}

bool TexData::IsBlockTransparent(blockType type)
{
    return blockTransparent[type];
}

void TexData::ResolveFaces(blockType& a, blockType& b)
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
        {0,0},
        {1,0},
        {1,1},
        {0,1},
    };
    uvs = VArray<float>(4, 2, (float*)uvPts);
}

void UIData::SetQuadNegativeAligned(vec2 corner, vec2 size)
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

void UIData::SetQuadCentered(vec2 center, vec2 size)
{
    vec2 corner = center - size / 2.f;
    SetQuadNegativeAligned(corner, size);
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
