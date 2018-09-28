#pragma once
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <string>
#include <GetTime.h>

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
		unsigned char id; // Our network defined enum types
		float x, y, z, w; // Character position
		RakNet::NetworkID networkId;
		//RakNet::SystemAddress systemAddress;

		Player_Movement(unsigned char _id, float _x, float _y, float _z, float _w, RakNet::NetworkID _networkId)
		{
			id = _id;
			x = _x;
			y = _y;
			z = _z;
			w = _w;
			networkId = _networkId;
		}
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
		float x, y, z, w;
		x = (float)lua_tonumber(L, lua_gettop(L));
		y = (float)lua_tonumber(L, lua_gettop(L));
		z = (float)lua_tonumber(L, lua_gettop(L));
		w = (float)lua_tonumber(L, lua_gettop(L));
		RakNet::NetworkID networkId = (RakNet::NetworkID)lua_tonumber(L, lua_gettop(L));
		lua_pop(L, 4);
		Player_Movement * packet = new Player_Movement(ID_UPDATE_SPHERE_LOCATION, x, y, z, w, networkId);
		packet->useTimeStamp = ID_TIMESTAMP;
		packet->timeStamp = RakNet::GetTime();
		lua_pushlightuserdata(L, (void*)packet);
		return 1;
	}

	static void LUA_Register_Network_Structs(lua_State * L)
	{
		lua_register(L, TEST_STRUCT_FOR_LUA, New_Test_Data);
		lua_register(L, PLAYER_MOVEMENT_FOR_LUA, New_Player_Movement_Data);
	}
}