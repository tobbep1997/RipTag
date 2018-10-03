#pragma once
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <string>
#include <GetTime.h>
#include <BitStream.h>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define PLAYER_MOVEMENT_FOR_LUA "MovePacket"
#define PACKETS_METATABLE "PACKETS"
#define LUA_TABLE_MESSAGE_IDENTIFIERS "NETWORK_MESSAGES"
#define LUA_ENTITY_CREATION "CreateEntityMsg"
#define LUA_GAME_MESSAGE "GameMessage"

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
	struct GAME_MESSAGE
	{
		unsigned char id;
		GAME_MESSAGE(unsigned char _id)
		{
			id = _id;
		}
	};
#pragma pack(pop)
#pragma pack(push, 1)
	struct ENTITY_MOVE_MESSAGE
	{
		unsigned char useTimeStamp; // Assign ID_TIMESTAMP to this
		RakNet::Time timeStamp; // Put the system time in here returned by RakNet::GetTime() or some other method that returns a similar value
		unsigned char id; // Our network defined enum types
		RakNet::NetworkID networkId;
		float x, y, z; // Character position
		//RakNet::SystemAddress systemAddress;

		ENTITY_MOVE_MESSAGE(unsigned char _id, RakNet::NetworkID _networkId, float _x, float _y, float _z)
		{
			id = _id;
			x = _x;
			y = _y;
			z = _z;
			networkId = _networkId;
			useTimeStamp = ID_TIMESTAMP;
			timeStamp = RakNet::GetTime();
			//RakNet::BitStream bs;
			//bs.EndianSwapBytes(1, sizeof(RakNet::Time));
		}
	};
#pragma pack(pop)
#pragma pack(push, 1)
	struct ENTITY_CREATE_MESSAGE
	{
		unsigned char id;
		RakNet::NetworkID nId;
		float pos[3];

		ENTITY_CREATE_MESSAGE(unsigned char _id, RakNet::NetworkID _nid, double _x, double _y, double _z)
		{
			id = _id;
			nId = _nid;
			pos[0] = _x;
			pos[1] = _y;
			pos[2] = _z;
		}
	};
#pragma pack(pop)
	//STRUCTS END

	static int New_Game_Message(lua_State * L)
	{
		unsigned char id = (unsigned char)lua_tonumber(L, -1);
		lua_pop(L, 1);
		GAME_MESSAGE * msg = new GAME_MESSAGE(id);
		lua_pushlightuserdata(L, (void*)msg);
		lua_pushnumber(L, sizeof(GAME_MESSAGE));

		return 2;
	}

	static int New_Player_Movement_Data(lua_State * L)
	{
		float x, y, z;
		unsigned char id = (unsigned char)lua_tonumber(L, -5);
		RakNet::NetworkID networkId = (RakNet::NetworkID)lua_tonumber(L, -4);
		x = (float)lua_tonumber(L, -3);
		y = (float)lua_tonumber(L, -2);
		z = (float)lua_tonumber(L, -1);
		lua_pop(L, 5);

		ENTITY_MOVE_MESSAGE * packet = new ENTITY_MOVE_MESSAGE(id, networkId, x, y, z);
		
		lua_pushlightuserdata(L, (void*)packet);
		lua_pushnumber(L, sizeof(ENTITY_MOVE_MESSAGE));

		return 2;
	}

	static int New_Entity_Creation_Message(lua_State * L)
	{
		float x, y, z;
		unsigned char id = (unsigned char)lua_tonumber(L, -5);
		RakNet::NetworkID networkId = (RakNet::NetworkID)lua_tonumber(L, -4);
		x = lua_tonumber(L, -3);
		y = lua_tonumber(L, -2);
		z = lua_tonumber(L, -1);
		lua_pop(L, 5);
		ENTITY_CREATE_MESSAGE * packet = new ENTITY_CREATE_MESSAGE(id, networkId, x, y, z);
		lua_pushlightuserdata(L, (void*)packet);
		lua_pushnumber(L, sizeof(ENTITY_CREATE_MESSAGE));
		return 2;
	}

	static void LUA_Register_Network_Structs(lua_State * L)
	{
		lua_register(L, LUA_GAME_MESSAGE, New_Game_Message);
		lua_register(L, PLAYER_MOVEMENT_FOR_LUA, New_Player_Movement_Data);
		lua_register(L, LUA_ENTITY_CREATION, New_Entity_Creation_Message);
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
