#include "ENG.h"
#include "Game.h"

int main()
{
    Game *game = new Game();

    ENG::Engine &engine = ENG::Engine::GetInstance();

    engine.SetApplication(game);

    if (engine.Init(1920, 1080)) {
        engine.Run();
    }

    engine.Destroy();

    return 0;
}
