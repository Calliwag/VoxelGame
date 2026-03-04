#pragma once
#include "SimView/SimView.hpp"
#include "Definitions.hpp"

using namespace SimView;

class BlockResources
{
	std::map<blockType, std::map<int, int>> blockTexture = {};
	std::map<blockType, bool> blockTransparent = {};

public:
	TextureArray texArray;

	BlockResources() {}
	BlockResources(std::vector<std::string> textures);
	void LinkBlockTextures(blockType type, int* texIds);
	void MarkBlockTransparent(blockType type);
	int GetBlockTexIndex(blockType type, int face);
	bool IsBlockTransparent(blockType type);
	void ResolveFaces(blockType& a, blockType& b);
};

class UIResources
{
	std::vector<Texture> textures = {};
	Texture layerTexture;
	VArray<float> quad;
	VArray<float> uvs;

public:
	UIResources() {};
	UIResources(std::vector<std::string> texNames);
	void SetQuadNNAligned(vec2 corner, vec2 size);
	void SetQuadNPAligned(vec2 corner, vec2 size);
	void SetQuadPNAligned(vec2 corner, vec2 size);
	void SetQuadPPAligned(vec2 corner, vec2 size);
	void SetQuadCentered(vec2 center, vec2 size);
	void BindQuad(ShaderProgram& uiShader, GLint posLoc, GLint uvLoc);
	void BindTex(ShaderProgram& uiShader, int texIndex);
	void SetLayerTexture(TextureArray& texArray, int layer);
	void BindLayerTexture(ShaderProgram& uiShader);
};