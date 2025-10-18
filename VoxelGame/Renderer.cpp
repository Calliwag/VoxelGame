#include "Renderer.hpp"
#include "blockShader.hpp"
#include "uiShader.hpp"
#include "World.hpp"

Renderer::Renderer(TexData texData, UIData uiData)
{
    this->texData = texData;
    this->uiData = uiData;

    blockShader = ShaderProgram(blockVertexShader, blockFragmentShader, false,
        { "matrix","offset","lightDir","atlas" }, // Uniforms
        { "pos","normIdx","texIndex","uv" }  // Attributes
    );
    blockShader.BindProgram();

    matrixLoc = blockShader.GetVarLoc("matrix");
    normLoc = blockShader.GetVarLoc("normIdx");
    offsetLoc = blockShader.GetVarLoc("offset");
    posLoc = blockShader.GetVarLoc("pos");
    uvLoc = blockShader.GetVarLoc("uv");
    lightDirLoc = blockShader.GetVarLoc("lightDir");
    texIndexLoc = blockShader.GetVarLoc("texIndex");
    atlasLoc = blockShader.GetVarLoc("atlas");

    uiShader = ShaderProgram(uiVertexShader, uiFragmentShader, false,
        { "matrix","tex","color" },
        { "pos","uv" }
    );
    uiShader.BindProgram();

    uiMatrixLoc = uiShader.GetVarLoc("matrix");
    uiTexLoc = uiShader.GetVarLoc("tex");
    uiPosLoc = uiShader.GetVarLoc("pos");
    uiUVLoc = uiShader.GetVarLoc("uv");
    uiColorLoc = uiShader.GetVarLoc("color");

    // Wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Renderer::Update(vec3 pos, vec3 dir, float fovY, float width, float height, vec3 lightDir)
{
    blockShader.BindProgram();
    vec3 up = { 0,0,1 };
    mat4x4 matrix = glm::perspective(3.1416f / 2, width / height, 0.1f, 1000.0f) * glm::lookAt(pos, pos + dir, up);
    blockShader.BindMat4x4(matrix, matrixLoc);
    blockShader.BindVec3(lightDir, lightDirLoc);
    blockShader.BindTextureArray(texData.texArray);
    frustum = Frustum(pos, dir, 0.1f, 1000.0f, fovY, width / height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);
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
    blockShader.BindArray(chunk.faceTexArray, texIndexLoc);
    blockShader.RenderTris(chunk.faceVertArray.count * 2);

    return true;
}

void Renderer::DrawUI(World& world)
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    world.window->SetBlendMode(BlendMode::Alpha);
    float width = world.window->width;
    float height = world.window->height;
    uiShader.BindProgram();
    mat3x3 scaleMatrix = mat3x3(
        2 / width, 0, 0,
        0, 2 / height, 0,
        -1, -1, 1
    );
    mat3x3 shiftMatrix = mat3x3(
        1, 0, -1,
        0, 1, -1,
        0, 0, 1
    );
    mat3x3 matrix = shiftMatrix * scaleMatrix;
    uiShader.BindMat3x3(matrix, uiMatrixLoc);
    uiData.BindQuad(uiShader, uiPosLoc, uiUVLoc);
    uiShader.BindColor(Color::White(1.0), uiColorLoc);

    // Draw crosshair
    uiData.SetQuadCentered({ width / 2, height / 2 }, { height / 64, height / 64 });
    uiData.BindQuad(uiShader, uiPosLoc, uiUVLoc);
    uiData.BindTex(uiShader, 0);
    uiShader.RenderQuad();
}
