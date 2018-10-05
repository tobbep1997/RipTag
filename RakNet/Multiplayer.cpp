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

	void Multiplayer::StartUpServer()
	{
		RakNet::StartupResult result;
		
		m_isServer = true;
		m_isClient = false;
		m_isRunning = true;

		//specify the port(s) to listen on
		result = this->pPeer->Startup(MAX_CONNECTIONS, &RakNet::SocketDescriptor(PEER_PORT, 0), 1, THREAD_PRIORITY_NORMAL);
		if (result != RakNet::RAKNET_STARTED && result != RakNet::RAKNET_ALREADY_STARTED)
		{
			m_isRunning = false;
		}

		this->pPeer->SetMaximumIncomingConnections(MAX_CONNECTIONS);
	}

	void Multiplayer::StartUpClient()
	{
		RakNet::StartupResult result;

		m_isServer = false;
		m_isClient = true;
		m_isRunning = true;

		//a client doesn't need to listen on a specific port - auto assigned
		result = this->pPeer->Startup(MAX_CONNECTIONS, &RakNet::SocketDescriptor(PEER_PORT, 0), 1, THREAD_PRIORITY_NORMAL);

		if (result != RakNet::RAKNET_STARTED && result != RakNet::RAKNET_ALREADY_STARTED)
		{
			m_isRunning = false;
		}
	}

	void Multiplayer::AdvertiseHost()
	{
		static ChronoClock localClock;
		if (localClock.isRunning() == false)
		{
			localClock.start();
		}

		double elapsedTime = localClock.getElapsedTime();

		if (elapsedTime >= ADVERTISEMENT_FREQUENCE)
			this->pPeer->AdvertiseSystem(LAN_IP.c_str(), PEER_PORT, nullptr, 0);
	}

	void Multiplayer::SearchLANHost()
	{
		static bool connectionAttempt = false;

		RakNet::Packet * packet;
		for (packet = pPeer->Receive(); packet; pPeer->DeallocatePacket(packet), packet = pPeer->Receive())
		{
			//look for the advertise message
			if (packet->data[0] == DefaultMessageIDTypes::ID_ADVERTISE_SYSTEM && !packet->wasGeneratedLocally)
			{
				//save the server IP and send a connection request
				this->m_rIP = packet->systemAddress;
				if (RakNet::ConnectionAttemptResult::CONNECTION_ATTEMPT_STARTED == this->pPeer->Connect(this->m_rIP.ToString(), this->m_rIP.GetPort(), nullptr, 0))
					connectionAttempt = true;
			}
			else if (packet->data[0] == DefaultMessageIDTypes::ID_CONNECTION_REQUEST_ACCEPTED && !packet->wasGeneratedLocally && connectionAttempt)
			{
				this->m_rIP = packet->systemAddress;
				this->m_isConnected = true;
				connectionAttempt = false;
				//we are now connected we can exit the loop immidietly
				pPeer->DeallocatePacket(packet);
				pPeer->SetOccasionalPing(true);
				return;
			}
			else if (packet->data[0] == DefaultMessageIDTypes::ID_CONNECTION_ATTEMPT_FAILED)
			{
				//we should also log the failed connection attempt
				connectionAttempt = false;
			}
		}
	}

	void Multiplayer::SearchLANClient()
	{

		RakNet::Packet * packet;
		for (packet = pPeer->Receive(); packet; pPeer->DeallocatePacket(packet), packet = pPeer->Receive())
		{
			if (packet->data[0] == DefaultMessageIDTypes::ID_NEW_INCOMING_CONNECTION && !packet->wasGeneratedLocally)
			{
				m_rIP = packet->systemAddress;
				m_isConnected = true;
				pPeer->DeallocatePacket(packet);
				pPeer->SetOccasionalPing(true);
				return;
			}
		}
	}

	void Multiplayer::Disconnect()
	{
		if (m_isConnected)
		{
			pPeer->CloseConnection(m_rIP, true);
			m_isConnected = m_isClient = m_isServer = m_isRunning = m_isGameRunning = false;
		}
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
			//std::cout << "\nDATA:\n"; std::cout << std::string((char*)packet->data, packet->length);
			std::cout << "\n\nReceived Packets Amount: " + std::to_string(packetsCounter) << std::endl;
			unsigned char mID = this->GetPacketIdentifier(packet->data);
			this->HandleRakNetMessages(mID);
			this->HandleGameMessages(mID, packet->data);
			
		}
	}

	void Multiplayer::SendPacket(const char * message, size_t length,PacketPriority priority)
	{
		this->pPeer->Send(message,
			length + 1,
			priority, 
			RELIABLE_ORDERED,
			0,
			RakNet::UNASSIGNED_RAKNET_GUID,
			true);
	}

	void Multiplayer::DestroySentPacket(void * msg)
	{
		unsigned char id = GetPacketIdentifier((unsigned char*)msg);

		//list of all structs
		ENTITY_MOVE_MESSAGE * pEMM = 0;
		GAME_MESSAGE * pGM = 0;
		ENTITY_CREATE_MESSAGE * pECM = 0;

		switch (id)
		{
		case ID_GAME_START:
			pGM = (GAME_MESSAGE*)msg;
			delete pGM; pGM = 0;
			break;
		case ID_CREATE_REMOTE_PLAYER:
			pECM = (ENTITY_CREATE_MESSAGE*)msg;
			delete pECM; pECM = 0;
			break;
		case ID_UPDATE_SPHERE_LOCATION:
			pEMM = (ENTITY_MOVE_MESSAGE*)msg;
			delete pEMM; pEMM = 0;
			break;
		default:
			break;
		}

	}

	void Multiplayer::EndConnectionAttempt()
	{
		if (m_isConnected)
			this->Disconnect();
		else
		{
			this->pPeer->Shutdown(1);
			m_isConnected = m_isClient = m_isServer = m_isRunning = false;
		}
	}

	void Multiplayer::Update()
	{
		if (m_isRunning)
		{
			if (m_isServer && !m_isConnected)
			{
				this->AdvertiseHost();
				this->SearchLANClient();
			}
			else if (m_isClient && !m_isConnected)
			{
				this->SearchLANHost();
			}
			else if (m_isConnected)
			{
				this->ReadPackets();
			}
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

	std::string Multiplayer::GetNetworkInfo()
	{
		std::string toReturn = "";
		toReturn = "Connected to: " + std::string(m_rIP.ToString());
		toReturn += "\nAverage Ping: " + std::to_string(this->pPeer->GetAveragePing(this->m_rIP));
		return toReturn;
	}

	Multiplayer::Multiplayer()
	{
		if (this->pPeer == 0)
			this->pPeer = RakNet::RakPeerInterface::GetInstance();

		if (pNetworkIDManager == 0)
			pNetworkIDManager = RakNet::NetworkIDManager::GetInstance();
		
		std::cout << "Multiplayer Constructor is called.\n";
	}


	Multiplayer::~Multiplayer()
	{
		if (this->pPeer)
		{
			this->Disconnect();
			RakNet::RakPeerInterface::DestroyInstance(this->pPeer);
		}
		if (this->pNetworkIDManager)
			RakNet::NetworkIDManager::DestroyInstance(this->pNetworkIDManager);
		std::cout << "Multiplayer Destructor is called.\n";
	}

	unsigned char Multiplayer::GetPacketIdentifier(unsigned char * data)
	{
		if ((unsigned char)data[0] == ID_TIMESTAMP)
			return (unsigned char)data[sizeof(unsigned char) + sizeof(RakNet::Time)];
		else
			return (unsigned char)data[0];
	}

	void Multiplayer::HandleRakNetMessages(unsigned char mID)
	{
		switch (mID)
		{
		case DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION:
			this->Disconnect();
			break;
		case DefaultMessageIDTypes::ID_CONNECTION_LOST:
			this->Disconnect();
			break;
		case DefaultMessageIDTypes::ID_NO_FREE_INCOMING_CONNECTIONS:
			// Print message: Server is full
			break;
		default:
			break;
		}
	}

	void Multiplayer::HandleGameMessages(unsigned char mID, unsigned char * data)
	{
		//Pointer to our lua_State (i just want a short variable for cleaner code)
		lua_State * L = this->pLuaState;

		//Call script after Message identification
		switch (mID)
		{
		case ID_GAME_START:
			if (m_isClient)
				this->m_isGameRunning = true;
			lua_getglobal(L, "GameStart");
			lua_pushboolean(L, this->isServer());
			lua_pcall(L, 1, 0, NULL);

			break;
		case ID_CREATE_REMOTE_PLAYER:
			//run creation script function
			lua_getglobal(L, "CreateCubePrototypeRemote");
			lua_pushlightuserdata(L, (void*)data);
			lua_pcall(L, 1, 0, NULL);
			
			break;
		case ID_UPDATE_SPHERE_LOCATION:
			// Insert script
			lua_getglobal(L, "RemotePlayerMoved");
			lua_pushlightuserdata(L, (void*)data);
			lua_pcall(L, 1, 0, NULL);

			break;
		default:
			break;
		}
	}

	void Multiplayer::_onDisconnect()
	{
		m_isConnected = m_isClient = m_isServer = m_isRunning = false;
		// Go back to menu and print a message that the player or you lost connection
		//might want to log who disconnected
	}




}