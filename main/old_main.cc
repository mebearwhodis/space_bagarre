#include "deprec_include/old_game.h"

int main(int argc, char** argv) {
    idealpotato::game::Game game;
    if (!game.Init()) return 1;
    game.Run();
    return 0;
}
