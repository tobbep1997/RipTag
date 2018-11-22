#include "RipTagPCH.h"
#include "EnemyHandler.h"

EnemyHandler::EnemyHandler()
{
}


EnemyHandler::~EnemyHandler()
{
}

void EnemyHandler::Init(std::vector<Enemy*> enemies, Player * player, Grid * grid)
{
	m_guards = enemies;
	m_player = player;
	m_grid = grid;

	if (Network::Multiplayer::GetInstance()->isServer())
		m_type = 0;
	else if (Network::Multiplayer::GetInstance()->isClient())
		m_type = 1;
	else
		m_type = 2;

	_registerThisInstanceToNetwork();
	for (Enemy* var : m_guards)
	{
		var->setGrid(m_grid);
	}
}

void EnemyHandler::Update(float deltaTime)
{
	static double accumulatedTime = 0.0;
	static const double SEND_UPDATES_FREQUENCY = 1.0 / 50.0; // 20 ms

	if (m_type == 0)
	{
		_isServerUpdate(deltaTime);
		accumulatedTime += deltaTime;
		if (accumulatedTime >= SEND_UPDATES_FREQUENCY)
		{
			accumulatedTime -= SEND_UPDATES_FREQUENCY;
			for (auto enemy : m_guards)
				enemy->sendNetworkUpdate();
		}
	}
	else if (m_type == 1)
		_isClientUpdate(deltaTime);
	else if (m_type == 2)
		_isSinglePlayerUpdate(deltaTime);
	else
		return;
}

void EnemyHandler::HandlePacket(unsigned char id, unsigned char * data)
{
	switch (id)
	{
	case Network::ID_ENEMY_UPDATE:
		Network::ENEMYUPDATEPACKET * pData = (Network::ENEMYUPDATEPACKET*)data;
		//this is very unsafe
		this->m_guards[pData->uniqueID]->onNetworkUpdate(pData);
		break;
	}
}

void EnemyHandler::_isServerUpdate(double deltaTime)
{
	static float timer = 0.0f;
	timer += deltaTime;

	int playerVisibility = 0;
	int remotePlayerVisibility = 0;

	float soundPercentage = 0.0f;

	for (int i = 0; i < m_guards.size(); i++)
	{

		Enemy * currentGuard = m_guards.at(i);
		currentGuard->SetLenghtToPlayer(m_player->getPosition());
		currentGuard->Update(deltaTime);
		currentGuard->PhysicsUpdate(deltaTime);

		int tempVisibility = _getPlayerVisibility(currentGuard);
		int tempVisRemotePlayer = _getRemotePlayerVisibility(currentGuard);

		if (tempVisibility > playerVisibility)
			playerVisibility = tempVisibility;

		if (tempVisRemotePlayer > remotePlayerVisibility)
			remotePlayerVisibility = tempVisRemotePlayer;

		float tempSoundPercentage = currentGuard->getSoundLocation().percentage;
		if (tempSoundPercentage > soundPercentage)
		{
			soundPercentage = tempSoundPercentage;
		}
	}

	m_player->SetCurrentVisability(playerVisibility);
	m_player->SetCurrentSoundPercentage(soundPercentage);
	m_remotePlayer->SetVisibility(remotePlayerVisibility);
}

void EnemyHandler::_isClientUpdate(double deltaTime)
{
	static float timer = 0.0f;
	timer += deltaTime;
	float soundPercentage = 0.0f;

	for (int i = 0; i < m_guards.size(); i++)
	{
		Enemy * currentGuard = m_guards.at(i);
		currentGuard->SetLenghtToPlayer(m_player->getPosition());
		currentGuard->ClientUpdate(deltaTime);
		currentGuard->PhysicsUpdate(deltaTime);

		float tempSoundPercentage = currentGuard->getSoundLocation().percentage;
		if (tempSoundPercentage > soundPercentage)
		{
			soundPercentage = tempSoundPercentage;
		}
		
	}
	m_player->SetCurrentSoundPercentage(soundPercentage);
}

void EnemyHandler::_isSinglePlayerUpdate(double deltaTime)
{
	static float timer = 0.0f;
	timer += deltaTime;

	int playerVisibility = 0;
	float soundPercentage = 0.0f;

	for (int i = 0; i < m_guards.size(); i++)
	{
		Enemy * currentGuard = m_guards.at(i);
		currentGuard->SetLenghtToPlayer(m_player->getPosition());
		currentGuard->Update(deltaTime);
		currentGuard->PhysicsUpdate(deltaTime);

		int tempVisibility = _getPlayerVisibility(currentGuard);

		if (tempVisibility > playerVisibility)
			playerVisibility = tempVisibility;

		float tempSoundPercentage = currentGuard->getSoundLocation().percentage;
		if (tempSoundPercentage > soundPercentage)
		{
			soundPercentage = tempSoundPercentage;
		}

	}

	m_player->SetCurrentVisability(playerVisibility);
	m_player->SetCurrentSoundPercentage(soundPercentage);
}

void EnemyHandler::_registerThisInstanceToNetwork()
{
	using namespace Network;
	using namespace std::placeholders;

	Multiplayer::addToOnReceiveFuncMap(ID_ENEMY_UPDATE, std::bind(&EnemyHandler::HandlePacket, this, _1, _2));
}

int EnemyHandler::_getPlayerVisibility(Enemy * guard)
{
	guard->CullingForVisability(*m_player->getTransform());
	guard->QueueForVisibility();
	return guard->getPlayerVisibility()[0];
}

int EnemyHandler::_getRemotePlayerVisibility(Enemy * guard)
{
	guard->CullingForVisability(*m_remotePlayer->getTransform());
	guard->QueueForVisibility();
	return guard->getPlayerVisibility()[1];
}
