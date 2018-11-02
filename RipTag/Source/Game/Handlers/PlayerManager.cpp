#include "PlayerManager.h"
#include "../Engine/EngineSource/Debugg/ImGui/imgui.h"


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
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_DISCONNECT, std::bind(&PlayerManager::_onRemotePlayerDisconnect, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_CREATE, std::bind(&PlayerManager::_onRemotePlayerCreate, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_UPDATE, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_ABILITY, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_ANIMATION, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
}

void PlayerManager::_onRemotePlayerCreate(unsigned char id, unsigned char * data)
{
	Network::CREATEPACKET * packet = (Network::CREATEPACKET*)data;
	if (!mRemotePlayer && !hasRemotePlayer)
	{
		this->mRemotePlayer = new RemotePlayer(packet->nid, packet->pos, packet->scale, packet->rotation);
		hasRemotePlayer = true;
	}
}

void PlayerManager::_onRemotePlayerPacket(unsigned char id, unsigned char * data)
{
	if (mRemotePlayer && hasRemotePlayer)
	{
		mRemotePlayer->HandlePacket(id, data);
	}
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
		AudioEngine::UpdateListenerAttributes(mLocalPlayer->getFMODListener());
	}
	if (hasRemotePlayer && hasLocalPlayer)
	{
		if (accumulatedDT >= frequency)
		{
			accumulatedDT -= frequency;
			mLocalPlayer->SendOnUpdateMessage();
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
#if _DEBUG
	ImGui::Begin("possese");
	if (hasRemotePlayer)
	{
		ImGui::Text("X: %f", mRemotePlayer->getPosition().x);
		ImGui::Text("Y: %f", mRemotePlayer->getPosition().y);
		ImGui::Text("Z: %f", mRemotePlayer->getPosition().z);
	}
	ImGui::Text("--------");
	if (hasLocalPlayer)
	{
		ImGui::Text("X: %f", mLocalPlayer->getPosition().x);
		ImGui::Text("Y: %f", mLocalPlayer->getPosition().y);
		ImGui::Text("Z: %f", mLocalPlayer->getPosition().z);
	}
	ImGui::End();
#endif
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
		DirectX::XMFLOAT4A scale = DirectX::XMFLOAT4A(0.015f, 0.015f, 0.015f, 1.0f);
		DirectX::XMFLOAT4A rot = {0.0, DirectX::XM_PI, 0.0, 0.0};

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
