#pragma once
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>

namespace Network
{
	enum GAME_MESSAGES
	{
		ID_GAME_START = ID_USER_PACKET_ENUM,
		ID_CREATE_REMOTE_PLAYER = ID_USER_PACKET_ENUM + 1,
		ID_UPDATE_SPHERE_LOCATION = ID_USER_PACKET_ENUM + 2
	};

}

#pragma pack(push, 1)
struct PlayerMovement
{
	unsigned char useTimeStamp; // Assign ID_TIMESTAMP to this
	RakNet::Time timeStamp; // Put the system time in here returned by RakNet::GetTime() or some other method that returns a similar value
	unsigned char typeId; // Our network defined enum types
	float x, y, z; // Character position
	RakNet::NetworkID networkId;
	RakNet::SystemAddress systemAddress;
};
#pragma pack(pop)

