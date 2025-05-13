#include "game/rollback_manager.h"

#include <iostream>

namespace idealpotato
{
    void RollbackManager::RegisterPlayerManager(PlayerCharacterManager* player_manager) noexcept
    {
        current_player_manager_ = player_manager;

        for (int i = 0; i < kMaxPlayers; ++i)
        {
            inputs_[i].clear();
            inputs_[i].resize(kMaxFrameCount);
            last_inputs_[i].Clear();
        }

        confirmed_frame_ = -1;
        current_frame_ = 0;
        last_remote_input_frame_ = -1;
        frame_to_confirm_ = 0;
    }


    void RollbackManager::RegisterPhysicsWorld(crackitos_physics::physics::PhysicsWorld* world) noexcept
    {
        physics_world_ = world;
    }


    void RollbackManager::Deinit() noexcept
    {
        current_frame_ = 0;
        confirmed_frame_ = -1;
        last_remote_input_frame_ = -1;
        frame_to_confirm_ = 0;

        for (auto& input_list : inputs_)
        {
            input_list.clear();
        }

        last_inputs_.fill(PlayerInput{});
    }

    void RollbackManager::SetLocalInput(int player_id, const PlayerInput& input) noexcept {
        inputs_[player_id][current_frame_] = input;
        last_inputs_[player_id] = input;
    }


    void RollbackManager::SetRemoteInputs(const std::vector<PlayerInput>& new_inputs, int player_id) {
        if (new_inputs.empty()) return;

        FrameNumber first_new_frame = std::max(0, current_frame_ - static_cast<FrameNumber>(new_inputs.size()));
        bool must_rollback = false;

        for (size_t i = 0; i < new_inputs.size(); ++i) {
            FrameNumber frame = first_new_frame + i;
            const PlayerInput& new_input = new_inputs[i];

            // Check mismatch to decide rollback
            if (inputs_[player_id][frame] != new_input) {
                must_rollback = true;
            }

            size_t kResizeBuffer = 20;
            if (inputs_[player_id].size() <= frame) {
                inputs_[player_id].resize(frame + kResizeBuffer);
            }
            inputs_[player_id][frame] = new_input;


        }

        last_remote_input_frame_ = current_frame_ - 1;
        last_inputs_[player_id] = new_inputs.back();

        if (must_rollback) {
            SimulateUntilCurrentFrame();
        }
    }



    void RollbackManager::SimulateUntilCurrentFrame() noexcept {
        // Step 1: Load confirmed saved state
        physics_world_->CopyFrom(saved_physics_states_[std::max(confirmed_frame_, 0)]);
        current_player_manager_->LoadPlayerState(saved_player_states_[std::max(confirmed_frame_, 0)]);


        // Step 2: Re-simulate all frames
        for (FrameNumber frame = confirmed_frame_ + 1; frame < current_frame_; ++frame) {
            for (int player_id = 0; player_id < kMaxPlayers; ++player_id) {
                current_player_manager_->SetPlayerInput(player_id, inputs_[player_id][frame]);
            }

            physics_world_->StepSimulation();
        }
    }


    void RollbackManager::ConfirmFrame() noexcept {
        for (int player_id = 0; player_id < kMaxPlayers; player_id++) {
            current_player_manager_->SetPlayerInput(player_id, inputs_[player_id][frame_to_confirm_]);
        }

        // Step 1: Update simulation to confirm frame
        physics_world_->StepSimulation();

        // Step 2: Save state
        // Step 2: Save states
        saved_physics_states_[frame_to_confirm_].CopyFrom(*physics_world_);
        saved_player_states_[frame_to_confirm_] = current_player_manager_->SavePlayerState();


        // Step 3: Advance tracking
        confirmed_frame_++;
        frame_to_confirm_++;
    }


    void RollbackManager::SaveFirstConfirmedFrame() noexcept
    {
        saved_physics_states_[0].CopyFrom(*physics_world_);
        saved_player_states_[0] = current_player_manager_->SavePlayerState();

    }


    const PlayerInput& RollbackManager::GetLastInput(int player_id) const noexcept
    {
        return last_inputs_[player_id];
    }

    std::vector<PlayerInput> RollbackManager::GetRecentInputs(int player_id, FrameNumber from, FrameNumber to) const
    {
        std::vector<PlayerInput> result;
        for (FrameNumber f = from + 1; f <= to && f < kMaxFrameCount; ++f)
        {
            result.push_back(inputs_[player_id][f]);
        }
        return result;
    }

} // namespace idealpotato
