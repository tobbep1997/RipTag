#pragma once
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <string>
#include <GetTime.h>
#include <BitStream.h>
#include <RakNetDefines.h>

#include <LuaTalker.h>

#define LUA_PACKETS_METATABLE "Packets"
#define LUA_TABLE_MESSAGE_IDENTIFIERS "PacketID"
#define LUA_GAME_MESSAGE_PACKET "GameMessage"
#define LUA_DESTROY_GAME_MESSAGE "DestroyGameMessage"

#define ENUM_TO_STR(ENUM) std::string(#ENUM)

namespace Network
{

	enum GAME_MESSAGES
	{
		ID_GAME_START = ID_USER_PACKET_ENUM,
		ID_PLAYER_CREATE = ID_USER_PACKET_ENUM + 1,
		ID_PLAYER_MOVE = ID_USER_PACKET_ENUM + 2,
		ID_PLAYER_JUMP = ID_USER_PACKET_ENUM + 3
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
		ENTITY_MOVE(unsigned char _id, RakNet::NetworkID _nid, float _x, float _y, float _z)
		{
			useTimestamp = ID_TIMESTAMP;
			timeStamp = RakNet::GetTime();
			id = _id;
			nID = _nid;
			x = _x;
			y = _y;
			z = _z;
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
	

	class Packets
	{
	private:
	public:
		//always create a pair of constructor and destructor for each struct declared
		Packets(){}
		~Packets(){}
		//MAKE SURE TO CALL THE DESTROY FUNCTION OF THE TYPE AFTER YOU HAVE CREATED ONE AND SENT THE PACKET
		//ALSO THESE 'CONSTRUCTOR' AND'DESTRUCTOR' FUNCTIONS MUTS BE STATIC
		static std::tuple<void*, size_t> New_Game_Message(unsigned char _id);
		static void Destroy_Game_Message(void * ptr);


		static void REGISTER_TO_LUA();

	};

	

}
