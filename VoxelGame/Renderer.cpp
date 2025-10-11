#include "Renderer.hpp"

const char* vertexShader = /* vertex shader:*/ R"(
#version 460 core

in vec3 pos;
in vec3 norm;

uniform mat4 matrix;
uniform vec3 offset;
uniform vec3 lightDir;

out float lightValue;

void main()
{
	gl_Position = matrix * vec4(offset + pos,1);
    lightValue = 0.5 * max(0.0, dot(-norm,lightDir)) + 0.5;
};

)";

const char* fragmentShader = /* fragment shader:*/ R"(
#version 460 core

in float lightValue;

out vec4 FragColor;

void main()
{
    FragColor = vec4(lightValue,lightValue,lightValue,1.0);
}

)";

Renderer::Renderer()
{
    blockShader = ShaderProgram(vertexShader, fragmentShader, false,
        { "matrix","offset","lightDir"}, // Uniforms
        { "pos","norm"}  // Attributes
    );
    blockShader.BindProgram();

    matrixLoc = blockShader.GetVarLoc("matrix");
    normLoc = blockShader.GetVarLoc("norm");
    offsetLoc = blockShader.GetVarLoc("offset");
    posLoc = blockShader.GetVarLoc("pos");
    lightDirLoc = blockShader.GetVarLoc("lightDir");

    // Depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    // Wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Renderer::Update(vec3 pos, vec3 dir, float fovY, float width, float height, vec3 lightDir)
{
    vec3 up = { 0,0,1 };
    mat4x4 matrix = glm::perspective(3.1416f / 2, width / height, 0.001f, 1000.0f) * glm::lookAt(pos, pos + dir, up);
    blockShader.BindMat4x4(matrix, matrixLoc);
    blockShader.BindVec3(lightDir, lightDirLoc);
    frustum = Frustum(pos, dir, 0.001f, 1000.0f, fovY, width / height);
    glClear(GL_DEPTH_BUFFER_BIT);
}

bool Renderer::DrawChunk(Chunk& chunk)
{
    if (!chunk.arraysGenerated)
        chunk.GenArrays();
    if (chunk.faceCount == 0)
        return false;
    ivec3 size = { CHUNK_SPAN + 1, CHUNK_SPAN + 1, CHUNK_SPAN + 1 };
    ivec3 chunkOffset = CHUNK_SPAN * chunk.coordinate;
    if (!frustum.CullAABB(chunkOffset, chunkOffset + ivec3{ CHUNK_SPAN,CHUNK_SPAN,CHUNK_SPAN }))
        return false;

    blockShader.BindVec3(chunkOffset, offsetLoc);

    blockShader.BindArray(*chunk.faceVertArray, posLoc);
    blockShader.BindArray(*chunk.faceNormArray, normLoc);
    blockShader.RenderTris(chunk.faceVertArray->count * 2);

    return true;
}
