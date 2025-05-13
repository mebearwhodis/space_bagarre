
#include "game/player_character.h"

namespace spacebagarre {
    PlayerCharacterState PlayerCharacter::SaveState(const crackitos_physics::physics::PhysicsWorld& world) const
    {
        PlayerCharacterState state;
        state.input_ = input_;
        state.jump_button_pressed_ = jump_button_pressed_;
        state.respawn_timer_ = respawn_timer_;
        state.shockwave_cooldown_ = shockwave_cooldown_;

        const auto& body = world.GetBody(body_);
        state.position_ = body.position();
        state.velocity_ = body.velocity();

        return state;
    }

    void PlayerCharacter::LoadState(const PlayerCharacterState& state, crackitos_physics::physics::PhysicsWorld& world)
    {
        input_ = state.input_;
        jump_button_pressed_ = state.jump_button_pressed_;
        respawn_timer_ = state.respawn_timer_;
        shockwave_cooldown_ = state.shockwave_cooldown_;

        auto& body = world.GetMutableBody(body_);
        body.set_position(state.position_);
        body.set_velocity(state.velocity_);
    }

} // spacebagarre