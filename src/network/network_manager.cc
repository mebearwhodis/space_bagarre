#include "network/network_manager.h"

#include <iostream>

namespace spacebagarre
{

static const ExitGames::Common::JString appID = L"05290f39-d7da-48d9-81a0-3f877594f26b"; // set your app id here
static const ExitGames::Common::JString appVersion = L"1.0";


static std::unique_ptr<ExitGames::LoadBalancing::Client> loadBalancingClient_;


void NetworkManager::Begin(ClientInterface* client, const ExitGames::LoadBalancing::ClientConstructOptions& clientConstructOptions)
{
	loadBalancingClient_ = std::make_unique<ExitGames::LoadBalancing::Client>(*client, appID, appVersion, clientConstructOptions);
	ExitGames::LoadBalancing::ConnectOptions connectOptions_{};
	if(!loadBalancingClient_->connect(connectOptions_))
	{
		std::cerr << "Could not connect. \n";
	}
}

void NetworkManager::Tick()
{
	loadBalancingClient_->service();
}

void NetworkManager::End()
{
	loadBalancingClient_->disconnect();
}
ExitGames::LoadBalancing::Client& GetLoadBalancingClient()
{
	return *loadBalancingClient_;
}
} // spacebagarre