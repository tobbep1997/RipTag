#pragma once
#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include "NetworkClock.h"
#include <string>
#include <NetworkIDManager.h>
#include "NetworkMessageIdentifiers.h"
#include <iostream>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define NETWORK_METATABLE "Network"
#define GET_MP_INSTANCE "Multiplayer"
#define SEND_DATA "Send"

namespace Network
{
	//Network constants
	const unsigned short PEER_PORT = 60005;

	const short MAX_CONNECTIONS = 2;
	const std::string LAN_IP = "255.255.255.255";

	//Other constants
	const double ADVERTISEMENT_FREQUENCE = 1 / 5.0;

	class Multiplayer
	{
	public:
		//Get a pointer to the Singleton Instance
		static Multiplayer * GetInstance();
		lua_State * pLuaState = nullptr;

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
		RakNet::NetworkIDManager * pNetworkIDManager = 0;

		bool m_isServer = false;
		bool m_isClient = false;
		bool m_isRunning = false;
		bool m_isConnected = false;

		RakNet::SystemAddress m_rIP;

		unsigned char GetPacketIdentifier(RakNet::Packet * p);
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

	static int Send_Data(lua_State * L)
	{
		void * data = lua_touserdata(L, lua_gettop(L));
		Multiplayer::GetInstance()->SendPacket((const char*)data);

		return 0;
	}

	static void LUA_Register_Network(lua_State * L)
	{
		lua_register(L, GET_MP_INSTANCE, Get_Instance);
		luaL_newmetatable(L, NETWORK_METATABLE);
		lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, Send_Data); lua_setfield(L, -2, SEND_DATA);
		lua_pop(L, 1);
	}
}