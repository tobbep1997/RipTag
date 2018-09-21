#pragma once
#include <RakPeerInterface.h>

namespace Network
{
	const unsigned short SERVER_PORT = 60005;
	const unsigned short CLIENT_PORT = 60006;
	const short MAX_CONNECTIONS = 2;


	class Multiplayer
	{
	public:
		//Get a pointer to the Singleton Instance
		static Multiplayer * GetInstance();
		//Assign the role, True sets it as Server, False sets it to Client
		void assignRole(bool isServer) { this->m_isServer = isServer; }
		void StartUp();
		void ConnectToLocalhost();
		void ReadPackets();
		void SendPacket(const char* message);

		bool isServer() { return m_isServer; }

	private:
		//private constructor to avoid instanciating more than one object
		Multiplayer();
		//the destructor of the singleton is called when the program exits.
		~Multiplayer();

		RakNet::RakPeerInterface * pPeer = 0;
		bool m_isServer = false;
	};

}