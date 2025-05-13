#include "game/player_character_manager.h"

namespace spacebagarre
{
    void PlayerCharacterManager::RegisterWorld(crackitos_physics::physics::PhysicsWorld* world)
    {
        world_ = world;
    }

    void PlayerCharacterManager::InitPlayers()
    {
        for (int i = 0; i < kMaxPlayers; ++i)
        {
            crackitos_core::math::Vec2f start_position;
            if (i == 0)
                //TODO tableau en constexpr
                start_position = {500.0f, 345.0f}; // Player 1 start
            else
                start_position = {1000.0f, 345.0f}; // Player 2 start

            // Create Body
            crackitos_physics::physics::Body body_def(
                crackitos_physics::physics::BodyType::Dynamic,
                start_position,
                crackitos_core::math::Vec2f::Zero(),
                true,
                1.0f // mass
            );
            players_[i].body_ = world_->CreateBody(body_def);

            // Create Collider
            crackitos_core::math::AABB aabb(
                crackitos_core::math::Vec2f(start_position.x - 8.0f, start_position.y - 16.0f),
                crackitos_core::math::Vec2f(start_position.x + 8.0f, start_position.y + 16.0f)
            );

            crackitos_physics::physics::Collider collider_def(
                aabb,
                0.0f, // friction
                0.0f, // bounciness
                false, // not a trigger
                players_[i].body_
            );

            players_[i].collider_ = world_->CreateCollider(players_[i].body_, collider_def);

            players_[i].player_index_ = i;
        }
    }

    void PlayerCharacterManager::Update()
    {
        for (auto& player : players_)
        {
            crackitos_physics::physics::Body& body = world_->GetMutableBody(player.body_);

            //Timers
            if (player.respawn_timer_ > 0.0f)
            {
                player.respawn_timer_ -= 1.0f / 60.0f;

                // Freeze position + ignore input
                body.set_velocity(crackitos_core::math::Vec2f::Zero());
                continue; // skip rest of update
            }

            if (player.shockwave_cooldown_ > 0.0f)
            {
                player.shockwave_cooldown_ -= 1.0f / 60.0f;
            }


            // Apply left/right movement only
            crackitos_core::math::Vec2f horizontal_velocity{player.input_.move_x_ * kMoveSpeed, 0.0f}; //Y is locked
            body.ApplyForce(horizontal_velocity);

            // Cap horizontal velocity
            crackitos_core::math::Vec2f velocity = body.velocity();
            if (velocity.x > kMaxHorizontalSpeed)
                velocity.x = kMaxHorizontalSpeed;
            else if (velocity.x < -kMaxHorizontalSpeed)
                velocity.x = -kMaxHorizontalSpeed;
            // Don't touch vertical velocity
            body.set_velocity(velocity);

            // Flap mechanic: apply upward impulse when jump is pressed
            if (player.input_.jump_ && !player.jump_button_pressed_)
            {
                body.ApplyImpulse(crackitos_core::math::Vec2f(0.0f, -kJumpImpulse));
                player.jump_button_pressed_ = true; // prevent holding jump
            }

            // Reset jump_button_pressed_ when jump is released
            if (!player.input_.jump_)
            {
                player.jump_button_pressed_ = false;
            }

            // --- Shockwave ---
            if (player.input_.shockwave_ && player.CanUseShockwave())
            {
                // TODO shockwave logic
                player.shockwave_cooldown_ = 6.0f;
            }
        }
    }

    void PlayerCharacterManager::Deinit()
    {
        players_.fill(PlayerCharacter{});
    }

    void PlayerCharacterManager::SetPlayerInput(int player_id, const PlayerInput& input)
    {
        if (player_id >= 0 && player_id < kMaxPlayers)
        {
            players_[player_id].input_ = input;
        }
    }

    std::array<PlayerCharacterState, kMaxPlayers> PlayerCharacterManager::SavePlayerState() const
    {
        std::array<PlayerCharacterState, kMaxPlayers> states{};
        for (int i = 0; i < kMaxPlayers; ++i)
        {
            states[i] = players_[i].SaveState(*world_);
        }
        return states;
    }

    void PlayerCharacterManager::LoadPlayerState(const std::array<PlayerCharacterState, kMaxPlayers>& states)
    {
        for (int i = 0; i < kMaxPlayers; ++i)
        {
            players_[i].LoadState(states[i], *world_);
        }
    }


    PlayerCharacter& PlayerCharacterManager::GetPlayer(int index)
    {
        return players_.at(index);
    }

    const PlayerCharacter& PlayerCharacterManager::GetPlayer(int index) const
    {
        return players_.at(index);
    }

    void PlayerCharacterManager::CopyFrom(const PlayerCharacterManager& other)
    {
        for (int i = 0; i < kMaxPlayers; ++i)
        {
            players_[i].player_index_ = other.players_[i].player_index_;
            players_[i].body_ = other.players_[i].body_;
            players_[i].collider_ = other.players_[i].collider_;
            players_[i].input_ = other.players_[i].input_;
            players_[i].respawn_timer_ = other.players_[i].respawn_timer_;
            players_[i].shockwave_cooldown_ = other.players_[i].shockwave_cooldown_;
        }
    }

} // spacebagarre
