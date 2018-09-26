#pragma once
#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include "NetworkClock.h"
#include <string>

namespace Network
{
	//Network constants
	const unsigned short SERVER_PORT = 60005;
	const unsigned short CLIENT_PORT = 60006;
	const short MAX_CONNECTIONS = 2;
	const std::string LAN_IP = "255.255.255.255";

	//Other constants
	const double ADVERTISEMENT_FREQUENCE = 1 / 5.0;

	class Multiplayer
	{
	public:
		//Get a pointer to the Singleton Instance
		static Multiplayer * GetInstance();
		
		void StartUpServer();
		void StartUpClient();

		void AdvertiseHost();
		void SearchLANHost();
		void SearchLANClient();
		void Disconnect();

		void ReadPackets();
		void SendPacket(const char* message);

		void Update();
		//GETs
		bool isServer() { return m_isServer; }
		bool isClient() { return m_isClient; }
		bool isRunning() { return m_isRunning; }
		bool isConnected() { return m_isConnected; }


		std::string GetNetworkInfo();
	private:
		//private constructor to avoid instanciating more than one object
		Multiplayer();
		//the destructor of the singleton is called when the program exits.
		~Multiplayer();

		RakNet::RakPeerInterface * pPeer = 0;

		bool m_isServer = false;
		bool m_isClient = false;
		bool m_isRunning = false;
		bool m_isConnected = false;

		RakNet::SystemAddress m_rIP;

		unsigned char GetPacketIdentifier(RakNet::Packet * p);
		void HandleRakNetMessages(unsigned char mID);
		void HandleGameMessages(unsigned char mID);

		//functions to handle RakNet internal messages
		void _onDisconnect();
	};

}