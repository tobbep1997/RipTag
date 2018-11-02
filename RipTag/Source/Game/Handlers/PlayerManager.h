#pragma once
#include "..\Actors\Player.h"
#include "..\Actors\RemotePlayer.h"
#include "Multiplayer.h"

class PlayerManager
{
public:
	PlayerManager(b3World * physWorld);
	~PlayerManager();
private:
	//our physics world
	b3World * mWorld = 0;

	//since we only have co-op we only need tohandle a local and remote player
	Player * mLocalPlayer = 0;
	RemotePlayer * mRemotePlayer = 0;
	bool hasLocalPlayer = false;
	bool hasRemotePlayer = false;

public:
	//We need to register the PlayerManager instance to our Network Function Map 
	//This is so  we can handle RemotePlayer events directly on Packet Receive 
	void RegisterThisInstanceToNetwork();
	void _onRemotePlayerCreate(unsigned char id, unsigned char * data);
	void _onRemotePlayerDisconnect(unsigned char id, unsigned char * data);
	void _onRemotePlayerPacket(unsigned char id, unsigned char * data);

	void Update(float dt);
	void PhysicsUpdate();
	void Draw();

	//Local player handling
	void CreateLocalPlayer();
	void SendOnPlayerCreate();
	Player * getLocalPlayer();
};

