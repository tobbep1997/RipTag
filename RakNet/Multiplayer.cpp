#include "Multiplayer.h"
#include <iostream>
#include <string>


namespace Network
{
	std::map<std::string, std::function<void()>> Multiplayer::LocalPlayerOnSendMap;
	std::map<unsigned char, std::function<void(unsigned char, unsigned char *)>> Multiplayer::RemotePlayerOnReceiveMap;
	std::map<unsigned char, std::function<void(unsigned char, RakNet::Packet*)>> Multiplayer::LobbyOnReceiveMap;

	bool Multiplayer::inPlayState = false;

	Multiplayer * Network::Multiplayer::GetInstance()
	{
		static Multiplayer m_instance;
		return &m_instance;
	}

	void Multiplayer::Init()
	{
		if (this->pPeer == 0)
			this->pPeer = RakNet::RakPeerInterface::GetInstance();

		this->SetNetworkID(this->GetNetworkID());
	}

	void Multiplayer::Destroy()
	{
		if (this->pPeer)
		{
			RakNet::RakPeerInterface::DestroyInstance(this->pPeer);
			this->pPeer = 0;
		}
	}

	void Multiplayer::addToOnSendFuncMap(std::string key, std::function<void()> func)
	{
		LocalPlayerOnSendMap.insert(std::pair < std::string, std::function<void()>>(key, func));
	}

	void Multiplayer::addToOnReceiveFuncMap(unsigned char key, std::function<void(unsigned char, unsigned char *)> func)
	{
		RemotePlayerOnReceiveMap.insert(std::pair < unsigned char, std::function<void(unsigned char, unsigned char *)>>(key, func));
	}

	void Multiplayer::addToLobbyOnReceiveMap(unsigned char key, std::function<void(unsigned char, RakNet::Packet*)> func)
	{
		LobbyOnReceiveMap.insert(std::pair <unsigned char, std::function<void(unsigned char, RakNet::Packet*)>>(key, func));
	}

	int Multiplayer::GenerateSeed()
	{
		this->m_seed = (int)time(0);
		return m_seed;
	}

	void Multiplayer::SetupServer()
	{
		if(m_isRunning)
			this->pPeer->SetMaximumIncomingConnections(MAX_CONNECTIONS);
	}

	void Multiplayer::CloseServer(RakNet::SystemAddress ip)
	{
		if (strcmp(ip.ToString(), "UNASSIGNED_SYSTEM_ADDRESS") == 0)
			return;
		if (m_isRunning)
		{
			this->pPeer->CloseConnection(ip, true, (unsigned char)'\000', PacketPriority::IMMEDIATE_PRIORITY);
			this->pPeer->SetMaximumIncomingConnections(0);
			return;
		}
	}

	void Multiplayer::StartUpPeer()
	{
		RakNet::StartupResult result;
		m_isRunning = true;
		auto lol = RakNet::SocketDescriptor(PEER_PORT, 0);
		//specify the port(s) to listen on
		result = this->pPeer->Startup(MAX_CONNECTIONS, &lol, 1, THREAD_PRIORITY_NORMAL);
		if (result != RakNet::RAKNET_STARTED && result != RakNet::RAKNET_ALREADY_STARTED)
		{
			m_isRunning = false;
		}
		else
		{
			this->pPeer->SetMaximumIncomingConnections(0);
		}
	}

	void Multiplayer::ShutdownPeer()
	{
		if (this->pPeer && m_isRunning)
		{
			this->pPeer->Shutdown(1);
			m_isRunning = false;
		}
	}

	void Multiplayer::AdvertiseHost(const char * additionalData, size_t length)
	{
		static ChronoClock localClock;
		if (!localClock.isRunning())
		{
			localClock.start();
		}

		double elapsedTime = localClock.getElapsedTime();

		if (elapsedTime >= ADVERTISEMENT_FREQUENCE)
			this->pPeer->AdvertiseSystem(LAN_IP.c_str(), PEER_PORT, additionalData, (int)length);
	}

	bool Multiplayer::ConnectTo(RakNet::SystemAddress ip)
	{
		if (RakNet::ConnectionAttemptResult::CONNECTION_ATTEMPT_STARTED == this->pPeer->Connect(ip.ToString(), PEER_PORT, nullptr, 0))
			return true;
		else
			return false;
	}

	void Multiplayer::Disconnect(RakNet::SystemAddress ip)
	{
		if (strcmp(ip.ToString(), "UNASSIGNED_SYSTEM_ADDRESS") == 0)
			return;

		if (m_isRunning)
			pPeer->CloseConnection(ip, true, (unsigned char)'\000', PacketPriority::IMMEDIATE_PRIORITY);
	}

	void Multiplayer::ReadPackets()
	{
		RakNet::Packet * packet;
		
		for (packet = pPeer->Receive(); packet; pPeer->DeallocatePacket(packet), packet = pPeer->Receive()) 
		{
			unsigned char mID = this->GetPacketIdentifier(packet->data);

			this->HandleLobbyMessages(mID, packet);
			this->HandleGameMessages(mID, packet->data);
		}
	}

	void Multiplayer::setOccasionalPing()
	{
		if (pPeer)
			pPeer->SetOccasionalPing(true);
	}

	void Multiplayer::HandlePackets()
	{
		static Multiplayer * pNetwork = Multiplayer::GetInstance();
		pNetwork->ReadPackets();
	}

	void Multiplayer::SendPacket(const char * message, size_t length,PacketPriority priority)
	{
		static Multiplayer * pNetwork = Multiplayer::GetInstance();
		pNetwork->_send_packet(message, length, priority);
	}

	void Multiplayer::_send_packet(const char * message, size_t length, PacketPriority priority)
	{
		this->pPeer->Send(message,
			(int)length + 1,
			priority, 
			RELIABLE_ORDERED,
			0,
			RakNet::UNASSIGNED_RAKNET_GUID,
			true);
	}

	void Multiplayer::EndConnectionAttempt()
	{
		if (m_isConnected);
			
		else
		{
			this->pPeer->Shutdown(1);
			m_isConnected = m_isClient = m_isServer = m_isRunning = false;
		}
	}

	void Multiplayer::Update()
	{
		if (m_isRunning && !inPlayState)	
			this->ReadPackets();
	}


	void Multiplayer::setRole(int role)
	{
		switch (role)
		{
		case 0:
			m_isServer = true;
			m_isClient = false;
			break;
		case 1:
			m_isServer = false;
			m_isClient = true;
			break;
		default:
			m_isServer = false;
			m_isClient = false;
			break;
		}
	}

	std::string Multiplayer::GetNetworkStatistics()
	{
		RakNet::RakNetStatistics * data = 0;
		char buffer[1024];

		if (this->m_isConnected)
		{
			data = this->pPeer->GetStatistics(this->m_rIP, data);
			if (data)
			{
				RakNet::StatisticsToString(data, buffer, 2);
				return std::string(buffer);
			}
			else
				return std::string("Failed to Retrieve Network Statistics\n");
		}
		else
			return std::string("No Connection Available\n");
		
	}

	std::string Multiplayer::GetNID()
	{
		return std::to_string(Multiplayer::GetInstance()->GetNetworkID());
	}

	std::string Multiplayer::GetNetworkInfo()
	{
		std::string toReturn = "";
		toReturn = "Connected to: " + std::string(m_rIP.ToString());
		toReturn += "\nAverage Ping: " + std::to_string(this->pPeer->GetAveragePing(this->m_rIP));
		return toReturn;
	}

	RakNet::SystemAddress Multiplayer::GetMySysAdress()
	{
		if (this->pPeer->IsActive())
			return this->pPeer->GetMyBoundAddress();
		return RakNet::SystemAddress();
	}

	RakNet::RakNetGUID Multiplayer::GetMyGUID()
	{
		
		if (this->pPeer->IsActive())
			return this->pPeer->GetMyGUID();
		return RakNet::RakNetGUID();
	}


	Multiplayer::Multiplayer()
	{
	}


	Multiplayer::~Multiplayer()
	{
	}

	unsigned char Multiplayer::GetPacketIdentifier(unsigned char * data)
	{
		if ((unsigned char)data[0] == ID_TIMESTAMP)
			return (unsigned char)data[sizeof(unsigned char) + sizeof(RakNet::Time)];
		else
			return (unsigned char)data[0];
	}

	void Multiplayer::HandleGameMessages(unsigned char mID, unsigned char * data)
	{
		std::map<unsigned char, std::function<void(unsigned char, unsigned char*)>>::iterator mapIterator = RemotePlayerOnReceiveMap.find(mID);

		if (mapIterator != RemotePlayerOnReceiveMap.end())
			mapIterator->second(mID, data);
	}

	void Multiplayer::HandleLobbyMessages(unsigned char mID, RakNet::Packet * packet)
	{
		auto it = LobbyOnReceiveMap.find(mID);
		if (it != LobbyOnReceiveMap.end())
			it->second(mID, packet);
	}
}