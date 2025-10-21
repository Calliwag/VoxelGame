#pragma once

#include "Chunk.hpp"
#include "SimView/SimView.hpp"
#include "Frustum.hpp"

using namespace SimView;

class Renderer
{
public:
	// 3D rendering
	ShaderProgram blockShader;
	Frustum frustum;
	GLint matrixLoc;
	GLint offsetLoc;
	GLint posLoc;
	GLint normLoc;
	GLint lightDirLoc;
	GLint texIndexLoc;
	GLint uvLoc;
	GLint atlasLoc;
	BlocksData texData;

	// 2D rendering
	ShaderProgram uiShader;
	GLint uiPosLoc;
	GLint uiUVLoc;
	GLint uiColorLoc;
	GLint uiTexLoc;
	GLint uiMatrixLoc;
	UIData uiData;

	Renderer() {}
	Renderer(BlocksData texData, UIData uiData);

	void Update(vec3 pos, vec3 dir, float fovY, float width, float height, vec3 lightDir);
	bool DrawChunk(Chunk& chunk);
	void DrawUI(World& world);
};

