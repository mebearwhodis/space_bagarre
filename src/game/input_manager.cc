#include "game/input_manager.h"

#include <iostream>

namespace idealpotato
{
    namespace
    {
        SDL_Gamepad* gamepad_player_one_ = nullptr;
        SDL_Gamepad* gamepad_player_two_ = nullptr;

        // std::vector<PlayerInput> player_inputs_;
        // std::string input_logs_path_;
    } // namespace

    void StartInputManager()
    {
        // player_inputs_.clear();
        // player_inputs_.reserve(kMaxFrameCount); //10 minutes at 60fps


        // Load controller mappings from file
        if (SDL_AddGamepadMappingsFromFile("data/gamecontrollerdb.txt") < 0)
        {
            std::cerr << "Failed to load controller mappings: " << SDL_GetError() << "\n";
        }
    }

    void StopInputManager()
    {
        if (gamepad_player_one_)
        {
            SDL_CloseGamepad(gamepad_player_one_);
            gamepad_player_one_ = nullptr;
        }
        if (gamepad_player_two_)
        {
            SDL_CloseGamepad(gamepad_player_two_);
            gamepad_player_two_ = nullptr;
        }
    }


    void UpdateInputManager(const SDL_Event& event)
    {
        if (event.type == SDL_EVENT_GAMEPAD_ADDED)
        {
            SDL_Gamepad* new_gamepad = SDL_OpenGamepad(event.gdevice.which);
            if (!new_gamepad)
            {
                std::cerr << "Failed to open gamepad: " << SDL_GetError() << "\n";
                return;
            }

            if (!gamepad_player_one_)
            {
                gamepad_player_one_ = new_gamepad;
                std::cout << "Assigned to Player 1\n";
            }
            else if (!gamepad_player_two_)
            {
                gamepad_player_two_ = new_gamepad;
                std::cout << "Assigned to Player 2\n";
            }
            else
            {
                SDL_CloseGamepad(new_gamepad);
                std::cout << "Ignored extra gamepad\n";
            }
        }

        if (event.type == SDL_EVENT_GAMEPAD_REMOVED)
        {
            SDL_JoystickID removed_id = event.gdevice.which;

            if (gamepad_player_one_ && SDL_GetGamepadID(gamepad_player_one_) == removed_id)
            {
                SDL_CloseGamepad(gamepad_player_one_);
                gamepad_player_one_ = nullptr;
                std::cout << "Player 1 gamepad disconnected\n";
            }
            else if (gamepad_player_two_ && SDL_GetGamepadID(gamepad_player_two_) == removed_id)
            {
                SDL_CloseGamepad(gamepad_player_two_);
                gamepad_player_two_ = nullptr;
                std::cout << "Player 2 gamepad disconnected\n";
            }
        }
    }

    PlayerInput GetPlayerInput(int player_id)
    {
        PlayerInput input{};

        SDL_Gamepad* gamepad = nullptr;
        if (player_id == 0)
            gamepad = gamepad_player_one_;
        else if (player_id == 1)
            gamepad = gamepad_player_two_;

        if (gamepad)
        {
            // Left stick axis
            const Sint16 axis_x = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
            const Sint16 axis_y = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);

            input.move_x_ = (static_cast<fp>(std::abs(axis_x)) > kDeadZone) ? static_cast<fp>(axis_x) / 32767.0f : 0.0f;
            input.move_y_ = (static_cast<fp>(std::abs(axis_y)) > kDeadZone) ? static_cast<fp>(axis_y) / 32768.0f : 0.0f;

            // Face buttons
            input.jump_ =
                SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_EAST) ||
                SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH);
            input.grab_ =
                SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_WEST) ||
                SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_NORTH);
        }
        else
        {
            // If no gamepad, fallback to keyboard
            const bool* keys = SDL_GetKeyboardState(nullptr);

            if (player_id == 0) // player one default keys
            {
                if (keys[SDL_SCANCODE_D]) input.move_x_ = 1.0f;
                if (keys[SDL_SCANCODE_A]) input.move_x_ = -1.0f;

                input.jump_ = keys[SDL_SCANCODE_SPACE] || keys[SDL_SCANCODE_W];
                input.grab_ = keys[SDL_SCANCODE_LSHIFT];
            }
            else if (player_id == 1) // player two keys (optional)
            {
                if (keys[SDL_SCANCODE_RIGHT]) input.move_x_ = 1.0f;
                if (keys[SDL_SCANCODE_LEFT]) input.move_x_ = -1.0f;

                input.jump_ = keys[SDL_SCANCODE_UP];
                input.grab_ = keys[SDL_SCANCODE_RSHIFT];
            }
        }

        return input;
    }

    // void SavePlayerInput()
    // {
    //     player_inputs_.push_back(GetPlayerInput(0));
    // }
    //
    // const std::vector<PlayerInput>& GetSavedInputs()
    // {
    //     return player_inputs_;
    // }

    // std::vector<PlayerInput> GetRecentInputs(FrameNumber from, FrameNumber to)
    // {
    //     std::vector<PlayerInput> recent;
    //     for (FrameNumber i = from + 1; i <= to && i < static_cast<FrameNumber>(player_inputs_.size()); ++i)
    //     {
    //         recent.push_back(player_inputs_[i]);
    //     }
    //     return recent;
    // }


    // void ClearSavedInputs()
    // {
    //     player_inputs_.clear();
    // }

    bool IsPlayerConnected(int player_id)
    {
        if (player_id == 0)
            return gamepad_player_one_ != nullptr;
        else if (player_id == 1)
            return gamepad_player_two_ != nullptr;

        return false;
    }
} // idealpotato
