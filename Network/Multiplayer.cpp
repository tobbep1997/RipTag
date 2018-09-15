#include "Multiplayer.h"



Network::Multiplayer::Multiplayer(Network::MP_TYPE type)
{
	if (m_Socket.bind(CONF_PORT) != sf::Socket::Done)
	{
		//error
	}

	if (type == MP_TYPE::SERVER)
		m_isServer = true;
	else if (type == MP_TYPE::CLIENT)
		m_isClient = true;
	else
		m_isServer = m_isClient = false;

	m_Socket.setBlocking(CONF_BLOCKING);

}


Network::Multiplayer::~Multiplayer()
{
}

void Network::Multiplayer::Broadcast(char arr[], unsigned int size)
{
	if (m_Socket.send(arr, size, sf::IpAddress::Broadcast, CONF_PORT) != sf::Socket::Done)
	{
		//error
	}
}
