#pragma once
#include "SimView/SimView.hpp"
#include "Definitions.hpp"

using namespace SimView;

class TexData
{
	std::map<blockType, std::map<int, int>> blockMap = {};

public:
	TextureArray texArray;

	TexData() {}
	TexData(std::vector<std::string> textures);
	void LinkBlockTextures(blockType type, int* texIds);
	int GetBlockTexIndex(blockType type, int face);
};