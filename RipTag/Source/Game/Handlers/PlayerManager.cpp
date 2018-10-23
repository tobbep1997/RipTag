#include "PlayerManager.h"



PlayerManager::PlayerManager()
{
}


PlayerManager::~PlayerManager()
{
}

void PlayerManager::RegisterThisInstanceToNetwork()
{
	using namespace Network;
	using namespace std::placeholders;

	Multiplayer::addToOnReceiveFuncMap(GAME_MESSAGES::ID_PLAYER_CREATE, std::bind(&PlayerManager::_onCommonPlayerEvent, this, _1, _2));

}

void PlayerManager::_onCommonPlayerEvent(unsigned char id, unsigned char * data)
{
}

void PlayerManager::_onRemotePlayerMove(unsigned char id, unsigned char * data)
{
}

void PlayerManager::Update(float dt)
{
}

void PlayerManager::Draw()
{
}
