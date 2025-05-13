
#include "graphics/resource_manager.h"
#include "graphics/renderer.h"
#include "engine/window.h"

#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_render.h>
#include <unordered_map>
#include <iostream>

namespace idealpotato
{
    static std::unordered_map<std::string, TextureResource> textures;

    void StartResourceManager()
    {
        // Optional: check or log renderer
        if (!GetRenderer())
        {
            std::cerr << "Renderer not initialized before ResourceManager!\n";
        }
        LoadTexture("background", "data/graphics/background/bg.png");
    }

    void StopResourceManager()
    {
        for (auto& [id, texture_res] : textures)
        {
            SDL_DestroyTexture(texture_res.texture);
        }
        textures.clear();
    }

    void RenderBackground()
    {
        TextureResource* bg = GetTextureResource("background");
        if (!bg || !bg->texture)
            return;

        int win_w = kWindowWidth;
        int win_h = kWindowHeight;

        SDL_FRect dst_rect;
        dst_rect.w = static_cast<float>(bg->width);
        dst_rect.h = static_cast<float>(bg->height);
        dst_rect.x = (win_w - dst_rect.w) / 2.0f;
        dst_rect.y = (win_h - dst_rect.h) / 2.0f;

        SDL_RenderTexture(GetRenderer(), bg->texture, nullptr, &dst_rect);
    }




    bool LoadTexture(const std::string& id, const std::string& filepath)
    {
        SDL_Surface* surface = IMG_Load(filepath.c_str());
        if (!surface)
        {
            std::cerr << "Failed to load image at " << filepath << "\n";
            return false;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(GetRenderer(), surface);
        if (!texture)
        {
            std::cerr << "Failed to create texture from '" << filepath << "': " << SDL_GetError() << "\n";
            SDL_DestroySurface(surface);
            return false;
        }

        TextureResource res;
        res.texture = texture;
        res.width = surface->w;
        res.height = surface->h;

        SDL_DestroySurface(surface);
        textures[id] = res;
        return true;
    }
    TextureResource* GetTextureResource(const std::string& id)
    {
        auto it = textures.find(id);
        return (it != textures.end()) ? &it->second : nullptr;
    }
}
// idealpotato