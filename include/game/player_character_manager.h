#ifndef SPACEBAGARRE_GAME_PLAYER_CHARACTER_MANAGER_H_
#define SPACEBAGARRE_GAME_PLAYER_CHARACTER_MANAGER_H_

#include <array>

#include "game/player_character.h"
#include "physics_world.h"

namespace spacebagarre
{

    constexpr int kMaxPlayers = 2;
    constexpr fp kMoveSpeed = 100.0f; // Left/right speed
    constexpr fp kMaxHorizontalSpeed = 200.0f; // horizontal clamp
    constexpr fp kJumpImpulse = 300.0f; // Upward thrust strength
    constexpr fp kJumpBufferTime = 0.1f; // 100ms window for buffered jump
    constexpr fp kCoyoteTime = 0.1f; // 100ms coyote forgiveness

    class PlayerCharacterManager
    {
    public:
        PlayerCharacterManager() = default;

        void RegisterWorld(crackitos_physics::physics::PhysicsWorld* world);
        void InitPlayers();
        void Update();
        void Deinit();

        void SetPlayerInput(int player_id, const PlayerInput& input);

        // Save and Load for rollback
        [[nodiscard]] std::array<PlayerCharacterState, kMaxPlayers> SavePlayerState() const;
        void LoadPlayerState(const std::array<PlayerCharacterState, kMaxPlayers>& states);


        PlayerCharacter& GetPlayer(int index);
        [[nodiscard]] const PlayerCharacter& GetPlayer(int index) const;

        void CopyFrom(const PlayerCharacterManager& other);


    private:
        std::array<PlayerCharacter, kMaxPlayers> players_{};
        crackitos_physics::physics::PhysicsWorld* world_ = nullptr;

        struct PlayerContactListener : public crackitos_physics::physics::ContactListener
        {
            PlayerCharacterManager* manager = nullptr;

            explicit PlayerContactListener(PlayerCharacterManager* mgr) : manager(mgr) {}

            void OnCollisionEnter(const crackitos_physics::physics::ColliderPair& pair) override
            {
                manager->HandleCollision(pair);
            }

            void OnCollisionStay(const crackitos_physics::physics::ColliderPair&) override {}
            void OnCollisionExit(const crackitos_physics::physics::ColliderPair&) override {}

            void OnTriggerEnter(const crackitos_physics::physics::ColliderPair&) override {}
            void OnTriggerStay(const crackitos_physics::physics::ColliderPair&) override {}
            void OnTriggerExit(const crackitos_physics::physics::ColliderPair&) override {}
        };

        void HandleCollision(const crackitos_physics::physics::ColliderPair& pair);

        PlayerContactListener contact_listener_{this};
    };

} // spacebagarre

#endif // SPACEBAGARRE_GAME_PLAYER_CHARACTER_MANAGER_H_
