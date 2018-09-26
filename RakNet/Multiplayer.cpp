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
				return;
			}
		}
	}

	void Multiplayer::Disconnect()
	{
		if (m_isConnected)
		{
			pPeer->CloseConnection(m_rIP, true);
			m_isConnected = m_isClient = m_isServer = m_isRunning = false;
		}
	}

	void Multiplayer::ReadPackets()
	{
		static int packetsCounter = 0;
		RakNet::Packet * packet;
		
		for (packet = pPeer->Receive(); packet; pPeer->DeallocatePacket(packet), packet = pPeer->Receive()) 
		{
			/*packetsCounter++;
			std::cout << "--------------------------NEW PACKET--------------------------\n";
			std::cout << "System Adress: "; std::cout << packet->systemAddress.ToString() << std::endl;
			std::cout << "RakNet GUID: "; std::cout << packet->guid.ToString() << std::endl;
			std::cout << "Lenght of Data in Bytes: " + std::to_string(packet->length) << std::endl;
			std::cout << "Message Identifier: " + std::to_string(packet->data[0]) << std::endl;
			std::cout << "\nDATA:\n"; std::cout << std::string((char*)packet->data, packet->length);
			std::cout << "\n\nReceived Packets Amount: " + std::to_string(packetsCounter) << std::endl;*/
			unsigned char mID = this->GetPacketIdentifier(packet);
			this->HandleRakNetMessages(mID);
			this->HandleGameMessages(mID);
		}
	}

	void Multiplayer::SendPacket(const char * message)
	{
		this->pPeer->Send(message,
			std::strlen(message) + 1,
			HIGH_PRIORITY, RELIABLE,
			0,
			RakNet::UNASSIGNED_RAKNET_GUID,
			true);
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


	std::string Multiplayer::GetNetworkInfo()
	{
		std::string toReturn = "";
		toReturn = "Connected to: " + std::string(m_rIP.ToString());
		return toReturn;
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
		{
			this->Disconnect();
			RakNet::RakPeerInterface::DestroyInstance(this->pPeer);
		}

		std::cout << "Multiplayer Destructor is called.\n";
	}

	unsigned char Multiplayer::GetPacketIdentifier(RakNet::Packet * p)
	{
		if ((unsigned char)p->data[0] == ID_TIMESTAMP)
			return (unsigned char)p->data[sizeof(unsigned char) + sizeof(unsigned long)];
		else
			return (unsigned char)p->data[0];
	}

	void Multiplayer::HandleRakNetMessages(unsigned char mID)
	{
		switch (mID)
		{
		case DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION:
			this->_onDisconnect();
			break;
		default:
			break;
		}
	}

	void Multiplayer::HandleGameMessages(unsigned char mID)
	{
		switch (mID)
		{
		default:
			break;
		}
	}

	void Multiplayer::_onDisconnect()
	{
		m_isConnected = m_isClient = m_isServer = m_isRunning = false;
		//might want to log who disconnected
	}




}