#pragma once
#include "Multiplayer.h"

class Player;
class RemotePlayer;

class PlayerManager
{
public:
	PlayerManager(b3World * physWorld);
	PlayerManager(){}
	~PlayerManager();
	enum PlayerType
	{
		One = 1,
		Two = 2
	};
private:
	//our physics world
	b3World * mWorld = 0;

	//since we only have co-op we only need tohandle a local and remote player
	Player * mLocalPlayer = 0;
	RemotePlayer * mRemotePlayer = 0;
	bool hasLocalPlayer = false;
	bool hasRemotePlayer = false;
	int player_type = 0;

public:
	//We need to register the PlayerManager instance to our Network Function Map 
	//This is so  we can handle RemotePlayer events directly on Packet Receive 
	void RegisterThisInstanceToNetwork();
	void _onRemotePlayerCreate(unsigned char id, unsigned char * data);
	void _onRemotePlayerDisconnect(unsigned char id, unsigned char * data);
	void _onRemotePlayerPacket(unsigned char id, unsigned char * data);
	void _onRemotePlayerWonPacket(unsigned char id, unsigned char *data);

	void Init(b3World * physWorld);
	void Update(float dt);
	void PhysicsUpdate();
	void Draw();
	void isCoop(bool coop);
	//Local player handling
	void CreateLocalPlayer(DirectX::XMFLOAT4A pos = { 0.f, 0.f, 0.f, 0.f });
	//call this function when a client connects to you
	void CreateRemotePlayer(DirectX::XMFLOAT4A pos, RakNet::NetworkID nid);
	//call this function if you had a client connected but he left
	void DestroyRemotePlayer();
	Player * getLocalPlayer();
	RemotePlayer * getRemotePlayer();

	bool isGameWon();

	//use this function to set the type from char selection 
	void setPlayerType(int type) { player_type = type; }
	int getPlayerType() { return player_type; }
};

