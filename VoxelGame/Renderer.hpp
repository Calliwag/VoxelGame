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

	GLint matrixLoc;
	GLint offsetLoc;
	GLint posLoc;
	GLint normLoc;
	GLint lightDirLoc;

	Renderer();

	void Update(vec3 pos, vec3 dir, float fovY, float width, float height, vec3 lightDir);

	bool DrawChunk(Chunk& chunk);
};

