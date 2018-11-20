#include "RipTagPCH.h"
#include "PlayerManager.h"



PlayerManager::PlayerManager(b3World * physWorld)
{
	this->mWorld = physWorld;
}


PlayerManager::~PlayerManager()
{
	if (mRemotePlayer)
	{
		Network::Multiplayer::RemotePlayerOnReceiveMap.clear();
		delete mRemotePlayer;
	}
	if (mLocalPlayer)
	{
		Network::Multiplayer::LocalPlayerOnSendMap.clear();
		delete mLocalPlayer;
	}
}

void PlayerManager::RegisterThisInstanceToNetwork()
{
	using namespace Network;
	using namespace std::placeholders;
	//Send handling
	Multiplayer::addToOnSendFuncMap("RemotePlayerCreate", std::bind(&PlayerManager::SendOnPlayerCreate, this));

	//Receive handling
	//Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_DISCONNECT, std::bind(&PlayerManager::_onRemotePlayerDisconnect, this, _1, _2));
	//Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_CREATE, std::bind(&PlayerManager::_onRemotePlayerCreate, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_UPDATE, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_ABILITY, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_ANIMATION, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_WON, std::bind(&PlayerManager::_onRemotePlayerWonPacket, this, _1, _2));
}

void PlayerManager::_onRemotePlayerCreate(unsigned char id, unsigned char * data)
{
	Network::CREATEPACKET * packet = (Network::CREATEPACKET*)data;
	if (!mRemotePlayer && !hasRemotePlayer)
	{
		this->mRemotePlayer = new RemotePlayer(packet->nid, packet->pos, packet->scale, packet->rotation);
		hasRemotePlayer = true;
		if (Network::Multiplayer::GetInstance()->isServer())
		{
			this->mLocalPlayer->SetAbilitySet(1);
			this->mRemotePlayer->SetAbilitySet(2);
		}
		else
		{
			this->mLocalPlayer->SetAbilitySet(2);
			this->mRemotePlayer->SetAbilitySet(1);
		}
	}
}

void PlayerManager::_onRemotePlayerPacket(unsigned char id, unsigned char * data)
{
	if (mRemotePlayer && hasRemotePlayer)
	{
		mRemotePlayer->HandlePacket(id, data);
	}
}

void PlayerManager::_onRemotePlayerWonPacket(unsigned char id, unsigned char * data)
{
	Network::ENTITYSTATEPACKET * pData = (Network::ENTITYSTATEPACKET*)data;
	mRemotePlayer->hasWon = pData->condition;
}

void PlayerManager::_onRemotePlayerDisconnect(unsigned char id, unsigned char * data)
{
	if (mRemotePlayer && hasRemotePlayer)
	{
		delete mRemotePlayer;
		mRemotePlayer = nullptr;
		hasRemotePlayer = false;
	}
}


void PlayerManager::Init(b3World * physWorld)
{
	this->mWorld = physWorld;
}

void PlayerManager::Update(float dt)
{
	static float accumulatedDT = 0;
	static const float frequency = 1.0f / 25.f; //one update each 40 ms

	accumulatedDT += dt;

	if (mRemotePlayer && hasRemotePlayer)
		mRemotePlayer->Update(dt);
	if (mLocalPlayer && hasLocalPlayer)
	{
		mLocalPlayer->Update(dt);
		const AudioEngine::Listener & pl = mLocalPlayer->getFMODListener();
		AudioEngine::UpdateListenerAttributes(pl);
	}
	if (hasRemotePlayer && hasLocalPlayer)
	{
		if (accumulatedDT >= frequency)
		{
			accumulatedDT -= frequency;
			mLocalPlayer->SendOnUpdateMessage();
			mLocalPlayer->SendAbilityUpdates();
		}
		mLocalPlayer->SendOnAnimationUpdate(dt);
	}

}

void PlayerManager::PhysicsUpdate()
{
	if (mLocalPlayer && hasLocalPlayer)
		mLocalPlayer->PhysicsUpdate();
}

void PlayerManager::Draw()
{
	if (mRemotePlayer && hasRemotePlayer)
	{
		mRemotePlayer->Draw();
		mRemotePlayer->getPosition();
	}
	if (mLocalPlayer && hasLocalPlayer)
		mLocalPlayer->Draw();
}

void PlayerManager::isCoop(bool coop)
{
	if (mLocalPlayer && coop)
		mLocalPlayer->RegisterThisInstanceToNetwork();
	if (mRemotePlayer && coop)
		this->RegisterThisInstanceToNetwork();
}

void PlayerManager::CreateLocalPlayer(DirectX::XMFLOAT4A pos)
{
	if (!mLocalPlayer)
	{
		hasLocalPlayer = true;
		mLocalPlayer = new Player();
		mLocalPlayer->Init(*this->mWorld, e_dynamicBody, 0.5f, 0.9f, 0.5f);
		mLocalPlayer->setEntityType(EntityType::PlayerType);
		mLocalPlayer->setColor(1.f, 1.f, 1.f, 1.f);
		mLocalPlayer->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
		mLocalPlayer->setScale(1.0f, 1.0f, 1.0f);
		mLocalPlayer->setPosition(0.0f, 0.0f, 0.0f);
		mLocalPlayer->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
		mLocalPlayer->setTextureTileMult(2, 2);
	}
}

void PlayerManager::CreateRemotePlayer(DirectX::XMFLOAT4A pos, RakNet::NetworkID nid)
{
	if (!hasRemotePlayer)
	{
		DirectX::XMFLOAT4A scale = DirectX::XMFLOAT4A(0.015f, 0.015f, 0.015f, 1.0f);
		DirectX::XMFLOAT4A rot = { 0.0, 0.0, 0.0, 0.0 };
		this->mRemotePlayer = new RemotePlayer(nid, pos, scale, rot);
		this->mRemotePlayer->setEntityType(EntityType::RemotePlayerType);
		hasRemotePlayer = true;
	}
}

void PlayerManager::DestroyRemotePlayer()
{
	if (mRemotePlayer)
		delete mRemotePlayer;
	mRemotePlayer = nullptr;
	hasRemotePlayer = false;
}

void PlayerManager::SendOnPlayerCreate()
{
	if (mLocalPlayer && hasLocalPlayer)
	{
		DirectX::XMFLOAT4A pos = mLocalPlayer->getPosition();
		DirectX::XMFLOAT4A scale = DirectX::XMFLOAT4A(0.015f, 0.015f, 0.015f, 1.0f);
		DirectX::XMFLOAT4A rot = {0.0, 0.0, 0.0, 0.0};

		Network::CREATEPACKET packet(Network::NETWORKMESSAGES::ID_PLAYER_CREATE,
			Network::Multiplayer::GetInstance()->GetNetworkID(),
			pos,
			scale,
			rot
		);

		Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), PacketPriority::LOW_PRIORITY);
	}
}

Player * PlayerManager::getLocalPlayer()
{
	return mLocalPlayer;
}

RemotePlayer * PlayerManager::getRemotePlayer()
{
	return this->mRemotePlayer;
}

bool PlayerManager::isGameWon()
{
	if (hasLocalPlayer && hasRemotePlayer)
		return (mLocalPlayer->getWinState() && mRemotePlayer->hasWon);
	else
		return mLocalPlayer->getWinState();
}
