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

        if (typeA == ObjectType::kPlayer && typeB == ObjectType::kCoin)
        {
            std::cout << "[Collision] Player picked up a coin (player: " << pair.colliderA.id << ", coin: " << pair.colliderB.id << ")\n";
            if (player_manager_)
            {
                for (int i = 0; i < kMaxPlayers; ++i)
                {
                    if (player_manager_->GetPlayer(i).collider_.id == pair.colliderA.id)
                    {
                        player_manager_->AddScore(i, 100);
                        break;
                    }
                }
            }
        }
        else if (typeB == ObjectType::kPlayer && typeA == ObjectType::kCoin)
        {
            std::cout << "[Collision] Player picked up a coin (player: " << pair.colliderB.id << ", coin: " << pair.colliderA.id << ")\n";
            if (player_manager_)
            {
                for (int i = 0; i < kMaxPlayers; ++i)
                {
                    if (player_manager_->GetPlayer(i).collider_.id == pair.colliderB.id)
                    {
                        player_manager_->AddScore(i, 100);
                        break;
                    }
                }
            }
        }

        else if (typeA == ObjectType::kPlayer && typeB == ObjectType::kWall)
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
                    }
                }
            }
        }
        else if (typeA == ObjectType::kPlayer && typeB == ObjectType::kPlayer)
        {
            std::cout << "[Collision] Player vs Player\n";
        }
    }
} // spacebagarre
