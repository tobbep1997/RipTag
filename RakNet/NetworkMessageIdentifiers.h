#pragma once
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <string>
#include <GetTime.h>
#include <BitStream.h>
#include <RakNetDefines.h>

#include<LuaTalker.h>

#define PLAYER_MOVEMENT_FOR_LUA "MovePacket"
#define PACKETS_METATABLE "PACKETS"
#define LUA_TABLE_MESSAGE_IDENTIFIERS "PacketID"
#define LUA_ENTITY_CREATION "CreateEntityMsg"
#define LUA_GAME_MESSAGE "GameMessage"

#define ENUM_TO_STR(ENUM) std::string(#ENUM)

namespace Network
{

	enum GAME_MESSAGES
	{
		ID_GAME_START = ID_USER_PACKET_ENUM
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
	//STRUCTS END
	

	class Packets
	{
	private:
	public:
		//always create a pair of constructor and destructor for each struct declared
		Packets(){}
		~Packets(){}
		//MAKE SURE TO CALL THE DESTROY FUNCTION OF THE TYPE AFTER YOU HAVE CREATED ONE AND SENT THE PACKET
		static int New_Game_Message(lua_State * L);
		static int Destroy_Game_Message(lua_State * L);


		static void REGISTER_TO_LUA();

	};

	class PacketIdentifiers
	{
	private:
		PacketIdentifiers(){}
		~PacketIdentifiers(){}
	public:
		static void REGISTER_TO_LUA();
	};
	

}
