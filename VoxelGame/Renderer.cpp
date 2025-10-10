#include "Renderer.hpp"

const char* vertexShader = /* vertex shader:*/ R"(
#version 460 core

in vec3 pos;

uniform mat4 matrix;
uniform vec3 offset;

void main()
{
	gl_Position = matrix * vec4(offset + pos,1);
};

)";

const char* fragmentShader = /* fragment shader:*/ R"(
#version 460 core

uniform vec4 faceColor;

out vec4 FragColor;

float LinDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0) / 10000 * (1 - z);
}

void main()
{
    float value = 1 - pow(2,-LinDepth(gl_FragCoord.z) * 1000000000.0);
    FragColor = vec4(faceColor.x * value, faceColor.y * value, faceColor.z * value, 1);
    //FragColor = faceColor;
}

)";

Renderer::Renderer()
{
    blockShader = ShaderProgram(vertexShader, fragmentShader, false,
        { "matrix","faceColor","offset" }, // Uniforms
        { "pos" }  // Attributes
    );
    blockShader.BindProgram();

    matrixLoc = blockShader.GetVarLoc("matrix");
    faceColorLoc = blockShader.GetVarLoc("faceColor");
    offsetLoc = blockShader.GetVarLoc("offset");
    posLoc = blockShader.GetVarLoc("pos");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);
}

void Renderer::Update(vec3 pos, vec3 dir, float fovY, float width, float height, vec3 lightDir)
{
    vec3 up = { 0,0,1 };
    mat4x4 matrix = glm::perspective(3.1416f / 2, width / height, 0.0001f, 10000.0f) * glm::lookAt(pos, pos + dir, up);
    blockShader.BindMat4x4(matrix, matrixLoc);
    frustum = Frustum(pos, dir, 0.0001f, 10000.0f, fovY, width / height);
    glClear(GL_DEPTH_BUFFER_BIT);

    lightDir = normalize(lightDir);
    if (!faceShades)
        faceShades = new float[6];
    // Calculate negative XY face shade
    faceShades[0] = 0.5 * glm::max(lightDir.z, 0.f) + 0.5;

    // Calculate positive XY face shade
    faceShades[1] = 0.5 * glm::max(-lightDir.z, 0.f) + 0.5;

    // Calculate negative YZ face shade
    faceShades[2] = 0.5 * glm::max(lightDir.x, 0.f) + 0.5;

    // Calculate positive YZ face shade
    faceShades[3] = 0.5 * glm::max(-lightDir.x, 0.f) + 0.5;

    // Calculate negative XZ face shade
    faceShades[4] = 0.5 * glm::max(lightDir.y, 0.f) + 0.5;

    // Calculate positive XZ face shade
    faceShades[5] = 0.5 * glm::max(-lightDir.y, 0.f) + 0.5;
}

bool Renderer::DrawChunk(Chunk& chunk)
{
    ivec3 size = { CHUNK_SPAN + 1, CHUNK_SPAN + 1, CHUNK_SPAN + 1 };
    ivec3 chunkOffset = CHUNK_SPAN * chunk.coordinate;
    ivec3 frOffset = (ivec3)frustum.pos - chunkOffset;
    blockShader.BindVec3(chunkOffset, offsetLoc);

    if (!frustum.CullAABB(chunkOffset, chunkOffset + ivec3{ CHUNK_SPAN,CHUNK_SPAN,CHUNK_SPAN }))
        return false;
    u8 shade;

    shade = faceShades[0] * 255;
    blockShader.BindColor(Color{shade,shade,shade,1}, faceColorLoc);
    blockShader.BindArray(*chunk.nxyArray, posLoc);
    blockShader.RenderTris(chunk.nxyFaces.size() * 2);

    shade = faceShades[1] * 255;
    blockShader.BindColor(Color{ shade,shade,shade,1 }, faceColorLoc);
    blockShader.BindArray(*chunk.pxyArray, posLoc);
    blockShader.RenderTris(chunk.pxyFaces.size() * 2);

    shade = faceShades[2] * 255;
    blockShader.BindColor(Color{ shade,shade,shade,1 }, faceColorLoc);
    blockShader.BindArray(*chunk.nyzArray, posLoc);
    blockShader.RenderTris(chunk.nyzFaces.size() * 2);

    shade = faceShades[3] * 255;
    blockShader.BindColor(Color{ shade,shade,shade,1 }, faceColorLoc);
    blockShader.BindArray(*chunk.pyzArray, posLoc);
    blockShader.RenderTris(chunk.pyzFaces.size() * 2);

    shade = faceShades[4] * 255;
    blockShader.BindColor(Color{ shade,shade,shade,1 }, faceColorLoc);
    blockShader.BindArray(*chunk.nxzArray, posLoc);
    blockShader.RenderTris(chunk.nxzFaces.size() * 2);

    shade = faceShades[5] * 255;
    blockShader.BindColor(Color{ shade,shade,shade,1 }, faceColorLoc);
    blockShader.BindArray(*chunk.pxzArray, posLoc);
    blockShader.RenderTris(chunk.pxzFaces.size() * 2);

    return true;
}
