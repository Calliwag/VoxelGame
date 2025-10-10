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
	float* faceShades;

	GLint matrixLoc;
	GLint faceColorLoc;
	GLint offsetLoc;
	GLint posLoc;

	Renderer();

	void Update(vec3 pos, vec3 dir, float fovY, float width, float height, vec3 lightDir);

	bool DrawChunk(Chunk& chunk);
};

