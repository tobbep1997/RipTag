#pragma once


#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include <RakPeerInterface.h>
#include <RakNetStatistics.h>
#include "NetworkClock.h"
#include <string>
#include <NetworkIDManager.h>
#include "NetworkMessageIdentifiers.h"
#include <iostream>

#include <LuaTalker.h>
#include <map>
#include <functional>


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
		void Init();
		void Destroy();
		
		static std::map<std::string, std::function<void()>> onSendMap;
		static std::map<unsigned char, std::function<void(unsigned char *)>> onReceiveMap;

		static void addToOnSendFuncMap(std::string key, std::function<void()> func);
		static void addToOnReceiveFuncMap(unsigned char key, std::function<void(unsigned char *)> func);

		RakNet::NetworkIDManager * pNetworkIDManager = 0;
		
		void StartUpServer();
		void StartUpClient();

		void AdvertiseHost();
		void SearchLANHost();
		void SearchLANClient();
		void Disconnect();

		void ReadPackets();
		void DestroySentPacket(void * msg);
		void EndConnectionAttempt();
		void Update();
		//GETs 
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
		int Send_Data(sol::this_state s);
		static void REGISTER_TO_LUA();

		static void SendPacket(const char* message, size_t length, PacketPriority priority);
		void _send_packet(const char* message, size_t length, PacketPriority priority);
		//unsafe, find a better way
		//private constructor to avoid instanciating more than one object
		//the destructor of the singleton is called when the program exits.
		Multiplayer();

		~Multiplayer();
	private:

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
}

#endif