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

        int blockCount = 3;
        Bitmap atlasImg = Bitmap::GetColorImage(16 * blockCount, 16 * 6, Color{0,0,0,255});
        {
            auto bottom = Bitmap::FromFile("Assets/dirt_bottom.bmp");
            auto top = Bitmap::FromFile("Assets/dirt_top.bmp");
            auto side = Bitmap::FromFile("Assets/dirt_side.bmp");
            DrawToAtlas(atlasImg, bottom, 1, 0);
            DrawToAtlas(atlasImg, top, 1, 1);
            DrawToAtlas(atlasImg, side, 1, 2);
            DrawToAtlas(atlasImg, side, 1, 3);
            DrawToAtlas(atlasImg, side, 1, 4);
            DrawToAtlas(atlasImg, side, 1, 5);
        }
        {
            auto bottom = Bitmap::FromFile("Assets/grass_bottom.bmp");
            auto top = Bitmap::FromFile("Assets/grass_top.bmp");
            auto side = Bitmap::FromFile("Assets/grass_side.bmp");
            DrawToAtlas(atlasImg, bottom, 2, 0);
            DrawToAtlas(atlasImg, top, 2, 1);
            DrawToAtlas(atlasImg, side, 2, 2);
            DrawToAtlas(atlasImg, side, 2, 3);
            DrawToAtlas(atlasImg, side, 2, 4);
            DrawToAtlas(atlasImg, side, 2, 5);
        }

        Texture atlasTex = Texture::FromBitmap(atlasImg);
        atlasTex.GenMipmaps(4, -1.0);

        Renderer r(atlasTex);
        Generator* gen = new WaveGen(0, 8, 128, 128);
        ChunkManager cm(gen);

        int chunksPerFrame = 10;
        int updateListInterval = 60;
        float radius = 32 * 16;
        float vRadius = 4 * 16;

        int frame = 0;
        vec3 pos = { .5,.5, 0 };
        vec3 vel = { 0, 0, 0 };
        vec3 cameraOffset = { 0,0,1.5 };
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
            if (window.IsKeyPressed(GLFW_KEY_RIGHT_CONTROL))
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

            vel.z -= 9.8 * (float)window.frameTime;
            vec3 newPos = pos + vel * (float)window.frameTime;
            vec3 start = newPos;
            auto rIter = cm.GetRayIter(start, vec3{0,0,-1});
            bool success = false;
            for(int i = 0; i < 3; i++)
            {
                if (rIter.chunk == nullptr)
                {
                    success = false;
                    break;
                }
                if (rIter.chunk->data.At(rIter.blockCoord))
                {
                    success = true;
                    break;
                }
                rIter.Next();
            }
            if (success)
            {
                float groundLevel = rIter.pos.z + 1;

                if (newPos.z < groundLevel)
                {
                    pos.z = groundLevel;
                    vel.z = 0;
                    pos.x = newPos.x;
                    pos.y = newPos.y;
                    if (window.IsKeyPressed(GLFW_KEY_SPACE))
                    {
                        vel.z = 10;
                    }
                }
                else
                {
                    pos = newPos;
                }
            }
            else
            {
                pos = newPos;
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