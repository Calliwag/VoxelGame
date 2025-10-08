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

void main()
{
    FragColor = faceColor;
}

)";

Renderer::Renderer()
{
    blockShader = ShaderProgram(vertexShader, fragmentShader, false,
        { "matrix","faceColor","offset" }, // Uniforms
        { "pos" }  // Attributes
    );
    blockShader.BindProgram();

    vec3 pos = { -16,0,16 };
    float hAngle = 3.1416 / 4;
    float vAngle = 0;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    int frame = 0;

    Grid<vec3, CHUNK_SPAN + 1, CHUNK_SPAN + 1, CHUNK_SPAN + 1> verticesGrid;
    for (int x = 0; x < CHUNK_SPAN + 1; x++)
        for (int y = 0; y < CHUNK_SPAN + 1; y++)
            for (int z = 0; z < CHUNK_SPAN + 1; z++)
            {
                verticesGrid.At({ x,y,z }) = { x,y,z };
            }
    chunkVertices = VArray(pow(CHUNK_SPAN + 1, 3), 3, verticesGrid.data);
    blockShader.BindArray(chunkVertices, "pos");
}

void Renderer::Update(vec3 pos, vec3 dir, float fovY, float width, float height)
{
    vec3 up = { 0,0,1 };
    mat4x4 matrix = glm::perspective(3.1416f / 2, width / height, 0.01f, 10000.0f) * glm::lookAt(pos, pos + dir, up);
    blockShader.BindMat4x4(matrix, "matrix");
    frustum = Frustum(pos, dir, 0.01f, 10000.0f, fovY, width / height);
}

bool Renderer::DrawChunk(Chunk& chunk)
{
    int rTris[6];
    IArray tris(6, rTris);

    ivec3 size = { CHUNK_SPAN + 1, CHUNK_SPAN + 1, CHUNK_SPAN + 1 };
    ivec3 chunkOffset = CHUNK_SPAN * chunk.coordinate;
    blockShader.BindVec3(chunkOffset, "offset");

    for (int i = 0; i < chunk.faces.size(); i++)
    {
        auto& face = chunk.faces[i];
        if (face.plane == XY)
        {
            blockShader.BindColor(Color::Red(1.0f), "faceColor");
        }
        if (face.plane == YZ)
        {
            blockShader.BindColor(Color::Green(1.0f), "faceColor");
        }
        if (face.plane == XZ)
        {
            blockShader.BindColor(Color::Blue(1.0f), "faceColor");
        }
        if (!frustum.CullPoint(face.verts[0] + chunkOffset) && !frustum.CullPoint(face.verts[1] + chunkOffset) && !frustum.CullPoint(face.verts[2] + chunkOffset) && !frustum.CullPoint(face.verts[3] + chunkOffset))
            continue;
        int c0 = GetIndex(face.verts[0], size);
        int c1 = GetIndex(face.verts[1], size);
        int c2 = GetIndex(face.verts[2], size);
        int c3 = GetIndex(face.verts[3], size);
        rTris[0] = c0;
        rTris[1] = c1;
        rTris[2] = c2;
        rTris[3] = c2;
        rTris[4] = c3;
        rTris[5] = c0;
        tris.Set(0, 6, rTris);

        blockShader.BindIndexArray(tris);

        blockShader.RenderTris(2);
        //blockShader.RenderPoints(6);
    }
    return true;
}
