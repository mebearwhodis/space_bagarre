
#ifndef RENDERER_H
#define RENDERER_H
#include <SDL3/SDL_render.h>

namespace idealpotato
{
    void StartRenderer();
    void StopRenderer();

    SDL_Renderer* GetRenderer();
}


#endif //RENDERER_H
