#include "network/network_client.h"
#include <Common-cpp/inc/JString.h>
#include <iostream>

namespace idealpotato
{
    namespace
    {
        NetworkClient network_client;
    }

    void StartNetwork()
    {
        PingSerializer::registerType();
        InputSerializer::registerType();
        ConfirmFrameSerializer::registerType();
        DesyncSerializer::registerType();
        // NetworkManager::Begin(&network_client);
    }

    void SendPingPacket(const uint8_t timer)
    {
        PingPacket pkt{};
        pkt.timer = timer;

        PingSerializer serializer(pkt);
        GetLoadBalancingClient().opRaiseEvent(
            true, // reliable
            serializer,
            static_cast<nByte>(PacketType::kPing)
        );
    }


    void NetworkClient::SendStartGameSignal()
    {
        ExitGames::LoadBalancing::RaiseEventOptions options{};
        GetLoadBalancingClient().opRaiseEvent(
            true,
            static_cast<nByte>(0),
            static_cast<nByte>(PacketType::kStartGame),
            options
            );
    }

    void NetworkClient::SendInputPacket(const InputPacket& pkt)
    {
        // std::cout << "[Photon] Sending input packet: " << pkt.input_size << " inputs\n";
        InputSerializer serializer(pkt);
        GetLoadBalancingClient().opRaiseEvent(
            false,
            serializer,
            static_cast<nByte>(PacketType::kInput)
        );
    }

    void NetworkClient::SendConfirmFramePacket(const ConfirmFramePacket& pkt)
    {
        ConfirmFrameSerializer serializer(pkt);
        GetLoadBalancingClient().opRaiseEvent(
            true, // reliable
            serializer,
            static_cast<nByte>(PacketType::kConfirmFrame)
        );
    }

    void NetworkClient::SendDesyncPacket(const DesyncPacket& pkt)
    {
        DesyncSerializer serializer(pkt);
        GetLoadBalancingClient().opRaiseEvent(
            true, // reliable
            serializer,
            static_cast<nByte>(PacketType::kDesync)
        );
    }


    void NetworkClient::Update()
    {
        NetworkManager::Tick();
    }


    int NetworkClient::GetPlayerCount() const
    {
        return GetLoadBalancingClient().getCurrentlyJoinedRoom().getPlayerCount();
    }

    void NetworkClient::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent)
    {
        // if (eventContent.getType() != ExitGames::Common::TypeCode::CUSTOM)
        // {
        //     std::cerr << "Invalid event type: " << eventContent.getType() << "\n";
        //     return;
        // }

        switch (static_cast<PacketType>(eventCode))
        {
        case PacketType::kStartGame:
            {
                std::cout << "[StartGame] Game is starting!\n";
                state_ = State::kPlaying;
                break;
            }

        case PacketType::kPing:
            {
                /*
                 *TODO: ping
                 * -player sends an empty packet to master,
                 * -> master sends a packet that contains nothing or the current start timer if it’s during the timer
                 * -> divide RTT by 2 to get the ping
                 * -> (take the average of the last pings, not just the last one)
                 * -e.g.: pingAvg = 0.75f * pingAvg + 0.25f * ping
                 */
                if (player_number_ == 0)
                {
                    const auto& wrapper =
                        ExitGames::Common::ValueObject<PingSerializer>(eventContent).getDataCopy();
                    const PingPacket& pkt = wrapper.GetPacket();
                    std::cout << "[Ping] Received timer byte: " << static_cast<int>(pkt.timer) << " from player " <<
                        playerNr << "\n";
                }
                break;
            }

        case PacketType::kInput:
            {
                const auto& wrapper =
                    ExitGames::Common::ValueObject<InputSerializer>(eventContent).getDataCopy();

                const InputPacket& pkt = wrapper.GetPacket();

                const uint16_t frame = pkt.GetFrame();
                const uint8_t sender_id = pkt.GetPlayerNumber(); // Online player number from packet
                const uint8_t local_id = GetOnlinePlayerNumber(); // Your local online ID
                const uint8_t remote_id = 1 - local_id; // Opponent is always the other player

                if (sender_id == GetLocalPlayerID())
                {
                    std::cerr << "Received input from myself? Ignoring. ID: " << static_cast<int>(sender_id) << "\n";
                    return;
                }

                std::vector<PlayerInput> remote_inputs;
                for (uint8_t i = 0; i < pkt.input_size; ++i)
                {
                    remote_inputs.push_back(pkt.inputs_table[i]);
                }

                if (rollback_manager_)
                {
                    rollback_manager_->SetRemoteInputs(remote_inputs, /*player_id=*/remote_id);
                }

                // bool has_non_zero_input = false;
                // for (uint8_t i = 0; i < pkt.input_size; ++i)
                // {
                //     const auto& input = pkt.inputs_table[i];
                //     if (input.move_x_ != 0 || input.move_y_ != 0 || input.jump_ || input.grab_)
                //     {
                //         has_non_zero_input = true;
                //         break;
                //     }
                // }
                //
                // if (has_non_zero_input)
                // {
                //     std::cout << "Received inputs from player " << static_cast<int>(sender_id)
                //         << " for frame " << frame
                //         << " with " << static_cast<int>(pkt.input_size) << " input(s)\n";
                // }

                break;
            }

        case PacketType::kConfirmFrame:
            {
                const auto& wrapper =
                    ExitGames::Common::ValueObject<ConfirmFrameSerializer>(eventContent).getDataCopy();
                const ConfirmFramePacket& pkt = wrapper.GetPacket();

                // std::cout << "[ConfirmFrame] Frame: " << pkt.confirm_frame << ", Checksum: " << pkt.confirm_value <<
                //     "\n";
                // for (int i = 0; i < kMaxPlayers; ++i)
                // {
                //     const PlayerInput& input = pkt.input[i];
                //     std::cout << "  Player[" << i << "] Input: {"
                //         << " x=" << input.move_x_
                //         << ", y=" << input.move_y_
                //         << ", j=" << input.jump_
                //         << ", g=" << input.grab_
                //         << " }\n";
                // }

                break;
            }

        case PacketType::kDesync:
            {
                const auto& wrapper =
                    ExitGames::Common::ValueObject<DesyncSerializer>(eventContent).getDataCopy();
                const DesyncPacket& pkt = wrapper.GetPacket();

                std::cerr << "[Desync] Received desync message: " << pkt.message << "\n";

                break;
            }
        }
    }


    void NetworkClient::debugReturn(int debugLevel, const ExitGames::Common::JString& string)
    {
        std::cerr << "DebugReturn, level: " << debugLevel << " with message: " << string.UTF8Representation().cstr() <<
            "\n";
    }

    void NetworkClient::connectionErrorReturn(int errorCode)
    {
        std::cerr << "ConnectionError, code: " << errorCode << "\n";
    }

    void NetworkClient::clientErrorReturn(int errorCode)
    {
        std::cerr << "ClientError, code: " << errorCode << "\n";
    }

    void NetworkClient::warningReturn(int warningCode)
    {
        std::cerr << "Warning, code: " << warningCode << "\n";
    }

    void NetworkClient::serverErrorReturn(int errorCode)
    {
        std::cerr << "ServerError, code: " << errorCode << "\n";
    }

    void NetworkClient::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs,
                                            const ExitGames::LoadBalancing::Player&)
    {
        //TODO setup rooms
        // auto& room = client.getCurrentlyJoinedRoom();
        // room.setIsOpen(false);
        std::cout << "Player " << playerNr << " joined the room.\n";

        if (player_number_ == -1)
            player_number_ = playerNr - 1;

        std::cout << "[Network] Assigned online player number: " << player_number_ << "\n";

        const auto playerCount = GetLoadBalancingClient().getCurrentlyJoinedRoom().getPlayerCount();
        if (playerCount >= 2)
            state_ = State::kJoined;
    }


    void NetworkClient::leaveRoomEventAction(int, bool)
    {
    }

    void NetworkClient::connectReturn(int errorCode, const ExitGames::Common::JString&,
                                      const ExitGames::Common::JString&, const ExitGames::Common::JString&)
    {
        if (errorCode == 0) // success
        {
            std::cout << "Connected to Photon server. Joining or creating room...\n";
            ExitGames::LoadBalancing::RoomOptions roomOptions;
            GetLoadBalancingClient().opJoinOrCreateRoom(L"MyRoom", roomOptions);
        }
        else
        {
            std::cerr << "Failed to connect to Photon. Error code: " << errorCode << "\n";
        }
    }

    void NetworkClient::disconnectReturn()
    {
        std::cerr << "Disconnect\n";
    }

    void NetworkClient::onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString>&,
                                           const ExitGames::Common::JVector<ExitGames::Common::JString>&)
    {
    }

    void NetworkClient::leaveRoomReturn(int, const ExitGames::Common::JString&)
    {
    }
} // namespace idealpotato
