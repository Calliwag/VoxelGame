#include "World.hpp"
#include <iostream>
#include <limits>
#include "Creature.hpp"
#include "PlayerCreature.hpp"

World::World()
{
    Core::Init();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    window = new Window(400, 400, "VoxelGame");
    glEnable(GL_MULTISAMPLE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    window->SetBlendMode(BlendMode::Alpha);
    window->BeginContext();

    TexData texData({ "noTex","dirt_top","dirt_side","grass_top","grass_side","rock_top","rock_side","wood","brick","glass"});
    texData.MarkBlockTransparent(0);
    int map1[] = { 1,1,2,2,2,2 };
    texData.LinkBlockTextures(1, map1);
    int map2[] = { 1,3,4,4,4,4 };
    texData.LinkBlockTextures(2, map2);
    int map3[] = { 5,5,6,6,6,6 };
    texData.LinkBlockTextures(3, map3);
    int map4[] = { 7,7,7,7,7,7 };
    texData.LinkBlockTextures(4, map4);
    int map5[] = { 8,8,8,8,8,8 };
    texData.LinkBlockTextures(5, map5);
    int map6[] = { 9,9,9,9,9,9 };
    texData.LinkBlockTextures(6, map6);
    texData.MarkBlockTransparent(6);

    UIData uiData({ "crosshair" });

    r = Renderer(texData, uiData);
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
        cm.GenerateOne(player->position, r.texData);
    }
}

void World::UpdateToGenerateList()
{
    constexpr float radius = 256;
    constexpr float vRadius = 64;
    cm.UpdateList(player->position, radius, vRadius);
    cm.UnloadDistant(player->position, radius, vRadius);
}

void World::SetBlock(ivec3 coord, blockType value)
{
    cm.SetBlock(coord, value, r.texData);
}
