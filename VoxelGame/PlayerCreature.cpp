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
        world.window->mouseDelta = { 0,0 };
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
    }

    if (world.window->IsKeyDown(GLFW_KEY_W))
        targetVel += vec2{ xyDir.x,xyDir.y } * moveSpeed;
    if (world.window->IsKeyDown(GLFW_KEY_S))
        targetVel += vec2{ -xyDir.x,-xyDir.y } * moveSpeed;
    if (world.window->IsKeyDown(GLFW_KEY_A))
        targetVel += vec2{ -xyDir.y,xyDir.x } * moveSpeed;
    if (world.window->IsKeyDown(GLFW_KEY_D))
        targetVel += vec2{ xyDir.y,-xyDir.x } * moveSpeed;

    //constexpr float fValue = 0.95;
    //velocity.x = velocity.x * fValue + targetVel.x * (1 - fValue);
    //velocity.y = velocity.y * fValue + targetVel.y * (1 - fValue);

    if (onGround && world.window->IsKeyDown(GLFW_KEY_SPACE))
    {
        Jump();
    }

    if (world.window->IsKeyPressed(GLFW_KEY_B))
    {
        auto rIter = world.cm.GetRayIter(position + vec3{0,0,1.62}, GetDir());
        bool found = false;
        while (true)
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
            world.cm.MeshChunkModified(rIter.chunk->coordinate);
        }
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
