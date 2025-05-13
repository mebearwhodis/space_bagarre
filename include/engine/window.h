#ifndef WINDOW_H
#define WINDOW_H

#include <utility>
#include <SDL3/SDL.h>

namespace idealpotato
{
    static constexpr int kWindowWidth = 1920;
    static constexpr int kWindowHeight = 1080;

    void StartWindow();
    void StopWindow();

    void UpdateWindow();
    void CloseWindow();

    bool IsWindowOpen();
    SDL_Window* GetWindow();
} // idealpotato

#endif //WINDOW_H
