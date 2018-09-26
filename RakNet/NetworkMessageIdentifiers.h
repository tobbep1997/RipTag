#pragma once
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>

namespace Network
{
	enum GAME_MESSAGES
	{
		ID_GAME_START = ID_USER_PACKET_ENUM + 1,
		ID_CREATE_REMOTE_PLAYER = ID_USER_PACKET_ENUM + 2,
		ID_
	};

}

#pragma pack(push, 1)
struct mystruct
{
	RakNet::Time t;

};
#pragma pack(pop)

