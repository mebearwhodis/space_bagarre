#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <vector>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>

#include "vec2.h"
#include "commons/commons.h"

namespace idealpotato
{
    constexpr FrameNumber kMaxFrameCount = 60 * 60 * 3; // 3 mins at 60fps
    constexpr int kMaxInputs = 30;

    struct PlayerInput
    {
        int move_x_ = 0;
        int move_y_ = 0;
        bool jump_ = false;
        bool grab_ = false;

        bool operator==(const PlayerInput& rhs) const noexcept
        {
            return move_x_ == rhs.move_x_
                && move_y_ == rhs.move_y_
                && jump_ == rhs.jump_
                && grab_ == rhs.grab_;
        }

        bool operator!=(const PlayerInput& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        void Clear() noexcept
        {
            move_x_ = 0;
            move_y_ = 0;
            jump_ = false;
            grab_ = false;
        }
    };


    static constexpr fp kDeadZone = 8000.0f;

    void StartInputManager();
    void StopInputManager();
    void UpdateInputManager(const SDL_Event& event);

    PlayerInput GetPlayerInput(int player_id = 0);

    // const std::vector<PlayerInput>& GetSavedInputs();
    // std::vector<PlayerInput> GetRecentInputs(FrameNumber from, FrameNumber to);

    // void SavePlayerInput();
    // void ClearSavedInputs();

    bool IsPlayerConnected(int player_id);
} // idealpotato

#endif //INPUT_MANAGER_H
