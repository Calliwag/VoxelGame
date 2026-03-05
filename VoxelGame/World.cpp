#include "World.hpp"
#include <iostream>
#include <limits>
#include "Creature.hpp"
#include "PlayerCreature.hpp"

World::World(Window* Window, Renderer& Renderer, Generator* Generator)
{
    window = Window;
    r = Renderer;
    cm = ChunkManager(Generator);
    player = nullptr;
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

void World::SetPlayer(PlayerCreature* player)
{
    this->player = player;
}

bool World::ShouldQuit()
{
    return window->ShouldClose();
}

void World::Step(float dt)
{
    chunkCreatures = {};
    for (auto* creature : creatures)
    {
        auto cBox = creature->GetAABB();
        ivec3 minB = floor(cBox.pos);
        ivec3 maxB = ceil(cBox.pos + cBox.size);
        ivec3 minC = cm.GetChunkCoord(minB);
        ivec3 maxC = cm.GetChunkCoord(maxB);

        for (int x = minC.x; x <= maxC.x; x++)
            for (int y = minC.y; y <= maxC.y; y++)
                for (int z = minC.z; z <= maxC.z; z++)
                {
                    chunkCreatures[ivec3{ x,y,z }].push_back(creature);
                }
    }
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

    vec2 hDir = { cos(player->hAngle),sin(player->hAngle) };
    vec3 dir = { hDir.x * cos(player->vAngle),hDir.y * cos(player->vAngle),sin(player->vAngle) };
    vec2 lHDir = { cos(-sunHAngle), sin(-sunHAngle) };
    vec3 lightDir = { lHDir.x * cos(-sunVAngle), lHDir.y * cos(-sunVAngle), sin(-sunVAngle) };
    r.Update(player->position + vec3{0,0,1.62}, dir, 3.1416 / 2, window->width, window->height, lightDir);

    for (auto& chunk : cm.chunks)
    {
        r.DrawChunk(chunk.second);
    }

    r.DrawUI(*this);

    window->EndFrame();

    std::cout << "FPS: " << window->GetFPS() << ", chunks to generate: " << cm.toGenerateList.size() << '\n';
}

void World::GenChunks(int count)
{
    for (int i = 0; i < count; i++)
    {
        cm.GenerateOne(player->position, r.blockResources);
    }
}

void World::UpdateToGenerateList()
{
    constexpr float radius = 256;
    constexpr float vRadius = 64;
    cm.UpdateList(player->position, radius, vRadius);
    cm.UnloadDistant(player->position, radius, vRadius);
}

void World::SetBlock(ivec3 coord, blockID value)
{
    cm.SetBlock(coord, value, r.blockResources);
}
