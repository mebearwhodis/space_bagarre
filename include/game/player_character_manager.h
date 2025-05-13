#ifndef SPACEBAGARRE_GAME_PLAYER_CHARACTER_MANAGER_H_
#define SPACEBAGARRE_GAME_PLAYER_CHARACTER_MANAGER_H_

#include <array>

#include "game/player_character.h"
#include <game/game_contact_listener.h>
#include "physics_world.h"

namespace spacebagarre
{
    static constexpr int kMaxPlayers = 2;
    static constexpr fp kMoveSpeed = 100.0f; // Left/right speed
    static constexpr fp kMaxHorizontalSpeed = 200.0f; // horizontal clamp
    static constexpr fp kJumpImpulse = 300.0f; // Upward thrust strength
    static constexpr fp kJumpBufferTime = 0.1f; // 100ms window for buffered jump
    static constexpr fp kCoyoteTime = 0.1f; // 100ms coyote forgiveness
    static constexpr std::array<crackitos_core::math::Vec2f, kMaxPlayers> kPlayerPositions = {
        {
            {640.0f, 360.0f},
            {1280.0f, 360.0f}
        }
    };

    class PlayerCharacterManager
    {
    public:
        PlayerCharacterManager() = default;

        void RegisterWorld(crackitos_physics::physics::PhysicsWorld* world, GameContactListener* listener);
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

        PlayerCharacter& GetMutablePlayer(int index);
        crackitos_physics::physics::Body& GetMutableBody(int index);
        crackitos_core::math::Vec2f GetPlayerStartPosition(int index) const;

        int GetScore(int player_index) const;
        void AddScore(int player_index, int amount);

    private:
        std::array<PlayerCharacter, kMaxPlayers> players_{};
        std::array<int, kMaxPlayers> scores_{};
        crackitos_physics::physics::PhysicsWorld* world_ = nullptr;
        GameContactListener* contact_listener_ = nullptr;
    };
} // spacebagarre

#endif // SPACEBAGARRE_GAME_PLAYER_CHARACTER_MANAGER_H_
