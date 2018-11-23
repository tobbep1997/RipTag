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

void EnemyHandler::setRemotePlayer(RemotePlayer * ptr)
{
	m_remotePlayer = ptr;
	for (auto & e : m_guards)
	{
		e->SetRemotePointer(m_remotePlayer);
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
		{
			Network::ENEMYUPDATEPACKET * pData = (Network::ENEMYUPDATEPACKET*)data;
			//this is very unsafe
			this->m_guards[pData->uniqueID]->onNetworkUpdate(pData);
		}
		break;
		case Network::ID_ENEMY_VISIBILITY:
		{
			Network::VISIBILITYPACKET * pData = (Network::VISIBILITYPACKET*)data;
			_onVisibilityPacket(pData);
		}
		break;
		case Network::ID_ENEMY_POSSESSED:
		{
			Network::ENTITYSTATEPACKET * pData = (Network::ENTITYSTATEPACKET*)data;
			_onPossessedPacket(pData);
		}
		break;
		case Network::ID_ENEMY_DISABLED:
		{
			Network::ENTITYSTATEPACKET * pData = (Network::ENTITYSTATEPACKET*)data;
			_onDisabledPacket(pData);
		}
		break;

	}
}

void EnemyHandler::_isServerUpdate(double deltaTime)
{
	static float timer = 0.0f;
	timer += deltaTime;

	int playerVisibility = 0;

	float soundPercentage = 0.0f;

	for (int i = 0; i < m_guards.size(); i++)
	{

		Enemy * currentGuard = m_guards.at(i);
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

void EnemyHandler::_isClientUpdate(double deltaTime)
{
	static float timer = 0.0f;
	timer += deltaTime;
	float soundPercentage = 0.0f;
	int playerVisibility = 0;
	for (int i = 0; i < m_guards.size(); i++)
	{
		Enemy * currentGuard = m_guards.at(i);
		//currentGuard->SetLenghtToPlayer(m_player->getPosition());
		currentGuard->ClientUpdate(deltaTime);
		currentGuard->PhysicsUpdate(deltaTime);

		int tempVisibility = _getPlayerVisibility(currentGuard);

		if (tempVisibility > playerVisibility)
			playerVisibility = tempVisibility;

		auto sl = currentGuard->getSoundLocation();
		float tempSoundPercentage = sl.percentage;
		
		Network::VISIBILITYPACKET packet;
		packet.visibilityValue = tempVisibility;
		packet.soundValue = tempSoundPercentage;
		packet.soundPos = sl.soundPos;
		packet.uniqueID = m_guards[i]->getUniqueID();
		
		Network::Multiplayer::SendPacket((const char *)&packet, sizeof(packet), PacketPriority::IMMEDIATE_PRIORITY);
		
		
		if (tempSoundPercentage > soundPercentage)
		{
			soundPercentage = tempSoundPercentage;
		}
		
	}

	m_player->SetCurrentVisability(playerVisibility);
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
	Multiplayer::addToOnReceiveFuncMap(ID_ENEMY_VISIBILITY, std::bind(&EnemyHandler::HandlePacket, this, _1, _2));
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

void EnemyHandler::_onVisibilityPacket(Network::VISIBILITYPACKET * data)
{
	//unsafe lol
	m_guards[data->uniqueID]->setCalculatedVisibilityFor(1, data->visibilityValue);
	m_guards[data->uniqueID]->setSoundLocationRemote({data->soundValue, data->soundPos}
	);
}

void EnemyHandler::_onPossessedPacket(Network::ENTITYSTATEPACKET * data)
{
	//state is their uniqueID
	m_guards[data->state]->onNetworkPossessed(data);
}

void EnemyHandler::_onDisabledPacket(Network::ENTITYSTATEPACKET * data)
{
	m_guards[data->state]->onNetworkDisabled(data);
}
