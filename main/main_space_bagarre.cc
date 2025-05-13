#include "engine/game_engine.h"
#include "network/network_client.h"

#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <view/imgui_menu.h>

using namespace spacebagarre;

int main(int argc, char* argv[])
{
    RunEngine();
    return 0;
}
