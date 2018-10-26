#pragma once
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <string>
#include <GetTime.h>
#include <BitStream.h>
#include <RakNetDefines.h>



namespace Network
{

	enum GAME_MESSAGES
	{
		ID_GAME_START = ID_USER_PACKET_ENUM,
		ID_PLAYER_CREATE = ID_USER_PACKET_ENUM + 1,
		ID_PLAYER_DISCONNECT = ID_USER_PACKET_ENUM + 2,
		ID_PLAYER_MOVE = ID_USER_PACKET_ENUM + 3,
		ID_PLAYER_JUMP = ID_USER_PACKET_ENUM + 4
	};


//STRUCTS BEGIN
#pragma pack(push, 1)
	struct GAME_MESSAGE
	{
		unsigned char id;
		GAME_MESSAGE(unsigned char _id)
		{
			id = _id;
		}
	};

	struct ENTITY_CREATE
	{
		unsigned char id;
		RakNet::NetworkID nID;
		float x, y, z;
		ENTITY_CREATE(unsigned char _id, RakNet::NetworkID _nid, float _x, float _y, float _z)
		{
			id = _id;
			nID = _nid;
			x = _x;
			y = _y;
			z = _z;
		}
	};

	struct ENTITY_MOVE
	{
		unsigned char useTimestamp;
		RakNet::Time timeStamp;
		unsigned char id;
		RakNet::NetworkID nID;
		float x, y, z;
		int state;
		ENTITY_MOVE(unsigned char _id, RakNet::NetworkID _nid, float _x, float _y, float _z, int _state)
		{
			useTimestamp = ID_TIMESTAMP;
			timeStamp = RakNet::GetTime();
			id = _id;
			nID = _nid;
			x = _x;
			y = _y;
			z = _z;
			state = _state;
		}
	};

	struct ENTITY_EVENT
	{
		unsigned char id;
		RakNet::NetworkID nID;
		ENTITY_EVENT(unsigned char _id, RakNet::NetworkID _nid)
		{
			id = _id;
			nID = _nid;
		}
	};
#pragma pack(pop)
	//STRUCTS END
	

}
