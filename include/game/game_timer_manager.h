#ifndef SPACEBAGARRE_GAME_GAME_TIMER_MANAGER_H_
#define SPACEBAGARRE_GAME_GAME_TIMER_MANAGER_H_

#include "input_manager.h"
#include "commons/commons.h"
#include "timer.h"

namespace spacebagarre
{
    enum class GamePhase
    {
        WaitingToStart,
        Playing,
        GameOver
    };

    class GameTimerManager
    {
    public:
        void StartGame();  // call when transitioning from menu to game
        void Tick();       // call every frame
        GamePhase GetPhase() const { return current_phase_; }
        fp TimeRemaining() const;
        fp ElapsedTime() const;

        bool IsFrozen() const; // true if WaitingToStart or GameOver

    private:
        crackitos_core::timer::Timer timer_;
        GamePhase current_phase_ = GamePhase::WaitingToStart;
        fp start_delay_ = 5.0f;
        fp max_play_time_ = (kMaxFrameCount / 60.0f) - 10.0f;
    };
}
// spacebagarre

#endif //SPACEBAGARRE_GAME_GAME_TIMER_MANAGER_H_
