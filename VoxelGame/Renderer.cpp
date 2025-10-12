#include "Renderer.hpp"

const char* vertexShader = /* vertex shader:*/ R"(
#version 460 core

in vec3 pos;
in vec2 uv;
in int normIdx;
in int blockType;

uniform mat4 matrix;
uniform vec3 offset;
uniform vec3 lightDir;

out vec2 faceLookup;
out float lightValue;
out vec2 fUV;

void main()
{
	gl_Position = matrix * vec4(offset + pos,1);
    faceLookup = vec2(blockType, normIdx); // x = block type, y = block face
    vec3 norm = vec3(0,0,0);
    switch (normIdx)
    {
        // XY faces
        case 0:
            norm.z = -1;
            break;
        case 1:
            norm.z = 1;
            break;
        // YZ faces
        case 2:
            norm.x = -1;
            break;
        case 3:
            norm.x = 1;
            break;
        // XZ faces
        case 4:
            norm.y = -1;
            break;
        case 5:
            norm.y = 1;
            break;
    }
    lightValue = 0.5 * max(0.0, dot(-norm,lightDir)) + 0.5;
    fUV = uv;
};

)";

const char* fragmentShader = /* fragment shader:*/ R"(
#version 460 core

in vec2 faceLookup; // x = block type, y = block face
in float lightValue;
in vec2 fUV;

out vec4 FragColor;

float Value(float val)
{
    if(abs(val - 1.0) <= 0.1) return 1.0;
    if(abs(val - 0.0) <= 0.1) return 0.0;
    return 0.5;
}

void main()
{
    //FragColor = vec4(1,1,1,1) * vec4(lightValue,lightValue,lightValue,1.0);
    //FragColor = vec4(Value(faceLookup.y), Value(faceLookup.y - 2), Value(faceLookup.y - 4), 1);
    FragColor = vec4(mod(fUV.x,1.0),mod(fUV.y,1.0),0,1) * vec4(lightValue,lightValue,lightValue,1.0);
}

)";

Renderer::Renderer()
{
    blockShader = ShaderProgram(vertexShader, fragmentShader, false,
        { "matrix","offset","lightDir"}, // Uniforms
        { "pos","normIdx","blockType","uv"}  // Attributes
    );
    blockShader.BindProgram();

    matrixLoc = blockShader.GetVarLoc("matrix");
    normLoc = blockShader.GetVarLoc("normIdx");
    offsetLoc = blockShader.GetVarLoc("offset");
    posLoc = blockShader.GetVarLoc("pos");
    uvLoc = blockShader.GetVarLoc("uv");
    lightDirLoc = blockShader.GetVarLoc("lightDir");
    blockTypeLoc = blockShader.GetVarLoc("blockType");

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

    blockShader.BindArray(chunk.faceVertArray, posLoc);
    blockShader.BindArray(chunk.faceNormArray, normLoc);
    blockShader.BindArray(chunk.faceUVArray, uvLoc);
    blockShader.BindArray(chunk.faceTypeArray, blockTypeLoc);
    blockShader.RenderTris(chunk.faceVertArray.count * 2);

    return true;
}
