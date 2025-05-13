#include <engine/game_engine.h>

    crackitos_core::timer::Timer timer;
int main(int argc, char** argv)
{
    timer.Tick();
    spacebagarre::RunOfflineGame();
    return 0;
}
