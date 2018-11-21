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
	for (Enemy* var : m_guards)
	{
		var->setGrid(m_grid);
	}
}

void EnemyHandler::Update(float deltaTime)
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
	}

	m_player->SetCurrentVisability(playerVisibility);
	m_player->SetCurrentSoundPercentage(soundPercentage);
}

int EnemyHandler::_getPlayerVisibility(Enemy * guard)
{
	guard->CullingForVisability(*m_player->getTransform());
	guard->QueueForVisibility();
	return guard->getPlayerVisibility()[0];
}

