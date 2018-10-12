#pragma once
#include <RakPeerInterface.h>
#include <RakNetStatistics.h>
#include "NetworkClock.h"
#include <string>
#include <NetworkIDManager.h>
#include "NetworkMessageIdentifiers.h"
#include <iostream>

#include <LuaTalker.h>


#define LUA_START_SERVER "StartServer"
#define LUA_START_CLIENT "StartClient"
#define LUA_END_CONNECTION_ATTEMPT "EndConnectionAttempt"
#define LUA_DISCONNECT "Disconnect"
#define LUA_IS_SERVER "IsServer"
#define LUA_IS_CLIENT "IsClient"
#define LUA_IS_PEER_RUNNING "IsPeerRunning"
#define LUA_IS_CONNECTED "IsPeerConnected"
#define LUA_IS_GAME_RUNNING "IsGameRunningNetwork"
#define LUA_GET_MY_NID "GetMyNID"
#define LUA_SET_GAME_RUNNING_NETWORK "SetGameIsRunningNetwork"
#define LUA_SEND_PACKET "SendPacket"

#define LUA_TABLE_PACKET_PRIORITIES "PACKET"


namespace Network
{
	//Network constants
	const unsigned short PEER_PORT = 60005;

	const short MAX_CONNECTIONS = 2;
	const std::string LAN_IP = "255.255.255.255";

	//Other constants
	const double ADVERTISEMENT_FREQUENCE = 1 / 5.0;

	class Multiplayer : public RakNet::NetworkIDObject
	{
	public:
		//Get a pointer to the Singleton Instance
		static Multiplayer * GetInstance();
		
		RakNet::NetworkIDManager * pNetworkIDManager = 0;
		
		void StartUpServer();
		void StartUpClient();

		void AdvertiseHost();
		void SearchLANHost();
		void SearchLANClient();
		void Disconnect();

		void ReadPackets();
		void SendPacket(const char* message, size_t length, PacketPriority priority);
		void DestroySentPacket(void * msg);
		void EndConnectionAttempt();
		void Update();
		//GETs this is horrible design
		bool isServer() { return m_isServer; }
		bool isClient() { return m_isClient; }
		bool isRunning() { return m_isRunning; }
		bool isConnected() { return m_isConnected; }
		bool isGameRunning() { return m_isGameRunning; }

		std::string GetNetworkStatistics();
		std::string GetNID();
		std::string GetNetworkInfo();

		void setIsGameRunning(bool running) { this->m_isGameRunning = running; }

		//LUA
		static int Send_Data(lua_State * L);
		static void REGISTER_TO_LUA();


		//unsafe, find a better way
	private:
		Multiplayer();
		~Multiplayer();
		//private constructor to avoid instanciating more than one object
		//the destructor of the singleton is called when the program exits.
		bool m_isServer = false;
		bool m_isClient = false;
		bool m_isRunning = false;
		bool m_isConnected = false;
		bool m_isGameRunning = false;

		RakNet::RakPeerInterface * pPeer = 0;


		RakNet::SystemAddress m_rIP;

		unsigned char GetPacketIdentifier(unsigned char * data);
		void HandleRakNetMessages(unsigned char mID);
		void HandleGameMessages(unsigned char mID, unsigned char * data);

		//functions to handle RakNet internal messages
		void _onDisconnect();

	};


	static void LUA_Register_Packet_Priorities()
	{
		
		LUA::LuaTalker * m_talker = LUA::LuaTalker::GetInstance();
		m_talker->getSolState().new_enum(LUA_TABLE_PACKET_PRIORITIES,
			ENUM_TO_STR(LOW_PRIORITY), PacketPriority::LOW_PRIORITY,
			ENUM_TO_STR(HIGH_PRIORITY), PacketPriority::HIGH_PRIORITY,
			ENUM_TO_STR(IMMEDIATE_PRIORITY), PacketPriority::IMMEDIATE_PRIORITY
		);
	}



}