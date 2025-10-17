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
