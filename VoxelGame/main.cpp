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
    //Generator* gen = new FlatGen(-8, 1);
    Generator* gen = new WaveGen(1, -8, 4, 32, 32);
    ChunkManager cm(gen);

    int chunksPerFrame = 10;
    int updateListInterval = 10;
    float radius = 512;
    float vRadius = 16;
    float drawRadius = 512;

    int frame = 0;
    vec3 pos = { .5,.5, 0 };
    vec3 dir = { 1,0,0 };
    float hAngle = 0;
    float vAngle = 0;
    float angleSpeed = 1;
    float moveSpeed = 8;
    bool genNew = true;
    float lHAngle = 3.1416 / 6.f;
    float lVAngle = -3.1416 / 4.f;

    while (!window.ShouldClose())
    {
        window.BeginFrame();
        window.PollEvents();
        window.FillScreen(Color::Black(1.0f));

        vec2 hDir = { cos(hAngle),sin(hAngle) };
        vec3 dir = { hDir.x * cos(vAngle),hDir.y * cos(vAngle),sin(vAngle) };
        vec2 lHDir = { cos(lHAngle), sin(lHAngle) };
        vec3 lightDir = { lHDir.x * cos(lVAngle), lHDir.y * cos(lVAngle), sin(lVAngle) };
        r.Update(pos, dir, 3.1416 / 2, window.width, window.height, lightDir);

        vec3 iPos = pos / (float)CHUNK_SPAN;
        float iRadius = drawRadius / (float)CHUNK_SPAN;
        for (auto& chunk : cm.chunks)
        {
            if(length((vec3)chunk.second.coordinate - iPos) < iRadius)
                r.DrawChunk(chunk.second);
        }

        window.EndFrame();

        if (genNew && frame % updateListInterval == 0)
        {
            cm.UpdateList(pos, radius, vRadius);
            cm.UnloadDistant(pos, radius, vRadius);
        }
        if (genNew && frame % 1 == 0)
        {
            for (int i = 0; i < chunksPerFrame; i++)
            {
                cm.GenerateOne(pos);
            }
        }

        if (window.IsKeyDown(GLFW_KEY_LEFT))
            hAngle += angleSpeed * window.frameTime;
        if (window.IsKeyDown(GLFW_KEY_RIGHT))
            hAngle -= angleSpeed * window.frameTime;
        if (window.IsKeyDown(GLFW_KEY_UP))
            vAngle += angleSpeed * window.frameTime;
        if (window.IsKeyDown(GLFW_KEY_DOWN))
            vAngle -= angleSpeed * window.frameTime;
        if (window.IsKeyDown(GLFW_KEY_W))
            pos += vec3{ hDir.x,hDir.y,0 } *moveSpeed * (float)window.frameTime;
        if (window.IsKeyDown(GLFW_KEY_S))
            pos -= vec3{ hDir.x,hDir.y,0 } *moveSpeed * (float)window.frameTime;
        if (window.IsKeyDown(GLFW_KEY_A))
            pos += vec3{ -hDir.y,hDir.x,0 } *moveSpeed * (float)window.frameTime;
        if (window.IsKeyDown(GLFW_KEY_D))
            pos -= vec3{ -hDir.y,hDir.x,0 } *moveSpeed * (float)window.frameTime;
        if (window.IsKeyPressed(GLFW_KEY_SPACE))
        {
            genNew = !genNew;
            std::cout << "press\n";
        }

        vAngle = glm::clamp(vAngle, -3.1415f / 2.f, 3.1415f / 2.f);

        std::cout << "FPS: " << window.GetFPS() << ", chunks to generate: " << cm.toGenerateList.size() << '\n';
        frame++;
    }

    window.EndContext();
    window.Destroy();
    Core::DeInit();
    return 0;
}