#include "graphics/renderer.h"

#include <engine/window.h>

namespace spacebagarre
{
    namespace
    {
        SDL_Renderer* renderer_;
    }

    void StartRenderer()
    {
        renderer_ = SDL_CreateRenderer(GetWindow(), nullptr);
    }

    void StopRenderer()
    {
        SDL_DestroyRenderer(renderer_);
    }

    SDL_Renderer* GetRenderer()
    {
        return renderer_;
    }
} // spacebagarre
