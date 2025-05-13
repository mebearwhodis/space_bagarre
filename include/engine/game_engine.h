
#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H
#include <SDL3/SDL_render.h>

#include "timer.h"

namespace idealpotato {

    void StartEngine();
    void RunEngine();
    void StopEngine();

    void RunOfflineGame();
    void RunOnlineGame();
    void DisplayOnlineLobby();
} // idealpotato

#endif //GAME_ENGINE_H
