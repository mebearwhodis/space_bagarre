#include "game/game_contact_listener.h"

#include <game/player_character_manager.h>

namespace spacebagarre
{
    void GameContactListener::RegisterToListenerMap(ObjectType type,
                                                    const crackitos_physics::physics::ColliderHandle& handle)
    {
        collider_type_map_[handle.id] = type;
    }

    void GameContactListener::OnCollisionEnter(const crackitos_physics::physics::ColliderPair& pair)
    {
        auto get_type = [&](const crackitos_physics::physics::ColliderHandle& handle) -> ObjectType
        {
            auto it = collider_type_map_.find(handle.id);
            return it != collider_type_map_.end() ? it->second : ObjectType::kOther;
        };

        ObjectType typeA = get_type(pair.colliderA);
        ObjectType typeB = get_type(pair.colliderB);

        if (typeA == ObjectType::kPlayer && typeB == ObjectType::kWall)
        {
            if (player_manager_)
            {
                for (int i = 0; i < kMaxPlayers; ++i)
                {
                    const auto& player = player_manager_->GetPlayer(i);
                    if (player.collider_.id == pair.colliderA.id)
                    {
                        auto& body = player_manager_->GetMutableBody(i);
                        body.set_position(player_manager_->GetPlayerStartPosition(i));
                        body.set_velocity({0.0f, 0.0f});
                        player_manager_->GetMutablePlayer(i).respawn_timer_ = 3.0f;
                        int current_score = player_manager_->GetScore(i);
                        if (current_score >= 50)
                        {
                            player_manager_->AddScore(i, -50);
                        }
                        else
                        {
                            player_manager_->AddScore(i, -current_score);
                        }
                    }
                }
            }
        }
        else if (typeB == ObjectType::kPlayer && typeA == ObjectType::kWall)
        {
            if (player_manager_)
            {
                for (int i = 0; i < kMaxPlayers; ++i)
                {
                    const auto& player = player_manager_->GetPlayer(i);
                    if (player.collider_.id == pair.colliderB.id)
                    {
                        auto& body = player_manager_->GetMutableBody(i);
                        body.set_position(player_manager_->GetPlayerStartPosition(i));
                        body.set_velocity({0.0f, 0.0f});
                        player_manager_->GetMutablePlayer(i).respawn_timer_ = 3.0f;
                        int current_score = player_manager_->GetScore(i);
                        if (current_score >= 50)
                        {
                            player_manager_->AddScore(i, -50);
                        }
                        else
                        {
                            player_manager_->AddScore(i, -current_score);
                        }
                    }
                }
            }
        }
        else if (typeA == ObjectType::kPlayer && typeB == ObjectType::kPlayer)
        {

        }

}

void GameContactListener::OnTriggerEnter(const crackitos_physics::physics::ColliderPair& pair)
{
    auto get_type = [&](const crackitos_physics::physics::ColliderHandle& handle) -> ObjectType
    {
        auto it = collider_type_map_.find(handle.id);
        return it != collider_type_map_.end() ? it->second : ObjectType::kOther;
    };

    ObjectType typeA = get_type(pair.colliderA);
    ObjectType typeB = get_type(pair.colliderB);
    if ((typeA == ObjectType::kPlayer && typeB == ObjectType::kCoin) ||
        (typeB == ObjectType::kPlayer && typeA == ObjectType::kCoin))
    {
        const int coin_id = (typeA == ObjectType::kCoin) ? pair.colliderA.id : pair.colliderB.id;
        const int player_id = (typeA == ObjectType::kPlayer) ? pair.colliderA.id : pair.colliderB.id;

        if (player_manager_ && coin_manager_)
        {
            // Only process if the coin is active
            const auto& coins = coin_manager_->GetCoins();
            for (int i = 0; i < coins.size(); ++i)
            {
                if (coins[i].collider_.id == coin_id && coins[i].active_)
                {
                    // Add score to player
                    for (int p = 0; p < kMaxPlayers; ++p)
                    {
                        if (player_manager_->GetPlayer(p).collider_.id == player_id)
                        {
                            const int current_score = player_manager_->GetScore(p);
                            if (current_score + 100 <= 999)
                            {
                                player_manager_->AddScore(p, 100);
                            }
                            else
                            {
                                player_manager_->AddScore(p, 999 - current_score);
                            }
                            break;
                        }
                    }

                    // Deactivate coin
                    coin_manager_->DeactivateCoinAt(i);
                    break;
                }
            }
        }
    }
}

} // spacebagarre
