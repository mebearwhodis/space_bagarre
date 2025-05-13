#include "engine/game_engine.h"

#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <iostream>
#include <ostream>
#include <game/input_manager.h>
#include <engine/window.h>
#include <game/coin_manager.h>
#include <game/game_contact_listener.h>
#include <game/game_timer_manager.h>
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
#include "physics_world.h"
#include "timer.h"
#include "vec2.h"

#include "game/player_character.h"

namespace spacebagarre
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
        GameContactListener contact_listener_;
        NetworkClient network_client_;
        GameTimerManager game_timer_;
        CoinManager coin_manager_;

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

        // 1. Window & Renderer
        StartWindow();
        StartRenderer();
        game_window_ = GetWindow();
        game_renderer_ = GetRenderer();

        // 2. ImGui (UI Layer)
        StartImGui();
        imgui_enabled_ = true;

        // 3. Game resources
        StartResourceManager();

        // 4. Physics setup
        contact_listener_.SetPlayerManager(&player_character_manager_);
        contact_listener_.SetCoinManager(&coin_manager_);
        physics_world_.SetContactListener(&contact_listener_);

        crackitos_core::math::AABB world_bounds({0, 0}, {kWindowWidth, kWindowHeight});
        physics_world_.Initialize(world_bounds, false, {0, 500.0f});

        // 5. Load level geometry
        CreateLevel(&physics_world_, &contact_listener_);

        // 6. Input system
        StartInputManager();

        // 7. Player, coin & rollback systems
        rollback_manager_.RegisterPlayerManager(&player_character_manager_);
        rollback_manager_.RegisterPhysicsWorld(&physics_world_);
        coin_manager_.Init(&physics_world_, &contact_listener_);
        player_character_manager_.RegisterWorld(&physics_world_, &contact_listener_);
        player_character_manager_.InitPlayers();

        // 8. Initial rollback save
        rollback_manager_.SaveFirstConfirmedFrame();
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
        game_timer_.StartGame();
        coin_manager_.DeactivateAll();
        coin_manager_.ActivateCoinAt(0);


        while (IsWindowOpen())
        {
            timer.Tick();
            game_timer_.Tick();

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
                if (!game_timer_.IsFrozen())
                {
                    player_character_manager_.SetPlayerInput(0, GetPlayerInput(0));
                    player_character_manager_.SetPlayerInput(1, GetPlayerInput(1));
                    physics_world_.Update();
                    player_character_manager_.Update();
                    coin_manager_.Update();
                }
            }

            SDL_SetRenderDrawColor(game_renderer_, 0, 0, 0, 255);
            SDL_RenderClear(game_renderer_);
            RenderBackground();
            RenderActiveCoins(coin_manager_);
            RenderPlayers(player_character_manager_, physics_world_);

            // --- Overlay: Get Ready / Game Over ---
            if (game_timer_.GetPhase() == GamePhase::WaitingToStart)
            {
                // draw get ready image here (or placeholder)
                RenderGetReadyOverlay();
            }
            else if (game_timer_.GetPhase() == GamePhase::GameOver)
            {
                RenderGameOverOverlay(player_character_manager_);

                // Display scores
                int score0 = player_character_manager_.GetScore(0);
                int score1 = player_character_manager_.GetScore(1);
            }

            SDL_RenderPresent(game_renderer_);
        }
    }

    void RunOnlineGame()
    {
        crackitos_core::timer::Timer timer;
        timer.SetFixedDeltaTime(1.0f / 60.0f);
        game_timer_.StartGame();
        coin_manager_.DeactivateAll();
        coin_manager_.ActivateCoinAt(0);


        while (IsWindowOpen())
        {
            timer.Tick();
            game_timer_.Tick();

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

                if (!game_timer_.IsFrozen())
                {
                    PlayerInput local_input = GetPlayerInput(0);
                    rollback_manager_.SetLocalInput(local_player_id, local_input);
                    FrameNumber confirmed_frame = rollback_manager_.GetConfirmedFrame();
                    FrameNumber current_frame = rollback_manager_.GetCurrentFrame();
                    auto recent_inputs = rollback_manager_.GetRecentInputs(
                        local_player_id, confirmed_frame, current_frame);

                    InputPacket pkt;
                    pkt.SetPlayerAndFrame(local_player_id, current_frame);
                    pkt.input_size = static_cast<uint8_t>(recent_inputs.size());
                    std::copy(recent_inputs.begin(), recent_inputs.end(), pkt.inputs_table.begin());

                    network_client_.SendInputPacket(pkt);

                    rollback_manager_.SimulateUntilCurrentFrame();


                    for (int player_id = 0; player_id < kMaxPlayers; ++player_id)
                    {
                        player_character_manager_.SetPlayerInput(player_id, rollback_manager_.GetLastInput(player_id));
                    }

                    physics_world_.StepSimulation();
                    player_character_manager_.Update();
                    coin_manager_.Update();
                }

                rollback_manager_.ConfirmFrame();
                rollback_manager_.IncreaseCurrentFrame();
            }

            SDL_SetRenderDrawColor(game_renderer_, 0, 0, 0, 255);
            SDL_RenderClear(game_renderer_);
            RenderBackground();
            RenderActiveCoins(coin_manager_);
            RenderPlayers(player_character_manager_, physics_world_);

            if (game_timer_.GetPhase() == GamePhase::WaitingToStart)
            {
                RenderGetReadyOverlay();
            }
            else if (game_timer_.GetPhase() == GamePhase::GameOver)
            {
                RenderGameOverOverlay(player_character_manager_);
            }

            SDL_RenderPresent(game_renderer_);
        }
    }

    void DisplayOnlineLobby()
    {
        network_client_.Update();

        int player_count = network_client_.GetPlayerCount();
        int local_id = network_client_.GetLocalPlayerID();

        ImGui::Begin("Room Lobby", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
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
} // spacebagarre
