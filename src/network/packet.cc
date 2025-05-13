#include <network/packet.h>

namespace spacebagarre
{
    bool PingSerializer::compare(const ExitGames::Common::CustomTypeBase& other) const
    {
        return ping_packet_.timer == static_cast<const PingSerializer&>(other).ping_packet_.timer;
    }

    void PingSerializer::duplicate(ExitGames::Common::CustomTypeBase* target) const
    {
        *reinterpret_cast<PingSerializer*>(target) = *this;
    }

    void PingSerializer::deserialize(const nByte* data, short length)
    {
        if (length >= 1)
            ping_packet_.timer = data[0];
    }

    short PingSerializer::serialize(nByte* data) const
    {
        if (data)
            data[0] = ping_packet_.timer;
        return 1;
    }

    ExitGames::Common::JString& PingSerializer::toString(ExitGames::Common::JString& retStr, bool) const
    {
        return retStr = L"PingPacket";
    }

    bool InputSerializer::compare(const CustomTypeBase& other) const
    {
        return input_packet_ == static_cast<const InputSerializer&>(other).input_packet_;
    }


    void InputSerializer::duplicate(CustomTypeBase* target) const
    {
        *reinterpret_cast<InputSerializer*>(target) = *this;
    }

    void InputSerializer::deserialize(const nByte* data, short length)
    {
        if (length < header_size_) return;

        std::memcpy(&input_packet_.player_and_frame, data, sizeof(uint16_t));
        input_packet_.input_size = data[2];

        const short expectedLen = header_size_ + input_packet_.input_size * 4;
        if (length < expectedLen) return;

        for (uint8_t i = 0; i < input_packet_.input_size; ++i)
        {
            PlayerInput input;
            input.move_x_ = static_cast<int8_t>(data[header_size_ + i * 4 + 0]);
            input.move_y_ = static_cast<int8_t>(data[header_size_ + i * 4 + 1]);
            input.jump_ = data[header_size_ + i * 4 + 2];
            input.grab_ = data[header_size_ + i * 4 + 3];
            input_packet_.inputs_table[i] = input;
        }
    }


    short InputSerializer::serialize(nByte* data) const
    {
        if (data)
        {
            std::memcpy(data, &input_packet_.player_and_frame, sizeof(uint16_t));
            data[2] = input_packet_.input_size;

            for (uint8_t i = 0; i < input_packet_.input_size; ++i)
            {
                const PlayerInput& input = input_packet_.inputs_table[i];
                data[header_size_ + i * 4 + 0] = static_cast<int8_t>(input.move_x_);
                data[header_size_ + i * 4 + 1] = static_cast<int8_t>(input.move_y_);
                data[header_size_ + i * 4 + 2] = static_cast<uint8_t>(input.jump_);
                data[header_size_ + i * 4 + 3] = static_cast<uint8_t>(input.grab_);
                //TODO sizeof playerinput etc.
            }
        }
        return static_cast<short>(header_size_ + input_packet_.input_size * 4);
    }

    ExitGames::Common::JString& InputSerializer::toString(ExitGames::Common::JString& retStr, bool) const
    {
        return retStr = L"InputPacket";
    }

    bool ConfirmFrameSerializer::compare(const ExitGames::Common::CustomTypeBase& other) const
    {
        const auto& otherPacket = static_cast<const ConfirmFrameSerializer&>(other).confirm_frame_packet_;
        return confirm_frame_packet_.confirm_frame == otherPacket.confirm_frame &&
            confirm_frame_packet_.confirm_value == otherPacket.confirm_value &&
            confirm_frame_packet_.input == otherPacket.input;
    }

    void ConfirmFrameSerializer::duplicate(ExitGames::Common::CustomTypeBase* target) const
    {
        *reinterpret_cast<ConfirmFrameSerializer*>(target) = *this;
    }

    void ConfirmFrameSerializer::deserialize(const nByte* data, short length)
    {
        if (length < 6 + kMaxPlayers * 4) return;

        std::memcpy(&confirm_frame_packet_.confirm_frame, data, sizeof(uint16_t));
        std::memcpy(&confirm_frame_packet_.confirm_value, data + 2, sizeof(uint32_t));

        for (int i = 0; i < kMaxPlayers; ++i)
        {
            const int base = 6 + i * 4;
            PlayerInput input;
            input.move_x_ = static_cast<int8_t>(data[base + 0]);
            input.move_y_ = static_cast<int8_t>(data[base + 1]);
            input.jump_ = data[base + 2];
            input.grab_ = data[base + 3];
            confirm_frame_packet_.input[i] = input;
        }
    }


    short ConfirmFrameSerializer::serialize(nByte* data) const
    {
        if (data)
        {
            std::memcpy(data, &confirm_frame_packet_.confirm_frame, sizeof(uint16_t));
            std::memcpy(data + 2, &confirm_frame_packet_.confirm_value, sizeof(uint32_t));
            for (int i = 0; i < kMaxPlayers; ++i)
            {
                const PlayerInput& input = confirm_frame_packet_.input[i];
                const int base = 6 + i * 4;
                data[base + 0] = static_cast<int8_t>(input.move_x_);
                data[base + 1] = static_cast<int8_t>(input.move_y_);
                data[base + 2] = static_cast<uint8_t>(input.jump_);
                data[base + 3] = static_cast<uint8_t>(input.grab_);
            }
        }
        return static_cast<short>(6 + kMaxPlayers * 4);
    }


    ExitGames::Common::JString& ConfirmFrameSerializer::toString(ExitGames::Common::JString& retStr, bool) const
    {
        return retStr = L"ConfirmFramePacket";
    }

    bool DesyncSerializer::compare(const ExitGames::Common::CustomTypeBase& other) const
    {
        return desync_packet_.message == static_cast<const DesyncSerializer&>(other).desync_packet_.message;
    }

    void DesyncSerializer::duplicate(ExitGames::Common::CustomTypeBase* target) const
    {
        *reinterpret_cast<DesyncSerializer*>(target) = *this;
    }

    void DesyncSerializer::deserialize(const nByte* data, short length)
    {
        desync_packet_.message = std::string(reinterpret_cast<const char*>(data), length);
    }

    short DesyncSerializer::serialize(nByte* data) const
    {
        std::memcpy(data, desync_packet_.message.data(), desync_packet_.message.size());
        return static_cast<short>(desync_packet_.message.size());
    }

    ExitGames::Common::JString& DesyncSerializer::toString(ExitGames::Common::JString& retStr, bool) const
    {
        return retStr = L"DesyncPacket";
    }
} // spacebagarre
