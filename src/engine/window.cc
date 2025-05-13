#include "engine/window.h"

#include <iostream>

namespace spacebagarre
{
    namespace
    {
        std::pair window_size_ = {kWindowWidth, kWindowHeight};
        SDL_Window* window_ = nullptr;
        bool is_open_ = false;
    }

    void StartWindow()
    {
        window_ = SDL_CreateWindow("Ideal Potato",
                                   window_size_.first,
                                   window_size_.second,
                                   0);

        if (window_ == nullptr)
        {
            std::cerr << "Failed to create window\n";
            std::terminate();
        }
        is_open_ = true;
    }

    void StopWindow()
    {
        SDL_DestroyWindow(window_);
    }

    void UpdateWindow()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                is_open_ = false;
            }
        }
    }

    void CloseWindow()
    {
        is_open_ = false;
    }

    bool IsWindowOpen()
    {
        return is_open_;
    }

    SDL_Window* GetWindow()
    {
        return window_;
    }
} // spacebagarre
