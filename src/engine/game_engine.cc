#include "engine/game_engine.h"

#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <iostream>
#include <ostream>
#include <game/input_manager.h>
#include <engine/window.h>
#include <game/level.h>
#include <game/player_character_manager.h>
#include <game/rollback_manager.h>
#include <graphics/renderer.h>
#include <graphics/resource_manager.h>
#include <network/network_client.h>
#include <SDL3/SDL_init.h>
#include <view/imgui_menu.h>

#include "commons/commons.h"

#include "body.h"
#include "collider.h"
#include "physics_world.h"
#include "timer.h"
#include "vec2.h"

#include "game/player_character.h"

namespace idealpotato
{
    namespace
    {
        enum class GameState
        {
            MainMenu,
            Lobby,
            OfflineGame,
            OnlineGame
        };

        GameState current_state_ = GameState::MainMenu;

        PlayerCharacterManager player_character_manager_;
        RollbackManager rollback_manager_;
        crackitos_physics::physics::PhysicsWorld physics_world_;
        NetworkClient network_client_;

        SDL_Window* game_window_;
        SDL_Renderer* game_renderer_;

        bool imgui_enabled_ = false;
    }

    void StartEngine()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
        {
            std::cerr << "Failed to initialize SDL: " << SDL_GetError() << "\n";
        }

        StartWindow();
        StartRenderer();
        StartImGui();
        imgui_enabled_ = true;

        StartResourceManager();

        game_window_ = GetWindow();
        game_renderer_ = GetRenderer();
    }

    void StopEngine()
    {
        StopInputManager();
        StopResourceManager();
        StopRenderer();
        StopWindow();
        SDL_Quit();
    }

    void RunOfflineGame()
    {
        crackitos_core::timer::Timer timer;
        timer.SetFixedDeltaTime(1.0f / 60.0f);
        while (IsWindowOpen())
        {
            timer.Tick();

            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                UpdateInputManager(event);
                if (event.type == SDL_EVENT_QUIT ||
                    (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE))
                {
                    CloseWindow();
                }
            }

            while (timer.FixedDeltaTimeStep())
            {
                player_character_manager_.SetPlayerInput(0, GetPlayerInput(0));
                player_character_manager_.SetPlayerInput(1, GetPlayerInput(1));
                physics_world_.Update();
                player_character_manager_.Update();
                // SavePlayerInput();
            }

            SDL_SetRenderDrawColor(game_renderer_, 0, 0, 0, 255);
            SDL_RenderClear(game_renderer_);
            RenderBackground();

            SDL_SetRenderDrawColor(game_renderer_, 255, 0, 255, 255);

            SDL_FRect rect_one;
            rect_one.x = physics_world_.GetMutableBody(player_character_manager_.GetPlayer(0).body_).position().x - 8;
            rect_one.y = physics_world_.GetMutableBody(player_character_manager_.GetPlayer(0).body_).position().y - 16;
            rect_one.w = 16;
            rect_one.h = 32;
            SDL_RenderFillRect(game_renderer_, &rect_one);

            SDL_SetRenderDrawColor(game_renderer_, 0, 255, 255, 255);
            SDL_FRect rect_two;
            rect_two.x = physics_world_.GetMutableBody(player_character_manager_.GetPlayer(1).body_).position().x - 8;
            rect_two.y = physics_world_.GetMutableBody(player_character_manager_.GetPlayer(1).body_).position().y - 16;
            rect_two.w = 16;
            rect_two.h = 32;
            SDL_RenderFillRect(game_renderer_, &rect_two);

            SDL_RenderPresent(game_renderer_);
        }
    }

    void RunOnlineGame()
    {
        crackitos_core::timer::Timer timer;
        timer.SetFixedDeltaTime(1.0f / 60.0f);
        while (IsWindowOpen())
        {
            timer.Tick();
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                UpdateInputManager(event);
                if (event.type == SDL_EVENT_QUIT ||
                    (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE))
                {
                    CloseWindow();
                }
            }

            network_client_.Update();

            while (timer.FixedDeltaTimeStep())
            {
                const int local_player_id = network_client_.GetOnlinePlayerNumber();
                PlayerInput local_input = GetPlayerInput(0);
                rollback_manager_.SetLocalInput(local_player_id, local_input);
                // SavePlayerInput();

                FrameNumber confirmed_frame = rollback_manager_.GetConfirmedFrame();
                FrameNumber current_frame = rollback_manager_.GetCurrentFrame();
                auto recent_inputs = rollback_manager_.GetRecentInputs(local_player_id, confirmed_frame, current_frame);

                InputPacket pkt;
                pkt.SetPlayerAndFrame(local_player_id, current_frame);
                pkt.input_size = static_cast<uint8_t>(recent_inputs.size());

                std::copy(recent_inputs.begin(), recent_inputs.end(), pkt.inputs_table.begin());


                // bool has_non_zero_input = false;
                // for (const auto& input : recent_inputs)
                // {
                //     if (input.move_x_ != 0 || input.move_y_ != 0 || input.jump_ || input.grab_)
                //     {
                //         has_non_zero_input = true;
                //         break;
                //     }
                // }
                //
                // if (has_non_zero_input)
                // {
                //     std::cout << "[SendInput] P" << local_player_id << ": sending "
                //         << static_cast<int>(pkt.input_size) << " inputs from frame "
                //         << (confirmed_frame + 1) << " to " << current_frame << "\n";
                //
                //     for (const auto& input : recent_inputs)
                //     {
                //         std::cout << "  MoveX: " << input.move_x_
                //             << ", MoveY: " << input.move_y_
                //             << ", Jump: " << input.jump_
                //             << ", Grab: " << input.grab_ << "\n";
                //     }
                // }


                network_client_.SendInputPacket(pkt);

                rollback_manager_.SimulateUntilCurrentFrame();

                for (int player_id = 0; player_id < kMaxPlayers; ++player_id)
                {
                    player_character_manager_.SetPlayerInput(player_id, rollback_manager_.GetLastInput(player_id));
                }

                physics_world_.StepSimulation();
                player_character_manager_.Update();

                rollback_manager_.ConfirmFrame();
                rollback_manager_.IncreaseCurrentFrame();
            }

            SDL_SetRenderDrawColor(game_renderer_, 0, 0, 0, 255);
            SDL_RenderClear(game_renderer_);
            RenderBackground();

            SDL_SetRenderDrawColor(game_renderer_, 255, 0, 255, 255);

            SDL_FRect rect_one;
            rect_one.x = physics_world_.GetMutableBody(player_character_manager_.GetPlayer(0).body_).position().x - 8;
            rect_one.y = physics_world_.GetMutableBody(player_character_manager_.GetPlayer(0).body_).position().y - 16;
            rect_one.w = 16;
            rect_one.h = 32;
            SDL_RenderRect(game_renderer_, &rect_one);

            SDL_SetRenderDrawColor(game_renderer_, 0, 255, 255, 255);
            SDL_FRect rect_two;
            rect_two.x = physics_world_.GetMutableBody(player_character_manager_.GetPlayer(1).body_).position().x - 8;
            rect_two.y = physics_world_.GetMutableBody(player_character_manager_.GetPlayer(1).body_).position().y - 16;
            rect_two.w = 16;
            rect_two.h = 32;
            SDL_RenderRect(game_renderer_, &rect_two);

            SDL_RenderPresent(game_renderer_);
        }
    }

    void DisplayOnlineLobby()
    {
        network_client_.Update();

        int player_count = network_client_.GetPlayerCount();
        int local_id = network_client_.GetLocalPlayerID();

        ImGui::Begin("Room Lobby", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::Text("Players in room: %d/2", player_count);

        if (player_count >= 2 && local_id == 0)
        {
            if (ImGui::Button("Start Game"))
            {
                network_client_.SendStartGameSignal();
                network_client_.SetState(State::kPlaying);
            }
        }
        ImGui::End();

        if (network_client_.GetState() == State::kPlaying)
        {
            StopImGui();
            imgui_enabled_ = false;
            current_state_ = GameState::OnlineGame;
            RunOnlineGame();
        }
    }

    void RunEngine()
    {
        StartEngine();

        crackitos_core::math::AABB world_bounds({0, 0}, {kWindowWidth, kWindowHeight});
        physics_world_.Initialize(world_bounds, false, {0, 981.0f});
        CreateLevel(&physics_world_);

        StartInputManager();

        rollback_manager_.RegisterPlayerManager(&player_character_manager_);
        rollback_manager_.RegisterPhysicsWorld(&physics_world_);
        player_character_manager_.RegisterWorld(&physics_world_);
        player_character_manager_.InitPlayers();
        rollback_manager_.SaveFirstConfirmedFrame();
        
        while (IsWindowOpen())
        {
            if (imgui_enabled_)
            {
                ImGui_ImplSDLRenderer3_NewFrame();
                ImGui_ImplSDL3_NewFrame();
                ImGui::NewFrame();
            }


            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (imgui_enabled_)
                    ImGui_ImplSDL3_ProcessEvent(&event);

                UpdateInputManager(event);
                if (event.type == SDL_EVENT_QUIT ||
                    (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE))
                {
                    CloseWindow();
                }
            }

            switch (current_state_)
            {
            case GameState::MainMenu:
                ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

                if (ImGui::Button("Start Offline"))
                {
                    ImGui::End();
                    StopImGui();
                    imgui_enabled_ = false;
                    current_state_ = GameState::OfflineGame;
                    RunOfflineGame();
                    break;
                }

                if (ImGui::Button("Connect to Lobby"))
                {
                    ImGui::End();
                    StartNetwork();
                    network_client_.RegisterRollbackManager(&rollback_manager_);
                    NetworkManager::Begin(&network_client_);
                    current_state_ = GameState::Lobby;
                    break;
                }

                ImGui::End();
                break;


            case GameState::Lobby:
                DisplayOnlineLobby();
                break;

            default:
                break;
            }

            SDL_SetRenderDrawColor(game_renderer_, 10, 10, 10, 255);
            SDL_RenderClear(game_renderer_);
            RenderBackground();

            if (imgui_enabled_)
            {
                ImGui::Render();
                ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), game_renderer_);
            }

            SDL_RenderPresent(game_renderer_);
            SDL_Delay(16);
        }

        StopEngine();
    }
} // idealpotato
