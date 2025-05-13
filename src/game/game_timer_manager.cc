#include "game/game_timer_manager.h"

namespace spacebagarre
{
    void GameTimerManager::StartGame()
    {
        timer_ = crackitos_core::timer::Timer(); // reset timer
        current_phase_ = GamePhase::WaitingToStart;
    }

    void GameTimerManager::Tick()
    {
        timer_.Tick();

        fp t = timer_.TotalTime();

        if (current_phase_ == GamePhase::WaitingToStart && t >= start_delay_)
        {
            current_phase_ = GamePhase::Playing;
        }
        else if (current_phase_ == GamePhase::Playing && t >= (start_delay_ + max_play_time_))
        {
            current_phase_ = GamePhase::GameOver;
        }
    }

    fp GameTimerManager::TimeRemaining() const
    {
        if (current_phase_ == GamePhase::Playing)
            return std::max(0.0f, start_delay_ + max_play_time_ - timer_.TotalTime());
        return 0.0f;
    }

    fp GameTimerManager::ElapsedTime() const
    {
        return timer_.TotalTime();
    }

    bool GameTimerManager::IsFrozen() const
    {
        return current_phase_ != GamePhase::Playing;
    }
}
// spacebagarre