#include <iostream>
#include "SimView/SimView.hpp"
#include "Chunk.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <list>
#include "Frustum.hpp"
#include "Renderer.hpp"

using namespace glm;
using namespace SimView;

int main()
{
    std::cout << "Hello World!\n";
    std::list<Chunk> chunks = {};

    Core::Init();
    Window window(400, 400, "VoxelGame");
    window.SetBlendMode(BlendMode::Alpha);
    window.BeginContext();

    Renderer r;

    int frame = 0;
    vec3 pos = {-1,0,0};
    vec3 dir = { 1,0,0 };
    while (!window.ShouldClose())
    {
        window.BeginFrame();
        window.PollEvents();
        window.FillScreen(Color::Black(1.0f));
        glClear(GL_DEPTH_BUFFER_BIT);

        pos += vec3(.5, 0, 0);
        //vec2 hDir = { cos(hAngle),sin(hAngle) };
        //vec3 dir = { hDir.x * cos(vAngle),hDir.y * cos(vAngle),sin(vAngle) };
        dir = -normalize(pos);
        r.Update(pos, dir, 3.1416 / 2, window.width, window.height);

        for (auto& chunk : chunks)
        {
            r.DrawChunk(chunk);
        }

        window.EndFrame();

        if (frame % 1 == 0)
        {
            ivec3 cgPos = pos / (float)CHUNK_SPAN;
            constexpr int span = 2;
            bool exit = false;
            for (int x = cgPos.x - span; x < cgPos.x + span && !exit; x++)
                for (int y = cgPos.y - span; y < cgPos.y + span && !exit; y++)
                    for (int z = cgPos.z - span; z < cgPos.z + span && !exit; z++)
                    {
                        bool found = false;
                        for (auto& chunk : chunks)
                        {
                            if (chunk.coordinate == ivec3{ x,y,z })
                            {
                                found = true;
                                break;
                            }
                        }
                        if (!found)
                        {
                            Chunk chunk(ivec3{ x,y,z });
                            chunk.GenChunkLevel(-10);
                            chunk.GenFaceGrids();
                            chunks.push_back(chunk);
                            exit = true;
                        }
                    }
        }

        std::cout << "FPS: " << window.GetFPS() << '\n';
        frame++;
    }

    window.EndContext();
    window.Destroy();
    Core::DeInit();
    return 0;
}