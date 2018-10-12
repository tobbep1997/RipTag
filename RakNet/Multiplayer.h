#pragma once
#include <RakPeerInterface.h>
#include <RakNetStatistics.h>
#include "NetworkClock.h"
#include <string>
#include <NetworkIDManager.h>
#include "NetworkMessageIdentifiers.h"
#include <iostream>

#include <LuaTalker.h>

#define NETWORK_METATABLE "Network"
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

		void setIsGameRunning(bool running) { this->m_isGameRunning = running; }
		std::string GetNetworkStatistics();
		std::string GetNID();
		std::string GetNetworkInfo();


		//LUA
		static bool isServerLUA() { return Multiplayer::GetInstance()->isServer(); }
		static bool isClientLUA() { return Multiplayer::GetInstance()->isClient(); }
		static bool isRunningLUA() { return Multiplayer::GetInstance()->isRunning(); }
		static bool isConnectedLUA() { return Multiplayer::GetInstance()->isConnected(); }
		static bool isGameRunningLUA() { return Multiplayer::GetInstance()->isGameRunning(); }
		static void StartUpServerLUA() { Multiplayer::GetInstance()->StartUpServer(); }
		static void StartUpClientLUA() { Multiplayer::GetInstance()->StartUpClient(); }
		static void EndConnectionAttemptLua() { Multiplayer::GetInstance()->EndConnectionAttempt(); }
		static std::string GetNidLUA() { Multiplayer::GetInstance()->GetNID(); }
		static int Send_Data(lua_State * L);
		static void REGISTER_TO_LUA();

		Multiplayer();
		~Multiplayer();

		//unsafe, find a better way
	private:
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

	static int Get_Instance(lua_State * L)
	{
		Multiplayer * pIns = Multiplayer::GetInstance();
		lua_pushlightuserdata(L, (void*)pIns);
		luaL_setmetatable(L, NETWORK_METATABLE);
		return 1;
	}


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