#include "TexData.hpp"

TexData::TexData(std::vector<std::string> textures)
{
    int texCount = textures.size();
    texArray = TextureArray(16, 16, texCount, 5);
    int texIndex = 0;
    for (auto& texName : textures)
    {
        auto img = Bitmap::FromFile("Assets/" + texName + ".bmp");
        texArray.LayerFromBitmap(img, texIndex++);
    }
    texArray.GenMipmaps(0);
}

void TexData::LinkBlockTextures(blockType type, int* texIds)
{
    for (int i = 0; i < 6; i++)
    {
        blockMap[type][i] = texIds[i];
    }
}

int TexData::GetBlockTexIndex(blockType type, int face)
{
    return blockMap[type][face];
}