#ifndef SPACEBAGARRE_GAME_PLAYER_CHARACTER_H_
#define SPACEBAGARRE_GAME_PLAYER_CHARACTER_H_
#include <game/input_manager.h>

#include "body.h"
#include "collider.h"
#include "physics_world.h"

namespace spacebagarre
{
    struct PlayerCharacterState
    {
        PlayerInput input_{};
        bool jump_button_pressed_ = false;
        fp respawn_timer_ = 0.0f;
        fp shockwave_cooldown_ = 0.0f;

        crackitos_core::math::Vec2f position_{};
        crackitos_core::math::Vec2f velocity_{};
    };

    //Separate character data from character physics data and actions
    struct PlayerCharacter
    {
        // --- Physics handles
        crackitos_physics::physics::BodyHandle body_;
        crackitos_physics::physics::ColliderHandle collider_;

        // --- Basic player state
        int player_index_ = 0;
        PlayerInput input_{};

        // --- Jump
        bool jump_button_pressed_ = false;

        // --- Timers
        fp respawn_timer_ = 0.0f;
        fp shockwave_cooldown_ = 0.0f;

        // --- Helpers
        [[nodiscard]] bool IsRespawning() const { return respawn_timer_ > 0.0f; }
        [[nodiscard]] bool CanUseShockwave() const { return shockwave_cooldown_ <= 0.0f; }

        [[nodiscard]] PlayerCharacterState SaveState(const crackitos_physics::physics::PhysicsWorld& world) const;
        void LoadState(const PlayerCharacterState& state, crackitos_physics::physics::PhysicsWorld& world);
    };
} // spacebagarre

#endif //SPACEBAGARRE_GAME_PLAYER_CHARACTER_H_

