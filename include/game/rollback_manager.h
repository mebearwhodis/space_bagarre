#ifndef ROLLBACK_MANAGER_H
#define ROLLBACK_MANAGER_H
#include <array>
#include <vector>

#include "commons/commons.h"
#include "game/input_manager.h"
#include "game/player_character_manager.h"

namespace idealpotato
{
    class RollbackManager
    {
    public:
        void RegisterPlayerManager(PlayerCharacterManager* player_manager) noexcept;
        void RegisterPhysicsWorld(crackitos_physics::physics::PhysicsWorld* world) noexcept;

        void Deinit() noexcept;

        void SetLocalInput(int player_id, const PlayerInput& input) noexcept;
        void SetRemoteInputs(const std::vector<PlayerInput>& new_inputs, int player_id);

        void SimulateUntilCurrentFrame() noexcept;
        void ConfirmFrame() noexcept;
        void SaveFirstConfirmedFrame() noexcept;

        [[nodiscard]] const PlayerInput& GetLastInput(int player_id) const noexcept;
        std::vector<PlayerInput> GetRecentInputs(int player_id, FrameNumber from, FrameNumber to) const;
        [[nodiscard]] FrameNumber GetCurrentFrame() const noexcept { return current_frame_; }
        void IncreaseCurrentFrame() noexcept { current_frame_++; }
        [[nodiscard]] FrameNumber GetConfirmedFrame() const noexcept { return confirmed_frame_; }
        [[nodiscard]] FrameNumber GetLastRemoteInputFrame() const noexcept { return last_remote_input_frame_; }
        [[nodiscard]] FrameNumber GetFrameToConfirm() const noexcept { return frame_to_confirm_; }


    private:
        PlayerCharacterManager* current_player_manager_ = nullptr;
        crackitos_physics::physics::PhysicsWorld* physics_world_ = nullptr;

        std::array<std::vector<PlayerInput>, kMaxPlayers> inputs_{};
        std::array<PlayerInput, kMaxPlayers> last_inputs_{};



        FrameNumber current_frame_ = 0;
        FrameNumber confirmed_frame_ = -1;
        FrameNumber last_remote_input_frame_ = -1;
        FrameNumber frame_to_confirm_ = 0;

        std::array<crackitos_physics::physics::PhysicsWorld, kMaxFrameCount> saved_physics_states_;
        std::array<std::array<PlayerCharacterState, kMaxPlayers>, kMaxFrameCount> saved_player_states_;
    };
} // namespace idealpotato
#endif
