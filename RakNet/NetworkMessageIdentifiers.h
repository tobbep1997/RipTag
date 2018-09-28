#pragma once
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <string>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define TEST_STRUCT_FOR_LUA "TestPacket"
#define PLAYER_MOVEMENT_FOR_LUA "Move"
#define PACKETS_METATABLE "Packets"

namespace Network
{
	enum GAME_MESSAGES
	{
		ID_GAME_START = ID_USER_PACKET_ENUM,
		ID_CREATE_REMOTE_PLAYER = ID_USER_PACKET_ENUM + 1,
		ID_UPDATE_SPHERE_LOCATION = ID_USER_PACKET_ENUM + 2,
		ID_PING_MESSAGE = ID_USER_PACKET_ENUM + 3
	};


	#pragma pack(push, 1)
	struct SCRIPT_TEST
	{
		unsigned char id;
		//data
		char msg[256];
		
		SCRIPT_TEST(unsigned char _id, const char * data)
		{
			id = _id;
			std::strncpy(msg, data, sizeof(msg));
		}
	};
//#pragma pack(pop)

//#pragma pack(push, 1)
	struct Game_Event
	{
		GAME_MESSAGES id;
		//data
	};
//#pragma pack(pop)

	//#pragma pack(push, 1)
	struct Player_Movement
	{
		unsigned char useTimeStamp; // Assign ID_TIMESTAMP to this
		RakNet::Time timeStamp; // Put the system time in here returned by RakNet::GetTime() or some other method that returns a similar value
		unsigned char typeId; // Our network defined enum types
		float x, y, z; // Character position
		RakNet::NetworkID networkId;
		RakNet::SystemAddress systemAddress;
	};
	#pragma pack(pop)

	static int New_Test_Data(lua_State * L)
	{
		const char * data = lua_tostring(L, lua_gettop(L));
		SCRIPT_TEST * packet = new SCRIPT_TEST((RakNet::MessageID)ID_PING_MESSAGE, data);
		lua_pushlightuserdata(L, (void*)packet);
		return 1;
	}

	static int New_Player_Movement_Data(lua_State * L)
	{
		const char * data = lua_tostring(L, lua_gettop(L));
		Player_Movement packet;
		packet.useTimeStamp = ID_TIMESTAMP;
		//packet.timeStamp = RakNet::GetTime();
		packet.typeId = ID_UPDATE_SPHERE_LOCATION;
		return 0;
	}

	static void LUA_Register_Network_Structs(lua_State * L)
	{
		lua_register(L, TEST_STRUCT_FOR_LUA, New_Test_Data);
		lua_register(L, PLAYER_MOVEMENT_FOR_LUA, New_Player_Movement_Data);
	}
}
/*
Network::Player_Movement pM;
pM.useTimeStamp = ID_TIMESTAMP;
pM.timeStamp = RakNet::GetTime();
pM.typeId = Network::ID_UPDATE_SPHERE_LOCATION;
pM.x = pos.x;
pM.y = pos.y;
pM.z = pos.z;
pM.networkId = id;
pM.systemAddress = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
pMP->SendPacket((const char *)&pM);
*/