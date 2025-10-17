#pragma once
#include "SimView/SimView.hpp"
#include "Definitions.hpp"

using namespace SimView;

class TexData
{
	std::map<blockType, std::map<int, int>> blockTexture = {};
	std::map<blockType, bool> blockTransparent = {};

public:
	TextureArray texArray;

	TexData() {}
	TexData(std::vector<std::string> textures);
	void LinkBlockTextures(blockType type, int* texIds);
	void MarkBlockTransparent(blockType type);
	int GetBlockTexIndex(blockType type, int face);
	bool IsBlockTransparent(blockType type);
	void ResolveFaces(blockType& a, blockType& b);
};