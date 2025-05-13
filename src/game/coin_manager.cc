#include "game/coin_manager.h"

#include <game/game_contact_listener.h>

#include "shape.h"

namespace spacebagarre
{
    void CoinManager::Init(crackitos_physics::physics::PhysicsWorld* world, GameContactListener* listener)
    {
        world_ = world;

        for (const auto& pos : kCoinPositions)
        {
            Coin coin;

            crackitos_physics::physics::Body body_def(
                crackitos_physics::physics::BodyType::Static,
                pos,
                {0, 0},
                false, 0.0f
            );
            coin.body_ = world->CreateBody(body_def);

            crackitos_core::math::Circle shape(pos, 18.0f);
            crackitos_physics::physics::Collider collider_def(
                shape,
                0.0f,
                0.0f,
                true,
                coin.body_
            );

            const auto collider_handle = world->CreateCollider(coin.body_, collider_def);
            coin.collider_ = collider_handle;
            if (listener)
            {
                listener->RegisterToListenerMap(ObjectType::kCoin, collider_handle);
            }
            coin.active_ = false;

            coins_.push_back(coin);
        }
    }

    void CoinManager::ActivateCoinAt(int index)
    {
        if (index >= 0 && index < coins_.size())
            coins_[index].Activate();
    }

    void CoinManager::DeactivateCoinAt(int index)
    {
        if (index >= 0 && index < coins_.size())
            coins_[index].Deactivate();
    }

    void CoinManager::DeactivateAll()
    {
        for (auto& coin : coins_)
            coin.Deactivate();
    }

    void CoinManager::Update()
    {
        if (coins_.empty()) return;

        coin_timer_ -= 1.0f / 60.0f;
        if (coin_timer_ <= 0.0f)
        {
            // Deactivate current
            DeactivateCoinAt(current_active_index_);

            // Advance to next
            current_active_index_ = (current_active_index_ + 1) % coins_.size();
            ActivateCoinAt(current_active_index_);
            coin_timer_ = coin_cycle_time_;
        }
    }

}
