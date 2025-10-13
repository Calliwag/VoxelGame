#pragma once

#include "Chunk.hpp"
#include "SimView/SimView.hpp"
#include "Frustum.hpp"

using namespace SimView;

class Renderer
{
public:
	ShaderProgram blockShader;
	Frustum frustum;

	GLint matrixLoc;
	GLint offsetLoc;
	GLint posLoc;
	GLint normLoc;
	GLint lightDirLoc;
	GLint blockTypeLoc;
	GLint uvLoc;
	GLint atlasLoc;

	Renderer(const Texture& atlasTex);

	void Update(vec3 pos, vec3 dir, float fovY, float width, float height, vec3 lightDir);

	bool DrawChunk(Chunk& chunk);
};

