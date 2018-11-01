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
		//PLAYER EVENTS
		ID_PLAYER_CREATE = ID_USER_PACKET_ENUM + 1,
		ID_PLAYER_DISCONNECT = ID_USER_PACKET_ENUM + 2,
		ID_PLAYER_UPDATE = ID_USER_PACKET_ENUM + 3,
		ID_PLAYER_STATE = ID_USER_PACKET_ENUM + 4,
		ID_PLAYER_ABILITY = ID_USER_PACKET_ENUM + 5,
		ID_PLAYER_ANIMATION = ID_USER_PACKET_ENUM + 6
	};


//STRUCTS BEGIN
#pragma pack(push, 1)
	struct EVENTPACKET
	{
		unsigned char id;
		EVENTPACKET(unsigned char _id) : id(_id) {}
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

	struct ENTITYABILITYPACKET
	{
		unsigned char id;
		RakNet::Time timeStamp;
		unsigned char m_id;
		unsigned int ability;
		XMFLOAT4A start;
		XMFLOAT4A velocity;
		unsigned int state;
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
		DirectX::XMFLOAT4A rot;
		ENTITYANIMATIONPACKET(unsigned char _id, RakNet::NetworkID _nid, float _dir, float _speed, DirectX::XMFLOAT4A _rot)
			: id(_id), nid(_nid), direction(_dir), speed(_speed), rot(_rot) {}
	};

#pragma pack(pop)
	//STRUCTS END
	

}
