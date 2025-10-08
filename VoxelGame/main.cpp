#include <iostream>
#include "SimView/SimView.hpp"
#include "Chunk.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <list>
#include "Frustum.hpp"
#include "Renderer.hpp"
#include "ChunkManager.hpp"

using namespace glm;
using namespace SimView;

int main()
{
    std::cout << "Hello World!\n";

    Core::Init();
    Window window(400, 400, "VoxelGame");
    window.SetBlendMode(BlendMode::Alpha);
    window.BeginContext();

    Renderer r;
    ChunkManager cm;


    int frame = 0;
    vec3 pos = {-1,0,0};
    vec3 dir = { 1,0,0 };
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!window.ShouldClose())
    {
        window.BeginFrame();
        window.PollEvents();
        window.FillScreen(Color::Black(1.0f));
        glClear(GL_DEPTH_BUFFER_BIT);

        pos += vec3(.25, 0, 0);
        //vec2 hDir = { cos(hAngle),sin(hAngle) };
        //vec3 dir = { hDir.x * cos(vAngle),hDir.y * cos(vAngle),sin(vAngle) };
        dir = -normalize(pos);
        r.Update(pos, dir, 3.1416 / 2, window.width, window.height);

        for (auto& chunk : cm.chunks)
        {
            r.DrawChunk(chunk);
        }

        window.EndFrame();

        if (frame % 1 == 0)
        {
            cm.UpdateList(pos, 64);
            cm.UnloadDistant(pos, 64);
            cm.GenerateOne(pos);
        }

        std::cout << "FPS: " << window.GetFPS() << '\n';
        frame++;
    }

    window.EndContext();
    window.Destroy();
    Core::DeInit();
    return 0;
}