#include "PlayerCreature.hpp"

PlayerCreature::PlayerCreature()
{
    mouseLock = false;
    position = { 0,0,10 };
    velocity = { 0,0,0 };
    targetVel = { 0,0 };
    acceleration = { 0,0,0 };
    hAngle = 0;
    vAngle = 0;
    moveSpeed = 5;
    angleSpeed = 1;
    bboxWidth = 0.6;
    bboxHeight = 1.8;
}

void PlayerCreature::Act(World& world)
{
    constexpr float mouseSensitivity = 0.01;

    auto xyDir = GetXYDir();
    if (mouseLock)
    {
        hAngle -= world.window->mouseDelta.x * mouseSensitivity;
        vAngle -= world.window->mouseDelta.y * mouseSensitivity;

        if (world.window->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
            BreakBlock(world);
        if (world.window->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
            PlaceBlock(world, inv.currentBlock);

        auto scrollDelta = world.window->GetScrollDelta();
        if(scrollDelta.y <= -1)
            if (inv.currentBlock > 0)
                inv.currentBlock--;
        if(scrollDelta.y >= 1)
            if (inv.currentBlock < 255)
                inv.currentBlock++;
        //#include <cinttypes>
        //printf("%" PRId8 "\n", inv.currentBlock);
    }
    else
    {
        if (world.window->IsKeyDown(GLFW_KEY_LEFT))
            hAngle += angleSpeed * world.window->frameTime;
        if (world.window->IsKeyDown(GLFW_KEY_RIGHT))
            hAngle -= angleSpeed * world.window->frameTime;
        if (world.window->IsKeyDown(GLFW_KEY_UP))
            vAngle += angleSpeed * world.window->frameTime;
        if (world.window->IsKeyDown(GLFW_KEY_DOWN))
            vAngle -= angleSpeed * world.window->frameTime;

        if (world.window->IsKeyPressed(GLFW_KEY_1))
            BreakBlock(world);
        if (world.window->IsKeyPressed(GLFW_KEY_2))
            PlaceBlock(world, 1);

        if (world.window->IsKeyPressed(GLFW_KEY_LEFT_BRACKET))
            if (inv.currentBlock > 0)
                inv.currentBlock--;
        if (world.window->IsKeyPressed(GLFW_KEY_RIGHT_BRACKET))
            if (inv.currentBlock < 255)
                inv.currentBlock++;
    }

    if (world.window->IsKeyDown(GLFW_KEY_W))
        targetVel += vec2{ xyDir.x,xyDir.y };
    if (world.window->IsKeyDown(GLFW_KEY_S))
        targetVel += vec2{ -xyDir.x,-xyDir.y };
    if (world.window->IsKeyDown(GLFW_KEY_A))
        targetVel += vec2{ -xyDir.y,xyDir.x };
    if (world.window->IsKeyDown(GLFW_KEY_D))
        targetVel += vec2{ xyDir.y,-xyDir.x };

    if (onGround && world.window->IsKeyPressed(GLFW_KEY_SPACE))
    {
        Jump();
    }

    if (world.window->IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        if (mouseLock)
        {
            world.window->UnlockMouse();
            mouseLock = false;
        }
        else
        {
            world.window->LockMouse();
            mouseLock = true;
        }
    }
}

void PlayerCreature::PlaceBlock(World& world, u8 type)
{
    auto rIter = world.cm.GetRayIter(position + vec3{ 0,0,1.62 }, GetDir());
    ivec3 last = rIter.pos;
    while (true)
    {
        if (rIter.faceDist >= 4)
        {
            return;
        }
        if (rIter.chunk == nullptr)
        {
            return;
        }
        if (rIter.chunk->data.At(rIter.blockCoord) != 0)
        {
            break;
        }
        last = rIter.pos;
        rIter.Next();
    }

    ivec3 chunkCoord = world.cm.GetChunkCoord(last);
    if (world.chunkCreatures.contains(chunkCoord))
    {
        auto blockAABB = world.cm.GetBlockAABB(last);
        auto& creatures = world.chunkCreatures.at(chunkCoord);
        for (auto* creature : creatures)
        {
            if (creature->GetAABB().Intersects(blockAABB))
                return;
        }
    }

    world.SetBlock(last, type);
}

void PlayerCreature::BreakBlock(World& world)
{
    auto rIter = world.cm.GetRayIter(position + vec3{ 0,0,1.62 }, GetDir());
    while (true)
    {
        if (rIter.faceDist >= 4)
        {
            return;
        }
        if (rIter.chunk == nullptr)
        {
            return;
        }
        if (rIter.chunk->data.At(rIter.blockCoord) != 0)
        {
            world.SetBlock(rIter.pos, 0);
            return;
        }
        rIter.Next();
    }
}
