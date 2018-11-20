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
	
	for (int i = 0; i < m_guards.size(); i++)
	{
		Enemy * currentGuard = m_guards.at(i);
		currentGuard->SetLenghtToPlayer(m_player->getPosition());
		currentGuard->Update(deltaTime);
		currentGuard->PhysicsUpdate(deltaTime);

		int tempVisibility = _getPlayerVisibility(currentGuard);
		if (tempVisibility > playerVisibility)
			playerVisibility = tempVisibility;

		//EnemyTransitionState transState = currentGuard->getTransitionState();
		//switch (transState)
		//{
		//case EnemyTransitionState::None:
		//	break;
		//case EnemyTransitionState::Alerted:
		//	currentGuard->onAlerted();
		//	break;
		//case EnemyTransitionState::InvestigateSound:
		//	currentGuard->onInvestigateSound(m_grid);
		//	break;
		//case EnemyTransitionState::InvestigateSight:
		//	currentGuard->onInvestigateSight(m_grid);
		//	break;
		//case EnemyTransitionState::Observe:
		//	currentGuard->onObserve(m_grid);
		//	break;
		//case EnemyTransitionState::SearchArea:
		//	currentGuard->onSearchArea(m_grid);
		//	break;
		//case EnemyTransitionState::ReturnToPatrol:
		//	currentGuard->onReturnToPatrol();
		//	break;
		//case EnemyTransitionState::BeingPossessed:
		//	currentGuard->onBeingPossessed();
		//	break;
		//case EnemyTransitionState::BeingDisabled:
		//	currentGuard->onBeingDisabled();
		//	break;
		//}

		//EnemyState state = currentGuard->getEnemyState();
		//switch (state)
		//{
		//case Investigating_Sight:
		//	if (timer > 0.3f)
		//	{
		//		timer = 0.0f;
		//		//_investigating(currentGuard);
		//		currentGuard->investigatingSight(m_grid);
		//	}
		//	std::cout << yellow << "Enemy State: Investigating Sight" << white << "\r";
		//	break;
		//case Investigating_Sound:
		//	if (timer > 0.3f)
		//	{
		//		timer = 0.0f;
		//		//_investigateSound(currentGuard);
		//		currentGuard->investigatingSound(m_grid);
		//	}
		//	std::cout << yellow << "Enemy State: Investigating Sound" << white << "\r";
		//	break;
		//case Investigating_Room:
		//	if (timer > 0.3f)
		//	{
		//		std::cout << yellow << "Enemy State: Investigating Room" << white << "\r";
		//		//_investigateRoom(currentGuard, timer);
		//		currentGuard->investigatingRoom(m_grid, timer);
		//		timer = 0.0f;
		//	}
		//	break;
		//case High_Alert:
		//	//_highAlert(currentGuard, deltaTime);
		//	currentGuard->highAlert(deltaTime);
		//	std::cout << yellow << "Enemy State: High Alert" << white << "\r";
		//	break;
		//case Patrolling:
		//	//_patrolling(currentGuard);
		//	currentGuard->patrolling();
		//	std::cout << yellow << "Enemy State: Patrolling" << white << "\r";
		//	break;
		//case Suspicious:
		//	//_suspicious(currentGuard, deltaTime);
		//	currentGuard->suspicious(deltaTime);
		//	std::cout << yellow << "Enemy State: Suspicious" << white << "\r";
		//	break;
		//case Scanning_Area:
		//	//_ScanArea(currentGuard, deltaTime);
		//	currentGuard->scanningArea(deltaTime);
		//	std::cout << yellow << "Enemy State: Scanning Area" << white << "\r";
		//	break;
		//}
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
		DirectX::XMFLOAT4A playerPos = guard->getClearestPlayerLocation();
		DirectX::XMFLOAT4A guardPos = guard->getPosition();
		Tile playerTile = m_grid->WorldPosToTile(playerPos.x, playerPos.z);
		Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

		guard->SetAlertVector(m_grid->FindPath(guardTile, playerTile));
		guard->setEnemeyState(Investigating_Sight);
		std::cout << green << "Enemy Transition: Suspicious -> Investigate Sight" << white << std::endl;

	}
	else
	{
		DirectX::XMFLOAT3 soundPos = guard->getLoudestSoundLocation().soundPos;
		DirectX::XMFLOAT4A guardPos = guard->getPosition();
		Tile soundTile = m_grid->WorldPosToTile(soundPos.x, soundPos.z);
		Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

		guard->SetAlertVector(m_grid->FindPath(guardTile, soundTile));
		guard->setEnemeyState(Investigating_Sound);
		std::cout << green << "Enemy Transition: Suspicious -> Investigate Sound" << white << std::endl;
	}
}

void EnemyHandler::_investigating(Enemy * guard)
{
	if (guard->GetAlertPathSize() > 0)
	{
		if (guard->getVisCounter() > guard->getBiggestVisCounter())
		{
			DirectX::XMFLOAT4A playerPos = m_player->getPosition();
			Node * pathDestination = guard->GetAlertDestination();

			if (abs(pathDestination->worldPos.x - playerPos.x) > 5.0f ||
				abs(pathDestination->worldPos.y - playerPos.z) > 5.0f)
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
		guard->setTransitionState(EnemyTransitionState::Observe);
	}
}

void EnemyHandler::_investigateSound(Enemy * guard)
{
	if (guard->GetAlertPathSize() > 0)
	{
		if (guard->getSoundLocation().percentage > guard->getLoudestSoundLocation().percentage)
		{
			DirectX::XMFLOAT3 soundPos = guard->getSoundLocation().soundPos;
			Node * pathDestination = guard->GetAlertDestination();

			if (abs(pathDestination->worldPos.x - soundPos.x) > 2.0f ||
				abs(pathDestination->worldPos.y - soundPos.z) > 2.0f)
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
		guard->setTransitionState(EnemyTransitionState::Observe);
	}
}

void EnemyHandler::_patrolling(Enemy * guard)
{
	if (guard->getVisCounter() >= ALERT_TIME_LIMIT || guard->getSoundLocation().percentage > SOUND_LEVEL) //"Huh?!" - Tim Allen
	{
		guard->setTransitionState(EnemyTransitionState::Alerted);
	}
}

void EnemyHandler::_highAlert(Enemy* guard, const double & dt)
{
	guard->AddHighAlertTimer(dt);
	if (guard->GetHighAlertTimer() >= HIGH_ALERT_LIMIT)
	{
		guard->SetHightAlertTimer(0.f);
		guard->setClearestPlayerLocation(DirectX::XMFLOAT4A(0, 0, 0, 1));
		guard->setLoudestSoundLocation(Enemy::SoundLocation());
		guard->setBiggestVisCounter(0);
		guard->setEnemeyState(Suspicious);
		std::cout << green << "Enemy Transition: High Alert -> Suspicious" << white << std::endl;
	}
}

void EnemyHandler::_suspicious(Enemy * guard, const double & dt)
{
	guard->AddActTimer(dt);
	float attentionMultiplier = 1.0f; // TEMP will be moved to Enemy
	if (guard->GetActTimer() > SUSPICIOUS_TIME_LIMIT / 3)
	{
		attentionMultiplier = 1.2f;
	}
	if (guard->getVisCounter()*attentionMultiplier >= guard->getBiggestVisCounter())
	{
		guard->setClearestPlayerLocation(m_player->getPosition());
		guard->setBiggestVisCounter(guard->getVisCounter()*attentionMultiplier);
		/*b3Vec3 dir(guard->getPosition().x - m_player->getPosition().x, guard->getPosition().y - m_player->getPosition().y, guard->getPosition().z - m_player->getPosition().z);
		b3Normalize(dir);
		guard->setDir(dir.x, dir.y, dir.y);*/
	}
	if (guard->getSoundLocation().percentage*attentionMultiplier >= guard->getLoudestSoundLocation().percentage)
	{
		Enemy::SoundLocation temp = guard->getSoundLocation();
		temp.percentage *= attentionMultiplier;
		guard->setLoudestSoundLocation(temp);
		/*b3Vec3 dir(guard->getPosition().x - guard->getLoudestSoundLocation().soundPos.x, guard->getPosition().y - guard->getLoudestSoundLocation().soundPos.y, guard->getPosition().z - guard->getLoudestSoundLocation().soundPos.z);
		b3Normalize(dir);
		guard->setDir(dir.x, dir.y, dir.y);*/
	}
	if (guard->GetActTimer() > SUSPICIOUS_TIME_LIMIT)
	{
		guard->SetActTimer(0.0f);
		if (guard->getBiggestVisCounter() >= ALERT_TIME_LIMIT * 1.5)
			guard->setTransitionState(EnemyTransitionState::InvestigateSight); //what was that?
		else if (guard->getLoudestSoundLocation().percentage > SOUND_LEVEL*1.5)
			guard->setTransitionState(EnemyTransitionState::InvestigateSound); //what was that noise?
		else
		{
			guard->setTransitionState(EnemyTransitionState::ReturnToPatrol);
			//Must have been nothing...
		}
	}
}

void EnemyHandler::_ScanArea(Enemy * guard, const double & dt) //Look around
{
	guard->AddActTimer(dt);
	//Do animation
	if (guard->GetActTimer() > SUSPICIOUS_TIME_LIMIT)
	{	
		guard->setTransitionState(EnemyTransitionState::SearchArea);
	}
}

void EnemyHandler::_investigateRoom(Enemy * guard, const double & dt) //search around room (high Alert?)
{
	guard->AddActTimer(dt);
	static int lastSearchDirX = 0;
	static int lastSearchDirY = 0;
	int random = dt;
	srand(random);

	if (guard->GetAlertPathSize() == 0)
	{
		int dirX = (rand() % 3) - 1;
		random += dt*100;
		srand(random);
		int dirY = (rand() % 3) - 1;

		if (lastSearchDirX == 0 && lastSearchDirY == 0)
		{

		}
		else
		{
			while ((dirX == 0 && dirY == 0) || (dirX == lastSearchDirX && dirY == lastSearchDirY) || (dirX == -lastSearchDirX && dirY == -lastSearchDirY))
			{
				random += dt * 100;
				srand(random);
				dirX = (rand() % 3) - 1;
				random += dt * 100;
				srand(random);
				dirY = (rand() % 3) - 1;

				/*float dy = dirY - lastSearchDirY;
				float dx = dirX - lastSearchDirX;
				float theta = std::atan2(dy, dx);
				theta *= 180 / B3_PI;
				if (abs(theta) > 25)
				{
				}
				else
				{
					dirX = 0;
					dirY = 0;
				}*/
			}
		}

		
		random += dt;
		srand(random);
		int searchLength = (rand() % 4) + 4;
		DirectX::XMFLOAT4A guardPos = guard->getPosition();
		Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);
		Tile newPos;
		for (int i = 1; i < searchLength; i++)
		{
			newPos = m_grid->WorldPosToTile(guardPos.x + (dirX*i), guardPos.z + (dirY*i));
			if (!newPos.getPathable())
				break;
		}

		if (newPos.getPathable())
		{
			lastSearchDirX = dirX;
			lastSearchDirY = dirY;
			guard->SetAlertVector(m_grid->FindPath(guardTile, newPos));
		}

		//guard->SetAlertVector(m_grid->FindPath(guardTile, randPos));
	}
	if (guard->getVisCounter() >= ALERT_TIME_LIMIT || guard->getSoundLocation().percentage > SOUND_LEVEL)
	{
		guard->setTransitionState(EnemyTransitionState::Alerted);
	}
	if (guard->GetActTimer() > SEARCH_ROOM_TIME_LIMIT)
	{
		guard->setTransitionState(EnemyTransitionState::ReturnToPatrol);
	}
}


/*float dirX = (((float)rand() / (float)RAND_MAX) *2) -1;
		dirX = std::round(dirX * 10.0) / 10.0;
		dirX = floor((dirX * 2) + 0.5) / 2;
		
		random += dt;
		srand(random);
		float dirY = (((float)rand() / (float)RAND_MAX) * 2) - 1;
		dirY = std::round(dirY * 10.0) / 10.0;
		dirY = floor((dirY * 2) + 0.5) / 2;

		while(dirX == -lastSearchDirX && dirY == -lastSearchDirY)
		{
			random += dt;
			srand(random);
			dirX = (((float)rand() / (float)RAND_MAX) * 2) - 1;
			dirX = std::round(dirX * 10.0) / 10.0;
			dirX = floor((dirX * 2) + 0.5) / 2;

			random += dt;
			srand(random);
			dirY = (((float)rand() / (float)RAND_MAX) * 2) - 1;
			dirY = std::round(dirY * 10.0) / 10.0;
			dirY = floor((dirY * 2) + 0.5) / 2;
		}
		random += dt;
		srand(random);*/