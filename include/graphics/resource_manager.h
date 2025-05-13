#ifndef SPACEBAGARRE_GRAPHICS_RESOURCE_MANAGER_H_
#define SPACEBAGARRE_GRAPHICS_RESOURCE_MANAGER_H_

#include <string>
#include <game/coin_manager.h>
#include <game/player_character_manager.h>
#include <SDL3/SDL.h>

namespace spacebagarre
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
    void RenderGetReadyOverlay();
    void RenderGameOverOverlay(const PlayerCharacterManager& player_manager);
    void RenderPlayers(const PlayerCharacterManager& player_manager,
                   const crackitos_physics::physics::PhysicsWorld& physics_world);

    void RenderActiveCoins(const CoinManager& manager);

    bool LoadTexture(const std::string& id, const std::string& filepath);
    TextureResource* GetTextureResource(const std::string& id);
} // spacebagarre

#endif //SPACEBAGARRE_GRAPHICS_RESOURCE_MANAGER_H_
