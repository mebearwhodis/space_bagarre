#include "graphics/resource_manager.h"
#include "graphics/renderer.h"
#include "engine/window.h"

#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_render.h>
#include <unordered_map>
#include <iostream>
#include <commons/commons.h>

namespace spacebagarre
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
        LoadTexture("get_ready", "data/graphics/get_ready.png");
        LoadTexture("game_over", "data/graphics/game_over.png");
        LoadTexture("pink_player", "data/graphics/characters/pink/shipPink_manned.png");
        LoadTexture("green_player", "data/graphics/characters/green/shipGreen_manned.png");
        LoadTexture("gold_star", "data/graphics/gold_star.png");
        LoadTexture("letter_p", "data/graphics/letters/letterP.png");
        LoadTexture("num_0", "data/graphics/numbers/number0.png");
        LoadTexture("num_1", "data/graphics/numbers/number1.png");
        LoadTexture("num_2", "data/graphics/numbers/number2.png");
        LoadTexture("num_3", "data/graphics/numbers/number3.png");
        LoadTexture("num_4", "data/graphics/numbers/number4.png");
        LoadTexture("num_5", "data/graphics/numbers/number5.png");
        LoadTexture("num_6", "data/graphics/numbers/number6.png");
        LoadTexture("num_7", "data/graphics/numbers/number7.png");
        LoadTexture("num_8", "data/graphics/numbers/number8.png");
        LoadTexture("num_9", "data/graphics/numbers/number9.png");
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
        dst_rect.w = static_cast<fp>(bg->width);
        dst_rect.h = static_cast<fp>(bg->height);
        dst_rect.x = (win_w - dst_rect.w) / 2.0f;
        dst_rect.y = (win_h - dst_rect.h) / 2.0f;

        SDL_RenderTexture(GetRenderer(), bg->texture, nullptr, &dst_rect);
    }

    void RenderGetReadyOverlay()
    {
        TextureResource* tex = GetTextureResource("get_ready");
        if (!tex || !tex->texture)
            return;

        SDL_FRect dst;
        dst.w = static_cast<float>(tex->width);
        dst.h = static_cast<float>(tex->height);
        dst.x = (kWindowWidth - dst.w) / 2.0f;
        dst.y = (kWindowHeight - dst.h) / 2.0f;

        SDL_RenderTexture(GetRenderer(), tex->texture, nullptr, &dst);
    }

    void RenderGameOverOverlay(const PlayerCharacterManager& player_manager)
    {
        // Render "Game Over" image
        TextureResource* tex = GetTextureResource("game_over");
        if (tex && tex->texture)
        {
            SDL_FRect dst;
            dst.w = static_cast<float>(tex->width);
            dst.h = static_cast<float>(tex->height);
            dst.x = (kWindowWidth - dst.w) / 2.0f;
            dst.y = (kWindowHeight - dst.h) / 2.0f;
            SDL_RenderTexture(GetRenderer(), tex->texture, nullptr, &dst);
        }

        // Position anchors
        const float label_y = kWindowHeight / 3.0f;

        const float p1_x = kWindowWidth / 3.0f;
        const float p2_x = kWindowWidth - kWindowWidth / 3.0f;

        // Draw P1
        TextureResource* letter_p = GetTextureResource("letter_p");
        TextureResource* num_1 = GetTextureResource("num_1");
        TextureResource* num_2 = GetTextureResource("num_2");

        float base_y = label_y;
        if (letter_p && num_1 && letter_p->texture && num_1->texture)
        {
            // Player 1 label ("P1")
            SDL_FRect p1_rect = {
                p1_x - static_cast<float>(letter_p->width),
                base_y - static_cast<float>(letter_p->height),
                static_cast<float>(letter_p->width),
                static_cast<float>(letter_p->height)
            };

            SDL_FRect n1_rect = {
                p1_x,
                base_y - static_cast<float>(num_1->height),
                static_cast<float>(num_1->width),
                static_cast<float>(num_1->height)
            };

            SDL_RenderTexture(GetRenderer(), letter_p->texture, nullptr, &p1_rect);
            SDL_RenderTexture(GetRenderer(), num_1->texture, nullptr, &n1_rect);
        }

        // Draw P2
        if (letter_p && num_2 && letter_p->texture && num_2->texture)
        {
            SDL_FRect p2_rect = {
                p2_x - static_cast<float>(letter_p->width),
                base_y - static_cast<float>(letter_p->height),
                static_cast<float>(letter_p->width),
                static_cast<float>(letter_p->height)
            };

            SDL_FRect n2_rect = {
                p2_x,
                base_y - static_cast<float>(num_2->height),
                static_cast<float>(num_2->width),
                static_cast<float>(num_2->height)
            };

            SDL_RenderTexture(GetRenderer(), letter_p->texture, nullptr, &p2_rect);
            SDL_RenderTexture(GetRenderer(), num_2->texture, nullptr, &n2_rect);
        }

        // Draw scores below
        for (int i = 0; i < kMaxPlayers; ++i)
        {
            int score = player_manager.GetScore(i);
            std::string score_str = std::to_string(score);
            float x = (i == 0) ? p1_x : p2_x;
            float digit_y = label_y + 80.0f;

            // Render digits centered
            float total_width = 0.0f;
            for (char digit : score_str)
            {
                std::string key = std::string("num_") + digit;
                TextureResource* num = GetTextureResource(key);
                if (num && num->texture)
                    total_width += num->width;
            }

            float start_x = x - total_width / 2.0f;
            for (char digit : score_str)
            {
                std::string key = std::string("num_") + digit;
                TextureResource* num = GetTextureResource(key);
                if (!num || !num->texture) continue;

                SDL_FRect dst = {
                    start_x,
                    digit_y,
                    static_cast<float>(num->width),
                    static_cast<float>(num->height)
                };
                SDL_RenderTexture(GetRenderer(), num->texture, nullptr, &dst);
                start_x += num->width;
            }
        }
    }


    void RenderPlayers(const PlayerCharacterManager& player_manager,
                       const crackitos_physics::physics::PhysicsWorld& physics_world)
    {
        TextureResource* pink_tex = GetTextureResource("pink_player");
        TextureResource* green_tex = GetTextureResource("green_player");

        if (!pink_tex || !pink_tex->texture || !green_tex || !green_tex->texture)
            return;

        for (int i = 0; i < kMaxPlayers; ++i)
        {
            const auto& player = player_manager.GetPlayer(i);
            const auto& body = physics_world.GetBody(player.body_);
            const auto& pos = body.position();

            TextureResource* tex = (i == 0) ? pink_tex : green_tex;

            // --- Sprite
            SDL_FRect sprite_dst;
            sprite_dst.w = static_cast<float>(tex->width);
            sprite_dst.h = static_cast<float>(tex->height);
            sprite_dst.x = pos.x - sprite_dst.w / 2.0f;
            sprite_dst.y = pos.y - sprite_dst.h / 2.0f;
            SDL_RenderTexture(GetRenderer(), tex->texture, nullptr, &sprite_dst);

            // --- Collider rectangle (overlay)
            // SDL_SetRenderDrawColor(GetRenderer(), 255, 0, 255, 128); // semi-transparent magenta
            // SDL_FRect collider_rect;
            // collider_rect.w = 72.0f;
            // collider_rect.h = 114.0f;
            // collider_rect.x = pos.x - collider_rect.w / 2.0f;
            // collider_rect.y = pos.y - collider_rect.h / 2.0f;
            // SDL_RenderRect(GetRenderer(), &collider_rect); // outline only
        }
    }

    void RenderActiveCoins(const CoinManager& manager)
    {
        TextureResource* coin_tex = GetTextureResource("gold_star");
        if (!coin_tex || !coin_tex->texture) return;

        const auto& coins = manager.GetCoins();

        for (size_t i = 0; i < coins.size(); ++i)
        {
            if (!coins[i].active_) continue;

            const crackitos_core::math::Vec2f& pos = kCoinPositions[i];

            SDL_FRect dst;
            dst.w = static_cast<float>(coin_tex->width);
            dst.h = static_cast<float>(coin_tex->height);
            dst.x = pos.x - dst.w / 2.0f;
            dst.y = pos.y - dst.h / 2.0f;

            SDL_RenderTexture(GetRenderer(), coin_tex->texture, nullptr, &dst);
        }
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

// spacebagarre
