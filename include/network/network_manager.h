
#ifndef IDEALPOTATO_NETWORK_MANAGER_H
#define IDEALPOTATO_NETWORK_MANAGER_H

#include <LoadBalancing-cpp/inc/Client.h>

namespace idealpotato
{

class ClientInterface : public ExitGames::LoadBalancing::Listener
{
};

namespace NetworkManager
{
void Begin(ClientInterface* client, const ExitGames::LoadBalancing::ClientConstructOptions& clientConstructOptions={});
void Tick();
void End();
};
ExitGames::LoadBalancing::Client& GetLoadBalancingClient();

} // namespace idealpotato



#endif //IDEALPOTATO_NETWORK_MANAGER_H