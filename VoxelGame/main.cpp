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

    float radius = 64 * 1.5;
    float drawRadius = 64;

    int frame = 0;
    vec3 pos = { 0,0,-6 };
    vec3 dir = { 1,0,0 };
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!window.ShouldClose())
    {
        window.BeginFrame();
        window.PollEvents();
        window.FillScreen(Color::Black(1.0f));
        glClear(GL_DEPTH_BUFFER_BIT);

        pos += vec3(.1, 0, 0);
        dir = { cos(frame / 100.f),sin(frame / 100.f),0 };
        //vec2 hDir = { cos(hAngle),sin(hAngle) };
        //vec3 dir = { hDir.x * cos(vAngle),hDir.y * cos(vAngle),sin(vAngle) };
        //dir = -normalize(pos);
        r.Update(pos, dir, 3.1416 / 2, window.width, window.height);

        vec3 iPos = pos / (float)CHUNK_SPAN;
        float iRadius = drawRadius / (float)CHUNK_SPAN;
        for (auto& chunk : cm.chunks)
        {
            if(length((vec3)chunk.second.coordinate - iPos) < iRadius)
                r.DrawChunk(chunk.second);
        }

        window.EndFrame();

        if (frame % 1 == 0)
        {
            cm.UpdateList(pos, radius);
            cm.UnloadDistant(pos, radius);
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