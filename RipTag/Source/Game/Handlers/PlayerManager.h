#pragma once
#include "..\Actors\Player.h"
#include "Multiplayer.h"

class PlayerManager
{
public:
	PlayerManager();
	~PlayerManager();
private:
	//since we only have co-op we only need tohandle a local and remote player
	Player * mLocalPlayer = 0;
	Player * mRemotePlayer = 0;

public:
	//We need to register the PlayerManager instance to our Network Function Map 
	//This is so  we can handle RemotePlayer events directly on Packet Receive 
	void RegisterThisInstanceToNetwork();
	void _onCommonPlayerEvent(unsigned char id, unsigned char * data);
	void _onRemotePlayerMove(unsigned char id, unsigned char * data);

	void Update(float dt);
	void Draw();

};

