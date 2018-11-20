#pragma once
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <string>
#include <GetTime.h>
#include <BitStream.h>
#include <RakNetDefines.h>
#include <DirectXMath.h>



namespace Network
{
	using namespace DirectX;

	enum NETWORKMESSAGES
	{
		//GAME EVENTS
		ID_GAME_START = ID_USER_PACKET_ENUM,
		//PLAYER EVENTS 1-19
		ID_PLAYER_CREATE = ID_USER_PACKET_ENUM + 1,
		ID_PLAYER_DISCONNECT = ID_USER_PACKET_ENUM + 2,
		ID_PLAYER_UPDATE = ID_USER_PACKET_ENUM + 3,
		ID_PLAYER_STATE = ID_USER_PACKET_ENUM + 4,
		ID_PLAYER_ABILITY = ID_USER_PACKET_ENUM + 5,
		ID_PLAYER_ANIMATION = ID_USER_PACKET_ENUM + 6,
		// 20-29 is reserved for lobby
		ID_SERVER_ADVERTISE = ID_USER_PACKET_ENUM + 20,
		ID_CHAR_SELECTED = ID_USER_PACKET_ENUM + 21,
		ID_READY_PRESSED = ID_USER_PACKET_ENUM + 22,
		ID_REQUEST_NID = ID_USER_PACKET_ENUM + 23,
		ID_REPLY_NID = ID_USER_PACKET_ENUM + 24,
		ID_REQUEST_SELECTED_CHAR = ID_USER_PACKET_ENUM + 25,
		ID_HOST_NAME = ID_USER_PACKET_ENUM + 26,

		//this packet is sent by the server, contains a random seed
		ID_GAME_STARTED = ID_USER_PACKET_ENUM + 29,
		//GAMEPLAY/GAMESTATE EVENTS 30-49
		ID_TRIGGER_USED = ID_USER_PACKET_ENUM + 30,
		ID_PLAYER_WON = ID_USER_PACKET_ENUM + 31,
		ID_PLAYER_LOST = ID_USER_PACKET_ENUM + 32,

	};


//STRUCTS BEGIN
#pragma pack(push, 1)
	struct COMMONEVENTPACKET
	{
		unsigned char id;
		RakNet::NetworkID nid;
		COMMONEVENTPACKET(unsigned char _id, RakNet::NetworkID _nid = 0) : id(_id), nid(_nid) {}
	};

	struct LOBBYEVENTPACKET
	{
		unsigned char id;
		char string[32];
		LOBBYEVENTPACKET(){}
		LOBBYEVENTPACKET(unsigned char _id, std::string str = "")
		{
			id = _id;
			if (str.size() > 32)
				str = str.substr(0, 32);
			strcpy(string, str.c_str());
		}
	};

	struct CREATEPACKET
	{
		unsigned char id; //variable name must be the same for all structs
		RakNet::Time timeStamp;
		unsigned char m_id; //this is our identifier
		RakNet::NetworkID nid;
		XMFLOAT4A pos;
		XMFLOAT4A scale;
		XMFLOAT4A rotation;
		CREATEPACKET(unsigned char _id, RakNet::NetworkID _nid, XMFLOAT4A _pos, XMFLOAT4A _scale, XMFLOAT4A _rot) 
			: id(DefaultMessageIDTypes::ID_TIMESTAMP), timeStamp(RakNet::GetTime()), m_id(_id), nid(_nid), pos(_pos), scale(_scale), rotation(_rot) {}
	};

	struct ENTITYUPDATEPACKET
	{
		unsigned char id;
		RakNet::Time timeStamp;
		unsigned char m_id;
		RakNet::NetworkID nid;
		unsigned int state;
		XMFLOAT4A pos;
		XMFLOAT4A rot;
		ENTITYUPDATEPACKET(unsigned char _id, RakNet::NetworkID _nid, unsigned int _state, XMFLOAT4A _pos, XMFLOAT4A _rot)
			: id(DefaultMessageIDTypes::ID_TIMESTAMP), timeStamp(RakNet::GetTime()), m_id(_id), nid(_nid), state(_state), pos(_pos), rot(_rot) {}
	};

	struct ENTITYSTATEPACKET
	{
		unsigned char id;
		unsigned int state;
		bool condition;
		ENTITYSTATEPACKET(unsigned char _id, unsigned int _state, bool _condition) : id(_id), state(_state), condition(_condition) {}
	};

	struct ENEMYAIUPDATEPACKET
	{
		unsigned char id;
		unsigned int state;
		//WHAT DATA? 
	
	};

	struct ENTITYABILITYPACKET
	{
		unsigned char id;
		RakNet::Time timeStamp;
		unsigned char m_id;
		unsigned int ability;
		XMFLOAT4A start;
		XMFLOAT4A velocity;
		unsigned int state;
		bool isCommonUpadate;
		ENTITYABILITYPACKET() {}
		ENTITYABILITYPACKET(unsigned char _id, unsigned int _ability, XMFLOAT4A _start, XMFLOAT4A _vel, unsigned int _state) 
			: id(ID_TIMESTAMP), timeStamp(RakNet::GetTime()), m_id(_id), ability(_ability), start(_start), velocity(_vel), state(_state) {}
	};

	struct ENTITYANIMATIONPACKET
	{
		unsigned char id;
		RakNet::NetworkID nid;
		float direction;
		float speed;
		float pitch;
		DirectX::XMFLOAT4A rot;
		ENTITYANIMATIONPACKET(unsigned char _id, RakNet::NetworkID _nid, float _dir, float _speed, float _pitch, DirectX::XMFLOAT4A _rot)
			: id(_id), nid(_nid), direction(_dir), speed(_speed), pitch(_pitch), rot(_rot) {}
	};

	struct TRIGGEREVENTPACKET
	{
		unsigned char id;
		int uniqueID;
		bool state;
		TRIGGEREVENTPACKET(unsigned char _id, int _uID, bool _state)
			: id(_id), uniqueID(_uID), state(_state) {}
	};

	struct CHARACTERSELECTIONPACKET
	{
		unsigned char id;
		int selectedChar;
		//either server or client
		int role;
	};

	struct GAMESTARTEDPACKET
	{
		unsigned char id;
		int seed;
		RakNet::NetworkID remoteID;
		GAMESTARTEDPACKET(unsigned char _id, int _seed, RakNet::NetworkID nid) : id(_id), seed(_seed), remoteID(nid) {}
	};
#pragma pack(pop)
	//STRUCTS END
	

}
