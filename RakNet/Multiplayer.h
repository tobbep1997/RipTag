#pragma once


#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include <RakPeerInterface.h>
#include <RakNetStatistics.h>
#include <NetworkIDObject.h>

#include "NetworkClock.h"
#include "NetworkMessageIdentifiers.h"

#include <string>
#include <map>
#include <functional>

struct CoopData
{
	int seed;
	int localPlayerCharacter;
	int remotePlayerCharacter;
	RakNet::NetworkID remoteID;
	int role;
	bool skipTutorial;
};

namespace Network
{

	//Network constants
	const unsigned short PEER_PORT = 60005;
	const short MAX_CONNECTIONS = 1;
	const std::string LAN_IP = "255.255.255.255";
	const double ADVERTISEMENT_FREQUENCE = 1 / 0.5;

	class Multiplayer : public RakNet::NetworkIDObject
	{
	public:
		//Get a pointer to the Singleton Instance
		static Multiplayer * GetInstance();
		void Init();
		void Destroy();
		
		//---- MAPS ----
		static std::map<std::string, std::function<void()>> LocalPlayerOnSendMap;
		static std::map<unsigned char, std::function<void(unsigned char, unsigned char *)>> RemotePlayerOnReceiveMap;
		static std::map<unsigned char, std::function<void(unsigned char, RakNet::Packet*)>> LobbyOnReceiveMap;
		//--------------

		static void addToOnSendFuncMap(std::string key, std::function<void()> func);
		static void addToOnReceiveFuncMap(unsigned char key, std::function<void(unsigned char, unsigned char *)> func);
		static void addToLobbyOnReceiveMap(unsigned char key, std::function<void(unsigned char, RakNet::Packet*)> func);

		static bool inPlayState;

		int GenerateSeed();
		int GetSeed() { return m_seed; } 
		
		void SetupServer();
		void CloseServer(RakNet::SystemAddress ip);

		void StartUpPeer();
		void ShutdownPeer();

		void AdvertiseHost(const char * additionalData = nullptr, size_t length = 0);
		bool ConnectTo(RakNet::SystemAddress ip);

		void Disconnect(RakNet::SystemAddress);

		void ReadPackets();
		void EndConnectionAttempt();
		void Update();
		//GETs 
		bool isServer() { return m_isServer; }
		bool isClient() { return m_isClient; }
		bool isRunning() { return m_isRunning; }
		bool isConnected() { return m_isConnected; }

		void setIsConnected(bool b) { this->m_isConnected = b; }
		void setRole(int role = -1);

		std::string GetNetworkStatistics();
		std::string GetNID();
		std::string GetNetworkInfo();
		RakNet::SystemAddress GetMySysAdress();
		RakNet::RakNetGUID GetMyGUID();

		void setOccasionalPing();

		static void HandlePackets();
		static void SendPacket(const char* message, size_t length, PacketPriority priority);
		void _send_packet(const char* message, size_t length, PacketPriority priority);
		
	private:
		Multiplayer();
		~Multiplayer();

		bool m_isServer = false;
		bool m_isClient = false;
		bool m_isRunning = false;
		bool m_isConnected = false;

		int m_seed = 0;

		RakNet::RakPeerInterface * pPeer = 0;
		RakNet::SystemAddress m_rIP;

		unsigned char GetPacketIdentifier(unsigned char * data);

		void HandleGameMessages(unsigned char mID, unsigned char * data);
		void HandleLobbyMessages(unsigned char mID, RakNet::Packet * packet);
	};
}

#endif