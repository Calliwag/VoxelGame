#include "World.hpp"
#include "PlayerCreature.hpp"

using namespace glm;
using namespace SimView;

int main()
{
    Core::Init();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    Window* window = new Window(400, 400, "VoxelGame");
    glEnable(GL_MULTISAMPLE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    window->SetBlendMode(BlendMode::Alpha);
    window->BeginContext();

    BlockResources blockResources({ "noTex","dirt_top","dirt_side","grass_top","grass_side","rock_top","rock_side","wood","brick","glass" });
    Block* airBlock = new DecoBlock({ 0,0,0,0,0,0 }, true);
    Block* dirtBlock = new DecoBlock({ 1,1,2,2,2,2 }, false);
    Block* grassBlock = new DecoBlock({ 1,3,4,4,4,4 }, false);
    Block* rockBlock = new DecoBlock({ 5,5,6,6,6,6 }, false);
    Block* woodBlock = new DecoBlock({ 7,7,7,7,7,7 }, false);
    Block* brickBlock = new DecoBlock({ 8,8,8,8,8,8 }, false);
    Block* glassBlock = new DecoBlock({ 9,9,9,9,9,9 }, true);

    blockResources.AddBlock(airBlock);
    blockResources.AddBlock(dirtBlock);
    blockResources.AddBlock(grassBlock);
    blockResources.AddBlock(rockBlock);
    blockResources.AddBlock(woodBlock);
    blockResources.AddBlock(brickBlock);
    blockResources.AddBlock(glassBlock);
    //blockResources.MarkBlockTransparent(0);
    //int map1[] = { 1,1,2,2,2,2 };
    //blockResources.LinkBlockTextures(1, map1);
    //int map2[] = { 1,3,4,4,4,4 };
    //blockResources.LinkBlockTextures(2, map2);
    //int map3[] = { 5,5,6,6,6,6 };
    //blockResources.LinkBlockTextures(3, map3);
    //int map4[] = { 7,7,7,7,7,7 };
    //blockResources.LinkBlockTextures(4, map4);
    //int map5[] = { 8,8,8,8,8,8 };
    //blockResources.LinkBlockTextures(5, map5);
    //int map6[] = { 9,9,9,9,9,9 };
    //blockResources.LinkBlockTextures(6, map6);
    //blockResources.MarkBlockTransparent(6);

    UIResources uiResources({ "crosshair" });
    Renderer renderer = Renderer(blockResources, uiResources);

    Generator* generator = new WaveGen(0, 8, 32, 128);

    World w(window,renderer,generator);

    auto player = new PlayerCreature();
    w.AddCreature(player);
    w.SetPlayer(player);

    int frame = 0;
    bool shouldGenerateChunks = true;
    while (!w.ShouldQuit())
    {
        w.Step(w.window->frameTime);
        if (shouldGenerateChunks)
        {
            if (frame % 10)
                w.UpdateToGenerateList();
            w.GenChunks(10);
        }
        w.Render();

        if (w.window->IsKeyPressed(GLFW_KEY_G))
            shouldGenerateChunks = !shouldGenerateChunks;

        frame++;
    }

    return 0;
}