#include "PlayerManager.h"



PlayerManager::PlayerManager(b3World * physWorld)
{
	this->mWorld = physWorld;
}


PlayerManager::~PlayerManager()
{
}

void PlayerManager::RegisterThisInstanceToNetwork()
{
	using namespace Network;
	using namespace std::placeholders;
	//Send handling
	Multiplayer::addToOnSendFuncMap("RemotePlayerCreate", std::bind(&PlayerManager::SendOnPlayerCreate, this));

	//Receive handling
	Multiplayer::addToOnReceiveFuncMap(GAME_MESSAGES::ID_PLAYER_CREATE, std::bind(&PlayerManager::_onRemotePlayerCreate, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(GAME_MESSAGES::ID_PLAYER_MOVE, std::bind(&PlayerManager::_onRemotePlayerEvent, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(GAME_MESSAGES::ID_PLAYER_JUMP, std::bind(&PlayerManager::_onRemotePlayerEvent, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(GAME_MESSAGES::ID_PLAYER_DISCONNECT, std::bind(&PlayerManager::_onRemotePlayerDisconnect, this, _1, _2));
}

void PlayerManager::_onRemotePlayerCreate(unsigned char id, unsigned char * data)
{
	Network::ENTITY_CREATE * packet = (Network::ENTITY_CREATE*)data;
	if (!mRemotePlayer && !hasRemotePlayer)
	{
		this->mRemotePlayer = new RemotePlayer(*this->mWorld, packet->nID, packet->x, packet->y, packet->z);
		hasRemotePlayer = true;
	}
}

void PlayerManager::_onRemotePlayerEvent(unsigned char id, unsigned char * data)
{
	if (mRemotePlayer && hasRemotePlayer)
	{
		mRemotePlayer->HandlePacket(id, data);
	}
}

void PlayerManager::_onRemotePlayerDisconnect(unsigned id, unsigned char * data)
{
	if (mRemotePlayer && hasRemotePlayer)
	{
		delete mRemotePlayer;
		mRemotePlayer = nullptr;
		hasRemotePlayer = false;
	}
}

void PlayerManager::Update(float dt)
{
	if (mRemotePlayer && hasRemotePlayer)
		mRemotePlayer->Update(dt);
	if (mLocalPlayer && hasLocalPlayer)
		mLocalPlayer->Update(dt);
}

void PlayerManager::PhysicsUpdate()
{
	if (mRemotePlayer && hasRemotePlayer)
		mRemotePlayer->PhysicsUpdate();
	if (mLocalPlayer && hasLocalPlayer)
		mLocalPlayer->PhysicsUpdate();
}

void PlayerManager::Draw()
{
	if (mRemotePlayer && hasRemotePlayer)
		mRemotePlayer->Draw();
	if (mLocalPlayer && hasLocalPlayer)
		mLocalPlayer->Draw();
}

void PlayerManager::CreateLocalPlayer()
{
	if (!mLocalPlayer && !hasLocalPlayer)
	{
		mLocalPlayer = new Player();
		hasLocalPlayer = true;
		mLocalPlayer->RegisterThisInstanceToNetwork();
	}
}

void PlayerManager::SendOnPlayerCreate()
{
	if (mLocalPlayer && hasLocalPlayer)
	{
		DirectX::XMFLOAT4A pos = mLocalPlayer->getPosition();
		Network::ENTITY_CREATE packet(Network::GAME_MESSAGES::ID_PLAYER_CREATE,
			Network::Multiplayer::GetInstance()->GetNetworkID(),
			pos.x,
			pos.y,
			pos.z
		);

		Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::ENTITY_CREATE), PacketPriority::LOW_PRIORITY);
	}
}

Player * PlayerManager::getLocalPlayer()
{
	return mLocalPlayer;
}
