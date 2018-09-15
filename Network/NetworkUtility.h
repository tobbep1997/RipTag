#pragma once
#include <SFML/Network.hpp>
#include <chrono>

namespace Network
{
	enum MID
	{
		//GAME STATE
		ID_NEWLOBBY = 0,
		ID_JOINLOBBY = 1,
		ID_LEAVELOBBY = 2,
		ID_QUITLOBBY = 3,
		ID_STARTGAME = 4,
		
		//PLAYER
		ID_MOVE = 5,
		ID_SHOOT = 6,
		ID_HOOK = 7,
		ID_BLINK = 8,
		ID_POSSESS = 9,
		ID_INTERACT = 10,
		ID_PAUSE = 11,
		ID_GAMEOVER = 12,
		ID_WIN = 13,
		ID_QUIT = 14,

		//Etc.
	};

	//Basklass
	class Message
	{
	public:
		sf::Uint8 UID;
		time_t timestamp;
		Message(unsigned int uid, time_t tstamp) : UID(uid), timestamp(tstamp) {}
		~Message(){}
	};

	sf::Packet& operator<<(sf::Packet& packet, const Message& message)
	{
		return packet << message.UID << message.timestamp;
	}

	sf::Packet& operator>>(sf::Packet& packet, Message& message)
	{
		return packet >> message.UID >> message.timestamp;
	}

	class GameMessage : public Message
	{
	public:
		//some information
		int keycode;
		GameMessage(unsigned int uid, time_t tstamp, int kcode) : Message(uid, tstamp) , keycode(kcode) {}
		~GameMessage() {}

	};

	sf::Packet& operator<<(sf::Packet& packet, const GameMessage& message)
	{
		return packet << message.UID << message.timestamp << message.keycode;
	}

	sf::Packet& operator>>(sf::Packet& packet, GameMessage& message)
	{
		return packet >> message.UID >> message.timestamp >> message.keycode;
	}


}
