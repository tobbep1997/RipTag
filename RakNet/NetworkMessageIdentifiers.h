#pragma once
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define TEST_STRUCT_FOR_LUA "TestPacket"

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
		const char * message;
	};
//#pragma pack(pop)

//#pragma pack(push, 1)
	struct Game_Event
	{
		GAME_MESSAGES id;
		//data
	};
//#pragma pack(pop)

	static int New_Test_Data(lua_State * L)
	{
		const char * data = lua_tostring(L, lua_gettop(L));
		SCRIPT_TEST packet;
		packet.id = ID_PING_MESSAGE;
		packet.message = data;
		lua_pushlightuserdata(L, (void*)&packet);
		return 1;
	}

	static void LUA_Register_Network_Structs(lua_State * L)
	{
		lua_register(L, TEST_STRUCT_FOR_LUA, New_Test_Data);
	}

	//#pragma pack(push, 1)
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

}