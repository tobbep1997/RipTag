#pragma once
//includes
#include<SFML/Network.hpp>


namespace Network
{
	constexpr auto CONF_BLOCKING = false;
	constexpr auto CONF_PORT = 2028;

	enum MP_TYPE
	{
		SERVER,
		CLIENT
	};

	class Multiplayer
	{
	private:
		sf::UdpSocket m_Socket;
		sf::IpAddress m_rIp;
		unsigned short m_rPort;
		bool m_isServer = false;
		bool m_isClient = false;

	public:
		//default constructor binds the socket to a port, select type to create Sever or Client side
		Multiplayer(MP_TYPE type);
		~Multiplayer();
		void Broadcast(char arr[], unsigned int size);
		void Broadcast(sf::Packet& packet);
		
	};
}


