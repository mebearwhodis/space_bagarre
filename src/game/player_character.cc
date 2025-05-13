
#include "game/player_character.h"

namespace spacebagarre {
    PlayerCharacterState PlayerCharacter::SaveState(const crackitos_physics::physics::PhysicsWorld& world) const
    {
        PlayerCharacterState state;
        state.input_ = input_;
        state.is_grounded_ = is_grounded_;
        state.is_jumping_ = is_jumping_;
        state.jump_buffer_timer_ = jump_buffer_timer_;
        state.coyote_timer_ = coyote_timer_;

        const auto& body = world.GetBody(body_);
        state.position_ = body.position();
        state.velocity_ = body.velocity();

        return state;
    }

    void PlayerCharacter::LoadState(const PlayerCharacterState& state, crackitos_physics::physics::PhysicsWorld& world)
    {
        input_ = state.input_;
        is_grounded_ = state.is_grounded_;
        is_jumping_ = state.is_jumping_;
        jump_buffer_timer_ = state.jump_buffer_timer_;
        coyote_timer_ = state.coyote_timer_;

        auto& body = world.GetMutableBody(body_);
        body.set_position(state.position_);
        body.set_velocity(state.velocity_);
    }

} // spacebagarre