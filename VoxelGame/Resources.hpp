#pragma once
#include "SimView/SimView.hpp"
#include "Definitions.hpp"
#include "Block.hpp"

using namespace SimView;

class BlockResources
{
	//std::map<blockID, std::map<int, int>> blockTexture = {};
	//std::map<blockID, bool> blockTransparent = {};
	std::vector<Block*> blocks = {};

public:
	TextureArray texArray;

	BlockResources() {}
	BlockResources(std::vector<std::string> textures);
	blockID AddBlock(Block* newBlock);
	//void LinkBlockTextures(blockID type, int* texIds);
	//void MarkBlockTransparent(blockID type);
	//int GetBlockTexIndex(blockID type, int face);
	//bool IsBlockTransparent(blockID type);
	void ResolveCoplanarFaces(blockID& a, blockID& b, blockID globalA, blockID globalB);
	Block* GetBlock(blockID globalBlockID);
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