#ifndef SPACEBAGARRE_GAME_COIN_MANAGER_H_
#define SPACEBAGARRE_GAME_COIN_MANAGER_H_

#include <vector>
#include <commons/commons.h>

#include "physics_world.h"
#include "vec2.h"

namespace spacebagarre
{
    class GameContactListener;

    struct Coin
    {
        crackitos_physics::physics::BodyHandle body_;
        crackitos_physics::physics::ColliderHandle collider_;
        bool active_ = false;

        void Activate(){ active_ = true; };
        void Deactivate(){ active_ = false; };
    };

    class CoinManager
    {
    public:
        void Init(crackitos_physics::physics::PhysicsWorld* world, GameContactListener* listener);
        void ActivateCoinAt(int index);
        void DeactivateCoinAt(int index);
        void DeactivateAll();
        void Update();
        const std::vector<Coin>& GetCoins() const { return coins_; }

    private:
        crackitos_physics::physics::PhysicsWorld* world_ = nullptr;
        std::vector<Coin> coins_;
        int current_active_index_ = 0;
        fp coin_timer_ = 0.0f;
        fp coin_cycle_time_ = 9.0f;

    };

    static constexpr std::array<crackitos_core::math::Vec2f, 20> kCoinPositions{
        {
            {960.0f, 409.0f},
            {1634.0f, 579.0f},
            {242.0f, 1030.0f},
            {1709.0f, 848.0f},
            {1347.0f, 453.0f},
            {707.0f, 622.0f},
            {1816.0f, 217.0f},
            {1602.0f, 180.0f},
            {1457.0f, 967.0f},
            {1076.0f, 89.0f},
            {933.0f, 866.0f},
            {71.0f, 728.0f},
            {1256.0f, 653.0f},
            {337.0f, 169.0f},
            {1013.0f, 578.0f},
            {558.0f, 332.0f},
            {1672.0f, 1004.0f},
            {1291.0f, 961.0f},
            {456.0f, 274.0f},
            {646.0f, 980.0f}
        }
    };
}

#endif //SPACEBAGARRE_GAME_COIN_MANAGER_H_
