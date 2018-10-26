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
		ID_GAME_START = ID_USER_PACKET_ENUM,
		ID_PLAYER_CREATE = ID_USER_PACKET_ENUM + 1,
		ID_PLAYER_DISCONNECT = ID_USER_PACKET_ENUM + 2,
		ID_PLAYER_UPDATE = ID_USER_PACKET_ENUM + 3
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

	struct PACKET
	{
		union
		{
			EVENTPACKET _event;
			CREATEPACKET _create;
			ENTITYUPDATEPACKET _update;
		};
	};

#pragma pack(pop)
	//STRUCTS END
	

}
