#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <engine/window.h>
#include <graphics/renderer.h>


namespace idealpotato
{
    void StartImGui()
    {
        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForSDLRenderer(GetWindow(), GetRenderer());
        ImGui_ImplSDLRenderer3_Init(GetRenderer());
    }

    void StopImGui()
    {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }
}
