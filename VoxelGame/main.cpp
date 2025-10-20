#include "World.hpp"
#include "PlayerCreature.hpp"

using namespace glm;
using namespace SimView;

int main()
{
    World w;

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