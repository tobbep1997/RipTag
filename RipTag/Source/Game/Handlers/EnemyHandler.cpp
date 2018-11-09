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
}

void EnemyHandler::Update(float deltaTime)
{
	int playerVisibility = 0;
	
	for (int i = 0; i < m_guards.size(); i++)
	{
		Enemy * currentGuard = m_guards.at(i);
		currentGuard->Update(deltaTime);
		currentGuard->PhysicsUpdate(deltaTime);

		int tempVisibility = _getPlayerVisibility(currentGuard);
		if (tempVisibility > playerVisibility)
			playerVisibility = tempVisibility;

		EnemyState state = currentGuard->getEnemyState();
		switch (state)
		{
		case Investigating_Sight:
			_investigating(currentGuard, tempVisibility);
			break;
		case Investigating_Sound:
			_investigateSound(currentGuard);
			break;
		case Patrolling:
			_patrolling(currentGuard, tempVisibility);
			break;
		}
	}

	m_player->SetCurrentVisability(playerVisibility);
}

int EnemyHandler::_getPlayerVisibility(Enemy * guard)
{
	guard->CullingForVisability(*m_player->getTransform());
	guard->QueueForVisibility();
	return guard->getPlayerVisibility()[0];
}

void EnemyHandler::_alert(Enemy * guard, bool followSound)
{
	if (!followSound)
	{
		DirectX::XMFLOAT4A playerPos = m_player->getPosition();
		DirectX::XMFLOAT4A guardPos = guard->getPosition();
		Tile playerTile = m_grid->WorldPosToTile(playerPos.x, playerPos.z);
		Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

		guard->SetAlertVector(m_grid->FindPath(guardTile, playerTile));
		guard->setEnemeyState(Investigating_Sight);
	}
	else
	{
		DirectX::XMFLOAT3 soundPos = guard->getSoundLocation().soundPos;
		DirectX::XMFLOAT4A guardPos = guard->getPosition();
		Tile soundTile = m_grid->WorldPosToTile(soundPos.x, soundPos.z);
		Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

		guard->SetAlertVector(m_grid->FindPath(guardTile, soundTile));
		guard->setEnemeyState(Investigating_Sound);
	}
}

void EnemyHandler::_investigating(Enemy * guard, int playerVisibility)
{
	if (guard->GetAlertPathSize() > 0)
	{
		DirectX::XMFLOAT4A playerPos = m_player->getPosition();
		Node * pathDestination = guard->GetAlertDestination();
		if (playerVisibility > 1700)
		{
			if (abs(pathDestination->worldPos.x - playerPos.x) > 5 ||
				abs(pathDestination->worldPos.y - playerPos.z) > 5)
			{
				DirectX::XMFLOAT4A guardPos = guard->getPosition();
				Tile playerTile = m_grid->WorldPosToTile(playerPos.x, playerPos.z);
				Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

				guard->SetAlertVector(m_grid->FindPath(guardTile, playerTile));
			}
		}
	}
	else
	{
		DirectX::XMFLOAT4A guardPos = guard->getPosition();
		Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

		guard->SetAlertVector(m_grid->FindPath(guardTile, guard->GetCurrentPathNode()->tile));
		guard->setEnemeyState(Patrolling);
	}
}

void EnemyHandler::_investigateSound(Enemy * guard)
{
	if (guard->GetAlertPathSize() > 0)
	{
		if (guard->getSoundLocation().percentage > SOUND_LEVEL)
		{
			DirectX::XMFLOAT3 soundPos = guard->getSoundLocation().soundPos;
			Node * pathDestination = guard->GetAlertDestination();
			if (abs(pathDestination->worldPos.x - soundPos.x) > 2 ||
				abs(pathDestination->worldPos.y - soundPos.z) > 2)
			{
				DirectX::XMFLOAT4A guardPos = guard->getPosition();
				Tile playerTile = m_grid->WorldPosToTile(soundPos.x, soundPos.z);
				Tile guardTile = m_grid->WorldPosToTile(soundPos.x, soundPos.z);

				guard->SetAlertVector(m_grid->FindPath(guardTile, playerTile));
			}
		}
	}
	else
	{
		DirectX::XMFLOAT4A guardPos = guard->getPosition();
		Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

		guard->SetAlertVector(m_grid->FindPath(guardTile, guard->GetCurrentPathNode()->tile));
		guard->setEnemeyState(Patrolling);
	}
}

void EnemyHandler::_patrolling(Enemy * guard, int playerVisibility)
{
	if (false && playerVisibility > SIGHT_LEVEL)
		_alert(guard);
	else if (guard->getSoundLocation().percentage > SOUND_LEVEL)
		_alert(guard, true);
}
