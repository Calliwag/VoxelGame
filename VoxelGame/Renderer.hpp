#pragma once

#include "Chunk.hpp"
#include "SimView/SimView.hpp"
#include "Frustum.hpp"

using namespace SimView;

class Renderer
{
public:
	ShaderProgram blockShader;
	VArray chunkVertices;
	Frustum frustum;
	int rTris[6];
	IArray tris;

	Renderer();

	void Update(vec3 pos, vec3 dir, float fovY, float width, float height);

	bool DrawChunk(Chunk& chunk);
};

