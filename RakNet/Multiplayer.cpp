#include "Multiplayer.h"
#include <iostream>
#include <string>

namespace Network
{

	Multiplayer * Network::Multiplayer::GetInstance()
	{
		static Multiplayer m_instance;
		return &m_instance;
	}

	void Multiplayer::StartUp()
	{
		RakNet::StartupResult result;
		if (m_isServer)
		{
			result = this->pPeer->Startup(MAX_CONNECTIONS, &RakNet::SocketDescriptor(SERVER_PORT, 0), 1);
			this->pPeer->SetMaximumIncomingConnections(MAX_CONNECTIONS);
		}
		if (result != RakNet::StartupResult::RAKNET_STARTED)
		{
			m_isServer = false;
			this->pPeer->Startup(1, &RakNet::SocketDescriptor(), 1);
			this->ConnectToLocalhost();
		}
	}

	void Multiplayer::ConnectToLocalhost()
	{
		if (!m_isServer)
			this->pPeer->Connect("localhost", SERVER_PORT, 0, 0);
	}

	void Multiplayer::ReadPackets()
	{
		static int packetsCounter = 0;
		RakNet::Packet * packet;
		
		for (packet = pPeer->Receive(); packet; pPeer->DeallocatePacket(packet), packet = pPeer->Receive()) 
		{
			packetsCounter++;
			std::cout << "--------------------------NEW PACKET--------------------------\n";
			std::cout << "System Adress: "; std::cout << packet->systemAddress.ToString() << std::endl;
			std::cout << "RakNet GUID: "; std::cout << packet->guid.ToString() << std::endl;
			std::cout << "Lenght of Data in Bytes: " + std::to_string(packet->length) << std::endl;
			std::cout << "Message Identifier: " + std::to_string(packet->data[0]) << std::endl;
			std::cout << "\nDATA:\n"; std::cout << std::string((char*)packet->data, packet->length);
			std::cout << "\n\nReceived Packets Amount: " + std::to_string(packetsCounter) << std::endl;
			
		}
	}

	void Multiplayer::SendPacket(const char * message)
	{
		this->pPeer->Send(message,
			std::strlen(message) + 1,
			LOW_PRIORITY, RELIABLE,
			0,
			RakNet::UNASSIGNED_RAKNET_GUID,
			true);
	}


	Multiplayer::Multiplayer()
	{
		if (this->pPeer == 0)
			this->pPeer = RakNet::RakPeerInterface::GetInstance();

		std::cout << "Multiplayer Constructor is called.\n";
	}


	Multiplayer::~Multiplayer()
	{
		if (this->pPeer)
			RakNet::RakPeerInterface::DestroyInstance(this->pPeer);

		std::cout << "Multiplayer Destructor is called.\n";
	}




}