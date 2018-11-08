#include "RipTagPCH.h"
#include "EnemyHandler.h"


EnemyHandler::EnemyHandler()
{
}


EnemyHandler::~EnemyHandler()
{
}

void EnemyHandler::Init(std::vector<Enemy*> enemies, Grid * grid)
{
	m_guards = enemies;
	m_grid = grid;
}

void EnemyHandler::Update(float deltaTime)
{
	const int * playerVisibility;
	for (int i = 0; i < m_guards.size(); i++)
	{
		Enemy * currentGuard = m_guards.at(i);
		//currentGuard->Update(deltaTime);
		//currentGuard->PhysicsUpdate(deltaTime);
		playerVisibility = currentGuard->getPlayerVisibility();

		EnemyState state = currentGuard->getEnemyState();

		switch (state)
		{
		case Alert:

			break;
		case Investigating:

			break;
		case Patrolling:

			break;
		case Possessed:

			break;
		case Stunned:

			break;
		}
	}
}
