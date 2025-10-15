#include "World.hpp"
#include <iostream>
#include <limits>

void Creature::Force(vec3 force)
{
    acceleration += force;
}

void Creature::Jump()
{
    velocity.z += 5;
}

void Creature::PhysUpdate(World& world, float dt)
{
    // Reset flags
    onGround = false;

    velocity += acceleration * dt;

    constexpr float fValue = 10;
    velocity.x = velocity.x * (1 - fValue * dt) + targetVel.x * fValue * dt;
    velocity.y = velocity.y * (1 - fValue * dt) + targetVel.y * fValue * dt;
    targetVel = { 0,0 };

    acceleration = { 0,0,0 };
    vec3 delta = velocity * dt;

    if (delta.z < 0)
    {
        auto rIter = world.cm.GetRayIter(position, vec3{ 0, 0, delta.z });
        while (true)
        {
            if (rIter.faceDist >= length(delta))
            {
                position.z += delta.z;
                break;
            }
            if (world.cm.IsBlockSolid(rIter.pos))
            {
                position.z = rIter.pos.z + 1;
                velocity.z = 0;
                onGround = true;
                break;
            }
            rIter.Next();
        }
    }
    else if (delta.z > 0)
    {
        auto rIter = world.cm.GetRayIter(position + vec3{ 0,0,bboxHeight }, vec3{ 0,0,delta.z });
        while (true)
        {
            if (rIter.faceDist >= length(delta))
            {
                position.z += delta.z;
                break;
            }
            if (world.cm.IsBlockSolid(rIter.pos))
            {
                position.z = rIter.pos.z - bboxHeight;
                velocity.z = 0;
                break;
            }
            rIter.Next();
        }
    }
    if (delta.x < 0)
    {
        auto rIter = world.cm.GetRayIter(position + vec3{ delta.x,0,0 } - vec3{ bboxWidth / 2, 0, 0 }, vec3{ -1,0,0 });
        while (true)
        {
            if (rIter.faceDist >= length(delta))
            {
                position.x += delta.x;
                break;
            }
            if (world.cm.IsBlockSolid(rIter.pos) || world.cm.IsBlockSolid(rIter.pos + ivec3{0,0,1}))
            {
                position.x += -rIter.faceDist;
                velocity.x = 0;
                break;
            }
            rIter.Next();
        }
    }
    else if (delta.x > 0)
    {
        auto rIter = world.cm.GetRayIter(position + vec3{ delta.x,0,0 } + vec3{ bboxWidth / 2, 0, 0 }, vec3{ 1,0,0 });
        while (true)
        {
            if (rIter.faceDist >= length(delta))
            {
                position.x += delta.x;
                break;
            }
            if (world.cm.IsBlockSolid(rIter.pos) || world.cm.IsBlockSolid(rIter.pos + ivec3{ 0,0,1 }))
            {
                position.x += rIter.faceDist;
                velocity.x = 0;
                break;
            }
            rIter.Next();
        }
    }
    if (delta.y < 0)
    {
        auto rIter = world.cm.GetRayIter(position + vec3{ 0,delta.y,0 } - vec3{ 0, bboxWidth / 2, 0 }, vec3{ 0,-1,0 });
        while (true)
        {
            if (rIter.faceDist >= length(delta))
            {
                position.y += delta.y;
                break;
            }
            if (world.cm.IsBlockSolid(rIter.pos) || world.cm.IsBlockSolid(rIter.pos + ivec3{ 0,0,1 }))
            {
                position.y += -rIter.faceDist;
                velocity.y = 0;
                break;
            }
            rIter.Next();
        }
    }
    else if (delta.y > 0)
    {
        auto rIter = world.cm.GetRayIter(position + vec3{ 0,delta.y,0 } + vec3{ 0, bboxWidth / 2, 0 }, vec3{ 0,1,0 });
        while (true)
        {
            if (rIter.faceDist >= length(delta))
            {
                position.y += delta.y;
                break;
            }
            if (world.cm.IsBlockSolid(rIter.pos) || world.cm.IsBlockSolid(rIter.pos + ivec3{ 0,0,1 }))
            {
                position.y += rIter.faceDist;
                velocity.y = 0;
                break;
            }
            rIter.Next();
        }
    }

    if (onGround)
    {
        velocity.x *= 1 - 0.95 * dt;
        velocity.y *= 1 - 0.95 * dt;
    }

    vAngle = clamp(vAngle, -3.1415f / 2, 3.1415f / 2);
}

vec2 Creature::GetXYDir()
{
    vec2 hDir = { cos(hAngle),sin(hAngle) };
    return hDir;
}

vec3 Creature::GetDir()
{
    vec2 hDir = { cos(hAngle),sin(hAngle) };
    vec3 dir = { hDir.x * cos(vAngle),hDir.y * cos(vAngle),sin(vAngle) };
    return dir;
}

World::World()
{
    Core::Init();
    glfwWindowHint(GLFW_SAMPLES, 4);
    window = new Window(400, 400, "VoxelGame");
    glEnable(GL_MULTISAMPLE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    window->SetBlendMode(BlendMode::Alpha);
    window->BeginContext();

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

    r = Renderer(texArray);
    Generator* gen = new WaveGen(0, 8, 128, 128);
    cm = ChunkManager(gen);
}

World::~World()
{
    window->EndContext();
    window->Destroy();
    Core::DeInit();
}

void World::AddCreature(Creature* creature)
{
    creatures.push_back(creature);
}

void World::SetViewCreature(Creature* creature)
{
    viewCreature = creature;
}

bool World::ShouldQuit()
{
    return window->ShouldClose();
}

void World::Step(float dt)
{
    for (auto* creature : creatures)
    {
        creature->Act(*this);
    }
    for (auto* creature : creatures)
    {
        creature->Force({ 0,0,-9.8 });
        creature->PhysUpdate(*this, dt);
    }
}

void World::Render()
{
    window->BeginFrame();
    window->PollEvents();
    window->FillScreen({ 135, 206, 235, 255 });

    vec2 hDir = { cos(viewCreature->hAngle),sin(viewCreature->hAngle) };
    vec3 dir = { hDir.x * cos(viewCreature->vAngle),hDir.y * cos(viewCreature->vAngle),sin(viewCreature->vAngle) };
    vec2 lHDir = { cos(-sunHAngle), sin(-sunHAngle) };
    vec3 lightDir = { lHDir.x * cos(-sunVAngle), lHDir.y * cos(-sunVAngle), sin(-sunVAngle) };
    r.Update(viewCreature->position + vec3{0,0,1.62}, dir, 3.1416 / 2, window->width, window->height, lightDir);

    for (auto& chunk : cm.chunks)
    {
        r.DrawChunk(chunk.second);
    }

    window->EndFrame();

    std::cout << "FPS: " << window->GetFPS() << ", chunks to generate: " << cm.toGenerateList.size() << '\n';
}

void World::GenChunks(int count)
{
    for (int i = 0; i < count; i++)
    {
        cm.GenerateOne(viewCreature->position);
    }
}

void World::UpdateToGenerateList()
{
    constexpr float radius = 256;
    constexpr float vRadius = 64;
    cm.UpdateList(viewCreature->position, radius, vRadius);
    cm.UnloadDistant(viewCreature->position, radius, vRadius);
}
