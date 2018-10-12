#pragma once
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <string>
#include <GetTime.h>
#include <BitStream.h>
#include <RakNetDefines.h>

#include<LuaTalker.h>

#define LUA_PACKETS_METATABLE "Packets"
#define LUA_TABLE_MESSAGE_IDENTIFIERS "PacketID"
#define LUA_GAME_MESSAGE_PACKET "GameMessage"
#define LUA_DESTROY_GAME_MESSAGE "DestroyGameMessage"

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
		static std::tuple<void*, size_t> New_Game_Message(unsigned char _id);
		static void Destroy_Game_Message(void * ptr);


		static void REGISTER_TO_LUA();

	};

	

}
