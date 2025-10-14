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

void DrawToAtlas(Bitmap& atlas, Bitmap& tex, int blockType, int blockFace)
{
    atlas.DrawBitmap(16 * blockType, 16 * blockFace, tex);
}

int main()
{
    try
    {
        std::cout << "Hello World!\n";

        Core::Init();
        glfwWindowHint(GLFW_SAMPLES, 4);
        Window window(400, 400, "VoxelGame");
        glEnable(GL_MULTISAMPLE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        window.SetBlendMode(BlendMode::Alpha);
        window.BeginContext();

        int blockCount = 2;
        TextureArray texArray(16, 16, 6 * blockCount, 5);
        {
            auto bottom = Bitmap::FromFile("Assets/dirt_bottom.bmp");
            auto top = Bitmap::FromFile("Assets/dirt_top.bmp");
            auto side = Bitmap::FromFile("Assets/dirt_side.bmp");
            texArray.LayerFromBitmap(bottom, 0);
            texArray.LayerFromBitmap(top, 1);
            texArray.LayerFromBitmap(side, 2);
            texArray.LayerFromBitmap(side, 3);
            texArray.LayerFromBitmap(side, 4);
            texArray.LayerFromBitmap(side, 5);
        }
        {
            auto bottom = Bitmap::FromFile("Assets/grass_bottom.bmp");
            auto top = Bitmap::FromFile("Assets/grass_top.bmp");
            auto side = Bitmap::FromFile("Assets/grass_side.bmp");
            texArray.LayerFromBitmap(bottom, 6);
            texArray.LayerFromBitmap(top, 7);
            texArray.LayerFromBitmap(side, 8);
            texArray.LayerFromBitmap(side, 9);
            texArray.LayerFromBitmap(side, 10);
            texArray.LayerFromBitmap(side, 11);
        }

        texArray.GenMipmaps(0);

        Renderer r(texArray);
        Generator* gen = new WaveGen(0, 8, 128, 128);
        ChunkManager cm(gen);

        int chunksPerFrame = 10;
        int updateListInterval = 60;
        float radius = 32 * 16;
        float vRadius = 4 * 16;

        int frame = 0;
        vec3 pos = { 0,0,0 };
        vec3 vel = { 0, 0, 0 };
        vec3 cameraOffset = { 0,0,1.62 };
        vec3 dir = { 1,0,0 };
        float hAngle = 0;
        float vAngle = 0;
        float angleSpeed = 1;
        float moveSpeed = 8;
        bool genNew = true;
        float lHAngle = 3.1416 / 6.f;
        float lVAngle = -3.1416 / 4.f;
        bool mouseLock = false;
        float mouseSensitivity = 0.01;
        while (!window.ShouldClose())
        {
            window.BeginFrame();
            window.PollEvents();
            window.FillScreen({ 135, 206, 235, 255 });

            vec2 hDir = { cos(hAngle),sin(hAngle) };
            vec3 dir = { hDir.x * cos(vAngle),hDir.y * cos(vAngle),sin(vAngle) };
            vec2 lHDir = { cos(lHAngle), sin(lHAngle) };
            vec3 lightDir = { lHDir.x * cos(lVAngle), lHDir.y * cos(lVAngle), sin(lVAngle) };
            r.Update(pos + cameraOffset, dir, 3.1416 / 2, window.width, window.height, lightDir);

            vec3 iPos = pos / (float)CHUNK_SPAN;
            for (auto& chunk : cm.chunks)
            {
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

            vel.x = 0;
            vel.y = 0;
            if(mouseLock)
            {
                hAngle -= window.mouseDelta.x * mouseSensitivity;
                vAngle -= window.mouseDelta.y * mouseSensitivity;
                window.mouseDelta = { 0,0 };
            }
            else
            {
                if (window.IsKeyDown(GLFW_KEY_LEFT))
                    hAngle += angleSpeed * window.frameTime;
                if (window.IsKeyDown(GLFW_KEY_RIGHT))
                    hAngle -= angleSpeed * window.frameTime;
                if (window.IsKeyDown(GLFW_KEY_UP))
                    vAngle += angleSpeed * window.frameTime;
                if (window.IsKeyDown(GLFW_KEY_DOWN))
                    vAngle -= angleSpeed * window.frameTime;
            }
            if (window.IsKeyDown(GLFW_KEY_W))
                vel += vec3{ hDir.x,hDir.y,0 } * moveSpeed;
            if (window.IsKeyDown(GLFW_KEY_S))
                vel -= vec3{ hDir.x,hDir.y,0 } * moveSpeed;
            if (window.IsKeyDown(GLFW_KEY_A))
                vel += vec3{ -hDir.y,hDir.x,0 } * moveSpeed;
            if (window.IsKeyDown(GLFW_KEY_D))
                vel -= vec3{ -hDir.y,hDir.x,0 } * moveSpeed;

            if (window.IsKeyPressed(GLFW_KEY_B))
            {
                auto rIter = cm.GetRayIter(pos + cameraOffset, dir);
                bool found = false;
                while(true)
                {
                    if (rIter.faceDist >= 4)
                    {
                        found = false;
                        break;
                    }
                    if (rIter.chunk == nullptr)
                    {
                        found = false;
                        break;
                    }
                    if (rIter.chunk->data.At(rIter.blockCoord) != 0)
                    {
                        found = true;
                        break;
                    }
                    rIter.Next();
                }
                if (found)
                {
                    rIter.chunk->data.At(rIter.blockCoord) = 0;
                    cm.MeshChunkModified(rIter.chunk->coordinate);
                }
            }

            if (window.IsKeyPressed(GLFW_KEY_G))
            {
                genNew = !genNew;
                std::cout << "press\n";
            }
            if (window.IsKeyPressed(GLFW_KEY_ESCAPE))
            {
                if (mouseLock)
                {
                    window.UnlockMouse();
                    mouseLock = false;
                }
                else
                {
                    window.LockMouse();
                    mouseLock = true;
                }
            }

            vAngle = glm::clamp(vAngle, -3.1415f / 2.f, 3.1415f / 2.f);

            std::cout << "FPS: " << window.GetFPS() << ", chunks to generate: " << cm.toGenerateList.size() << '\n';
            frame++;

            vel.z += -9.8 * window.frameTime;
            auto ray = vel * (float)window.frameTime;

            const float bboxWidth = 0.3;
            const float bboxHeight = 1.8;
            if (vel.z < 0)
            {
                auto rIter = cm.GetRayIter(pos, vec3{ 0, 0, ray.z });
                while(true)
                {
                    if (rIter.faceDist >= length(ray))
                    {
                        pos.z += ray.z;
                        break;
                    }
                    if (cm.IsBlockSolid(rIter.pos))
                    {
                        pos.z -= rIter.faceDist;
                        vel.z = 0;
                        if (window.IsKeyPressed(GLFW_KEY_SPACE))
                        {
                            vel.z = 10;
                        }
                        break;
                    }
                    rIter.Next();
                }
            }
            else if (vel.z > 0)
            {
                auto rIter = cm.GetRayIter(pos + vec3{ 0,0,ray.z } + vec3{ 0,0,bboxHeight }, vec3{ 0,0,1 });
                while (true)
                {
                    if (rIter.faceDist >= length(ray))
                    {
                        pos.z += ray.z;
                        break;
                    }
                    if (cm.IsBlockSolid(rIter.pos))
                    {
                        pos.z += rIter.faceDist;
                        vel.z = 0;
                        break;
                    }
                    rIter.Next();
                }
            }
            if (vel.x < 0)
            {
                auto rIter = cm.GetRayIter(pos + vec3{ ray.x,0,0 } - vec3{ bboxWidth, 0, 0 }, vec3{ -1,0,0 });
                while (true)
                {
                    if (rIter.faceDist >= length(ray))
                    {
                        pos.x += ray.x;
                        break;
                    }
                    if (cm.IsBlockSolid(rIter.pos))
                    {
                        pos.x += -rIter.faceDist;
                        vel.x = 0;
                        break;
                    }
                    rIter.Next();
                }
            }
            else if (vel.x > 0)
            {
                auto rIter = cm.GetRayIter(pos + vec3{ ray.x,0,0 } + vec3{ bboxWidth, 0, 0 }, vec3{ 1,0,0 });
                while (true)
                {
                    if (rIter.faceDist >= length(ray))
                    {
                        pos.x += ray.x;
                        break;
                    }
                    if (cm.IsBlockSolid(rIter.pos))
                    {
                        pos.x += rIter.faceDist;
                        vel.x = 0;
                        break;
                    }
                    rIter.Next();
                }
            }
            if (vel.y < 0)
            {
                auto rIter = cm.GetRayIter(pos + vec3{ 0,ray.y,0 } - vec3{ 0, bboxWidth, 0 }, vec3{ 0,-1,0 });
                while (true)
                {
                    if (rIter.faceDist >= length(ray))
                    {
                        pos.y += ray.y;
                        break;
                    }
                    if (cm.IsBlockSolid(rIter.pos))
                    {
                        pos.y += -rIter.faceDist;
                        vel.y = 0;
                        break;
                    }
                    rIter.Next();
                }
            }
            else if (vel.y > 0)
            {
                auto rIter = cm.GetRayIter(pos + vec3{ 0,ray.y,0 } + vec3{ 0, bboxWidth, 0 }, vec3{ 0,1,0 });
                while (true)
                {
                    if (rIter.faceDist >= length(ray))
                    {
                        pos.y += ray.y;
                        break;
                    }
                    if (cm.IsBlockSolid(rIter.pos))
                    {
                        pos.y += rIter.faceDist;
                        vel.y = 0;
                        break;
                    }
                    rIter.Next();
                }
            }
        }

        window.EndContext();
        window.Destroy();
        Core::DeInit();
    }
    catch (std::exception& err)
    {
        std::cout << err.what();
        throw err;
    }
    return 0;
}