#ifndef SPACEBAGARRE_GRAPHICS_RENDERER_H_
#define SPACEBAGARRE_GRAPHICS_RENDERER_H_
#include <SDL3/SDL_render.h>

namespace spacebagarre
{
    void StartRenderer();
    void StopRenderer();

    SDL_Renderer* GetRenderer();
} //spacebagarre


#endif //SPACEBAGARRE_GRAPHICS_RENDERER_H_
