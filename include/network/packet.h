#ifndef SPACEBAGARRE_NETWORK_PACKET_H_
#define SPACEBAGARRE_NETWORK_PACKET_H_

#include <array>
#include <sstream>
#include <string>
#include <vector>
#include <game/input_manager.h>
#include <game/player_character_manager.h>

#include "Common-cpp/inc/CustomType.h"


namespace spacebagarre
{
    enum class PacketType : nByte
    {
        kStartGame = 1,
        kPing,
        kInput,
        kConfirmFrame,
        kDesync
    };

    //--------------------------- Start Game ------------------------------
    //No need to implement it since it's just a signal with no payload,
    // TODO will need to use something like this:
    // nByte eventCode = static_cast<nByte>(PacketType::kStartGame);
    // LoadBalancingClient::opRaiseEvent(eventCode, nullptr, 0, true);


    //--------------------------- Ping ------------------------------
    struct PingPacket
    {
        uint8_t timer = 0;

        bool operator==(const PingPacket& other) const
        {
            return timer == other.timer;
        }
    };

    class PingSerializer final : public ExitGames::Common::CustomType<
            PingSerializer, static_cast<nByte>(PacketType::kPing)>
    {
    public:
        PingSerializer() = default;
        explicit PingSerializer(const PingPacket& packet) : ping_packet_(packet){}
        bool compare(const ExitGames::Common::CustomTypeBase& other) const override;
        void duplicate(ExitGames::Common::CustomTypeBase* target) const override;
        void deserialize(const nByte* data, short length) override;
        short serialize(nByte* data) const override;
        ExitGames::Common::JString& toString(ExitGames::Common::JString& retStr, bool) const override;
        const PingPacket& GetPacket() const { return ping_packet_; }

    private:
        PingPacket ping_packet_{};
    };

    //--------------------------- Input ------------------------------
    struct InputPacket
    {
        uint16_t player_and_frame = 0; // 1 bit for player number, 15 bits for frame
        uint8_t input_size = 0;
        std::array<PlayerInput, kMaxInputs> inputs_table{}; // from confirmedFrame + 1 to current


        // Setter
        void SetPlayerAndFrame(bool playerNumber, uint16_t frame)
        {
            player_and_frame = (static_cast<uint16_t>(playerNumber) << 15) | (frame & 0x7FFF);
        }

        // Getters
        bool GetPlayerNumber() const
        {
            return (player_and_frame >> 15) & 0x1;
        }

        uint16_t GetFrame() const
        {
            return player_and_frame & 0x7FFF;
        }

        bool operator==(const InputPacket& other) const
        {
            bool result = player_and_frame == other.player_and_frame && input_size == other.input_size;
            if (!result)
                return false;
            for (int i = 0; i < input_size; i++)
            {
                if (inputs_table[i] != other.inputs_table[i])
                    return false;
            }
            return true;
        }
    };

    class InputSerializer final : public ExitGames::Common::CustomType<
            InputSerializer, static_cast<nByte>(PacketType::kInput)>
    {
    public:
        InputSerializer() = default;

        explicit InputSerializer(const InputPacket& packet) : input_packet_(packet)
        {
        }

        bool compare(const ExitGames::Common::CustomTypeBase& other) const override;
        void duplicate(ExitGames::Common::CustomTypeBase* target) const override;
        void deserialize(const nByte* data, short length) override;
        short serialize(nByte* data) const override;
        ExitGames::Common::JString& toString(ExitGames::Common::JString& retStr, bool) const override;
        const InputPacket& GetPacket() const { return input_packet_; }

    private:
        InputPacket input_packet_{};
        int header_size_ = 3;
    };

    //--------------------------- Confirm Frame ------------------------------
    struct ConfirmFramePacket
    {
        uint16_t confirm_frame = 0;
        uint32_t confirm_value = 0;
        std::array<PlayerInput, kMaxPlayers> input{};

        bool operator==(const ConfirmFramePacket& other) const
        {
            return confirm_frame == other.confirm_frame && confirm_value == other.confirm_value && input == other.input;
        }
    };

    class ConfirmFrameSerializer final : public ExitGames::Common::CustomType<
            ConfirmFrameSerializer, static_cast<nByte>(PacketType::kConfirmFrame)>
    {
    public:
        ConfirmFrameSerializer() = default;

        explicit ConfirmFrameSerializer(const ConfirmFramePacket& packet) : confirm_frame_packet_(packet)
        {
        }

        bool compare(const ExitGames::Common::CustomTypeBase& other) const override;
        void duplicate(ExitGames::Common::CustomTypeBase* target) const override;
        void deserialize(const nByte* data, short length) override;
        short serialize(nByte* data) const override;
        ExitGames::Common::JString& toString(ExitGames::Common::JString& retStr, bool) const override;
        const ConfirmFramePacket& GetPacket() const { return confirm_frame_packet_; }

    private:
        ConfirmFramePacket confirm_frame_packet_{};
    };

    //--------------------------- Desync ------------------------------
    struct DesyncPacket
    {
        std::string message;
    };

    class DesyncSerializer final : public ExitGames::Common::CustomType<
            DesyncSerializer, static_cast<nByte>(PacketType::kDesync)>
    {
    public:
        DesyncSerializer() = default;
        explicit DesyncSerializer(const DesyncPacket& packet) : desync_packet_(packet){}
        bool compare(const ExitGames::Common::CustomTypeBase& other) const override;
        void duplicate(ExitGames::Common::CustomTypeBase* target) const override;
        void deserialize(const nByte* data, short length) override;
        short serialize(nByte* data) const override;
        ExitGames::Common::JString& toString(ExitGames::Common::JString& retStr, bool) const override;
        const DesyncPacket& GetPacket() const { return desync_packet_; }

    private:
        DesyncPacket desync_packet_;
    };
} // spacebagarre

#endif // SPACEBAGARRE_NETWORK_PACKET_H_
