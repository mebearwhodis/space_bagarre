#include "game/player_character_manager.h"

namespace idealpotato
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

            // Apply left/right movement only
            crackitos_core::math::Vec2f velocity{player.input_.move_x_ * kMoveSpeed, 0.0f}; //Y is locked
            body.ApplyForce(velocity);
            //TODO cap velocity (horizontally but also vertically, when falling)

            // Jump timers
            if (player.jump_buffer_timer_ > 0.0f)
            {
                player.jump_buffer_timer_ -= 1.0f / 60.0f;
            }
            if (player.coyote_timer_ > 0.0f)
            {
                player.coyote_timer_ -= 1.0f / 60.0f;
            }

            // Jump input
            if (player.input_.jump_)
            {
                player.jump_buffer_timer_ = kJumpBufferTime;
            }

            // Update grounded state
            if (std::abs(body.velocity().y) <= 0.01f)
            {
                player.is_grounded_ = true;
                player.is_jumping_ = false;
            }
            else
            {
                player.is_grounded_ = false;
            }

            // Coyote time
            if (player.is_grounded_)
            {
                player.coyote_timer_ = kCoyoteTime; // refresh coyote timer while grounded
            }

            // Attempt jump
            if (player.is_grounded_ && player.jump_buffer_timer_ > 0.0f && player.coyote_timer_ >= 0.0f)
            {
                body.ApplyImpulse(crackitos_core::math::Vec2f(0.0f, -kJumpImpulse));
                player.is_grounded_ = false; // immediately mark airborne
                player.is_jumping_ = true;
                player.jump_buffer_timer_ = 0.0f;
                player.coyote_timer_ = 0.0f;
            }

            // VARIABLE JUMP HEIGHT:
            if (player.is_jumping_)
            {
                // If jump button is released early and player is rising
                if (!player.input_.jump_ && body.velocity().y < 0.0f)
                {
                    // Cut upward velocity to zero
                    crackitos_core::math::Vec2f new_velocity = body.velocity();
                    new_velocity.y *= 0.5f; // cut vertical speed in half for softer fall
                    body.set_velocity(new_velocity);

                    player.is_jumping_ = false;
                }
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
            players_[i].jump_buffer_timer_ = other.players_[i].jump_buffer_timer_;
            players_[i].coyote_timer_ = other.players_[i].coyote_timer_;
            players_[i].is_grounded_ = other.players_[i].is_grounded_;
            players_[i].is_jumping_ = other.players_[i].is_jumping_;
        }
    }

} // idealpotato
