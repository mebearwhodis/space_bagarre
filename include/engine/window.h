#ifndef SPACEBAGARRE_ENGINE_WINDOW_H_
#define SPACEBAGARRE_ENGINE_WINDOW_H_

#include <utility>
#include <SDL3/SDL.h>

namespace spacebagarre
{
    static constexpr int kWindowWidth = 1920;
    static constexpr int kWindowHeight = 1080;

    void StartWindow();
    void StopWindow();

    void UpdateWindow();
    void CloseWindow();

    bool IsWindowOpen();
    SDL_Window* GetWindow();
} // spacebagarre

#endif //SPACEBAGARRE_ENGINE_WINDOW_H_
