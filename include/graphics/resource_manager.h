#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <string>
#include <SDL3/SDL.h>

namespace idealpotato
{
    struct TextureResource
    {
        SDL_Texture* texture = nullptr;
        int width = 0;
        int height = 0;
    };

    void StartResourceManager();
    void StopResourceManager();

    void RenderBackground();

    bool LoadTexture(const std::string& id, const std::string& filepath);
    TextureResource* GetTextureResource(const std::string& id);
} // idealpotato

#endif //RESOURCE_MANAGER_H
