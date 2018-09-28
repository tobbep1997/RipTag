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

#define PLAYER_MOVEMENT_FOR_LUA "MovePacket"
#define PACKETS_METATABLE "Packets"
#define LUA_TABLE_MESSAGE_IDENTIFIERS "NETWORK_MESSAGES"

#define ENUM_TO_STR(ENUM) std::string(#ENUM)

namespace Network
{
	enum GAME_MESSAGES
	{
		ID_GAME_START = ID_USER_PACKET_ENUM,
		ID_CREATE_REMOTE_PLAYER = ID_USER_PACKET_ENUM + 1,
		ID_UPDATE_SPHERE_LOCATION = ID_USER_PACKET_ENUM + 2,
		ID_PING_MESSAGE = ID_USER_PACKET_ENUM + 3
	};


	//STRUCTS BEGIN
	#pragma pack(push, 1)
	
	struct Player_Movement
	{
		unsigned char useTimeStamp; // Assign ID_TIMESTAMP to this
		RakNet::Time timeStamp; // Put the system time in here returned by RakNet::GetTime() or some other method that returns a similar value
		unsigned char id; // Our network defined enum types
		float x, y, z; // Character position
		RakNet::NetworkID networkId;
		//RakNet::SystemAddress systemAddress;

		Player_Movement(unsigned char _id, float _x, float _y, float _z, RakNet::NetworkID _networkId)
		{
			id = _id;
			x = _x;
			y = _y;
			z = _z;
			networkId = _networkId;
		}
	};

	#pragma pack(pop)
	//STRUCTS END

	static int New_Player_Movement_Data(lua_State * L)
	{
		float x, y, z;
		x = (float)lua_tonumber(L, lua_gettop(L));
		y = (float)lua_tonumber(L, lua_gettop(L));
		z = (float)lua_tonumber(L, lua_gettop(L));
		RakNet::NetworkID networkId = (RakNet::NetworkID)lua_tonumber(L, lua_gettop(L));
		lua_pop(L, 4);
		Player_Movement * packet = new Player_Movement(ID_UPDATE_SPHERE_LOCATION, x, y, z, networkId);
		packet->useTimeStamp = ID_TIMESTAMP;
		packet->timeStamp = RakNet::GetTime();
		lua_pushlightuserdata(L, (void*)packet);
		return 1;
	}

	static void LUA_Register_Network_Structs(lua_State * L)
	{
		lua_register(L, PLAYER_MOVEMENT_FOR_LUA, New_Player_Movement_Data);
	}

	static void LUA_Setfield_Enum(lua_State* L, std::string index, int value)
	{
		lua_pushstring(L, index.c_str());
		lua_pushnumber(L, (unsigned char)value);
		lua_settable(L, -3);
	}

	static void LUA_Register_Network_MessageTypes(lua_State *L)
	{
		lua_newtable(L);

		LUA_Setfield_Enum(L, ENUM_TO_STR(ID_GAME_START), GAME_MESSAGES::ID_GAME_START);
		LUA_Setfield_Enum(L, ENUM_TO_STR(ID_CREATE_REMOTE_PLAYER), GAME_MESSAGES::ID_CREATE_REMOTE_PLAYER);
		LUA_Setfield_Enum(L, ENUM_TO_STR(ID_PING_MESSAGE), GAME_MESSAGES::ID_PING_MESSAGE);
		LUA_Setfield_Enum(L, ENUM_TO_STR(ID_UPDATE_SPHERE_LOCATION), GAME_MESSAGES::ID_UPDATE_SPHERE_LOCATION);

		lua_setglobal(L, LUA_TABLE_MESSAGE_IDENTIFIERS);
	}

}
