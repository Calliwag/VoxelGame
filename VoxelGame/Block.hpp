#pragma once

//#include "World.hpp"
#include <array>
#include "Definitions.hpp"

class Block
{
public:
	// Creation and destruction
	virtual Block* GetNew() = 0;
	//virtual void Destroy(World& world, ivec3 worldCoord) = 0;
	virtual ~Block() {};

	// Rendering info
	virtual bool IsTransparent() = 0;
	virtual int GetTextureId(int faceIndex) = 0;
};

class DecoBlock : public Block
{
public:
	DecoBlock(std::array<int, 6> faceTextureMap, bool isTransparent)
	{
		_faceTextureMap = faceTextureMap;
		_isTransparent = isTransparent;
	};

	// Creation and destruction
	Block* GetNew() override { return this; };
	//void Destroy(World& world, ivec3 worldCoord) override {};
	~DecoBlock() override {};

	// Rendering info
	bool IsTransparent() override { return _isTransparent; };
	int GetTextureId(int faceIndex) override { return _faceTextureMap[faceIndex]; };

private:
	std::array<int, 6> _faceTextureMap;
	bool _isTransparent;
};