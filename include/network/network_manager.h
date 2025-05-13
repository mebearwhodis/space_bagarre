
#ifndef SPACEBAGARRE_NETWORK_NETWORK_MANAGER_H_
#define SPACEBAGARRE_NETWORK_NETWORK_MANAGER_H_

#include <LoadBalancing-cpp/inc/Client.h>

namespace spacebagarre
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

} // spacebagarre



#endif //SPACEBAGARRE_NETWORK_NETWORK_MANAGER_H_