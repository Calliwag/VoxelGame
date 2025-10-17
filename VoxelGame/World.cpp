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

    if (length(targetVel) > 1.0)
        targetVel = moveSpeed * normalize(targetVel);
    else
        targetVel *= moveSpeed;
    constexpr float fValue = 10;
    velocity.x = velocity.x * (1 - fValue * dt) + targetVel.x * fValue * dt;
    velocity.y = velocity.y * (1 - fValue * dt) + targetVel.y * fValue * dt;
    targetVel = { 0,0 };

    acceleration = { 0,0,0 };
    vec3 delta = velocity * dt;

    SweepBBox(world, delta, 1.0, dt);

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

AABB Creature::GetAABB()
{
    return AABB::BottomCenterAligned(position, { bboxWidth,bboxWidth,bboxHeight });
}

void Creature::SweepBBox(World& world, vec3 delta, float timeLeft, float dt)
{
    auto cBox = GetAABB();
    auto broadBox = cBox.BroadExpand(delta);
    ivec3 min = floor(broadBox.pos);
    ivec3 max = ceil(broadBox.pos + broadBox.size);

    bool foundIntersect = false;
    float closestIntersect = timeLeft;
    vec3 closestNorm = { 0,0,0 };
    for (int x = min.x; x < max.x; x++)
    {
        for (int y = min.y; y < max.y; y++)
        {
            for (int z = min.z; z < max.z; z++)
            {
                if (!world.cm.IsBlockSolid({ x,y,z }))
                    continue;
                auto bBox = world.cm.GetBlockAABB({ x,y,z });
                vec3 norm;
                float intersect = cBox.SweepCollision(delta, bBox, norm);
                if (intersect < closestIntersect)
                {
                    foundIntersect = true;
                    closestIntersect = intersect;
                    closestNorm = norm;
                }
            }
        }
    }

    if (!foundIntersect)
    {
        position += delta;
    }
    else
    {
        if (closestNorm.z >= 0)
            onGround = true;
        position += delta * closestIntersect;
        timeLeft = timeLeft - closestIntersect;
        velocity -= dot(velocity, closestNorm) * closestNorm;
        delta = velocity * dt * timeLeft;
        if (timeLeft > 0.0)
        {
            SweepBBox(world, delta, timeLeft, dt);
        }
    }
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
