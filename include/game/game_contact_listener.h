#ifndef SPACEBAGARRE_GAME_GAME_CONTACT_LISTENER_H_
#define SPACEBAGARRE_GAME_GAME_CONTACT_LISTENER_H_

#include "contact_listener.h"
#include <unordered_map>
#include <iostream>

#include "coin_manager.h"

namespace spacebagarre
{
    enum class ObjectType
    {
        kPlayer,
        kWall,
        kCoin,
        kOther
    };

    class PlayerCharacterManager;

    class GameContactListener : public crackitos_physics::physics::ContactListener
    {
    private:
        std::unordered_map<int, ObjectType> collider_type_map_;
        PlayerCharacterManager* player_manager_ = nullptr;
        CoinManager* coin_manager_ = nullptr;

    public:
        void SetPlayerManager(PlayerCharacterManager* manager) { player_manager_ = manager; }
        void SetCoinManager(CoinManager* manager) { coin_manager_ = manager; }
        void RegisterToListenerMap(ObjectType type, const crackitos_physics::physics::ColliderHandle& handle);

        void OnCollisionEnter(const crackitos_physics::physics::ColliderPair& pair) override;

        void OnCollisionStay(const crackitos_physics::physics::ColliderPair&) override
        {
        }

        void OnCollisionExit(const crackitos_physics::physics::ColliderPair&) override
        {
        }

        void OnTriggerEnter(const crackitos_physics::physics::ColliderPair&) override;

        void OnTriggerStay(const crackitos_physics::physics::ColliderPair&) override
        {
        }

        void OnTriggerExit(const crackitos_physics::physics::ColliderPair&) override
        {
        }
    };

    GameContactListener& GetGameContactListener();
} // spacebagarre

#endif //SPACEBAGARRE_GAME_GAME_CONTACT_LISTENER_H_
