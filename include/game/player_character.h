#ifndef PLAYER_CHARACTER_H
#define PLAYER_CHARACTER_H
#include <game/input_manager.h>

#include "body.h"
#include "collider.h"
#include "physics_world.h"

namespace idealpotato
{
    struct PlayerCharacterState
    {
        PlayerInput input_{};
        bool is_grounded_ = false;
        bool is_jumping_ = false;
        fp jump_buffer_timer_ = 0.0f;
        fp coyote_timer_ = 0.0f;

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

        // --- Input snapshot for this frame
        PlayerInput input_{};

        // --- Jump
        bool is_grounded_ = false;
        bool jump_button_pressed_ = false;
        bool is_jumping_ = false;

        fp jump_buffer_timer_ = 0.0f;
        fp coyote_timer_ = 0.0f;

        [[nodiscard]] PlayerCharacterState SaveState(const crackitos_physics::physics::PhysicsWorld& world) const;
        void LoadState(const PlayerCharacterState& state, crackitos_physics::physics::PhysicsWorld& world);
    };
} // idealpotato

#endif //PLAYER_CHARACTER_H
