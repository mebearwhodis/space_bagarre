#ifndef SPACEBAGARRE_NETWORK_NETWORK_CLIENT_H_
#define SPACEBAGARRE_NETWORK_NETWORK_CLIENT_H_

#include <game/rollback_manager.h>

#include "network_manager.h"
#include "packet.h"

namespace spacebagarre
{
    enum class State
    {
        kUnconnected,
        kConnecting,
        kConnected,
        kJoining,
        kJoined,
        kPlaying,
        kError
    };

    void StartNetwork();

    class NetworkClient : public ClientInterface
    {
    private:
        RollbackManager* rollback_manager_{};
        int player_number_ = -1;
        State state_ = State::kUnconnected;
    public:
        void SendStartGameSignal();
        void SendPingPacket(uint8_t timer);
        void SendInputPacket(const InputPacket& pkt);
        void SendConfirmFramePacket(const ConfirmFramePacket& pkt);
        void SendDesyncPacket(const DesyncPacket& pkt);

        void Update();
        void RegisterRollbackManager(RollbackManager* rollback_manager) { rollback_manager_ = rollback_manager; }
        int GetLocalPlayerID() const { return player_number_; }
        int GetRemotePlayerID() const { return player_number_ == 1 ? 0 : 1; }
        int GetOnlinePlayerNumber() const { return player_number_; }
        State GetState() const { return state_; }
        void SetState(State state) { state_ = state; }
        int GetPlayerCount() const;


        void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) override;

        void debugReturn(int debugLevel, const ExitGames::Common::JString& string) override;
        void connectionErrorReturn(int errorCode) override;
        void clientErrorReturn(int errorCode) override;
        void warningReturn(int warningCode) override;
        void serverErrorReturn(int errorCode) override;
        void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs,
                                 const ExitGames::LoadBalancing::Player& player) override;
        void leaveRoomEventAction(int playerNr, bool isInactive) override;
        void connectReturn(int errorCode, const ExitGames::Common::JString& errorString,
                           const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster) override;
        void disconnectReturn(void) override;
        void onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString>& regions,
                                const ExitGames::Common::JVector<ExitGames::Common::JString>& regionsServers) override;
        void leaveRoomReturn(int, const ExitGames::Common::JString&) override;
    };

} // spacebagarre

#endif // SPACEBAGARRE_NETWORK_NETWORK_CLIENT_H_
