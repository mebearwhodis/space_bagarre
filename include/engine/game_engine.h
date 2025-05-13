
#ifndef SPACEBAGARRE_ENGINE_GAME_ENGINE_H_
#define SPACEBAGARRE_ENGINE_GAME_ENGINE_H_
#include <SDL3/SDL_render.h>

#include "timer.h"

namespace spacebagarre {

    void StartEngine();
    void RunEngine();
    void StopEngine();

    void RunOfflineGame();
    void RunOnlineGame();
    void DisplayOnlineLobby();
} // spacebagarre

#endif //SPACEBAGARRE_ENGINE_GAME_ENGINE_H_
