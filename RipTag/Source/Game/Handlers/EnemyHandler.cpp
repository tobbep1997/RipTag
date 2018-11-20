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

		EnemyState state = currentGuard->getEnemyState();
		switch (state)
		{
		case Investigating_Sight:
			if (timer > 0.3f)
			{
				timer = 0.0f;
				_investigating(currentGuard);
			}
			std::cout << yellow << "Enemy State: Investigating Sight" << white << "\r";
			break;
		case Investigating_Sound:
			if (timer > 0.3f)
			{
				timer = 0.0f;
				_investigateSound(currentGuard);
			}
			std::cout << yellow << "Enemy State: Investigating Sound" << white << "\r";
			break;
		case High_Alert:
			_highAlert(currentGuard, deltaTime);
			std::cout << yellow << "Enemy State: High Alert" << white << "\r";
			break;
		case Patrolling:
			_patrolling(currentGuard);
			std::cout << yellow << "Enemy State: Patrolling" << white << "\r";
			break;
		case Suspicious:
			_suspicious(currentGuard, deltaTime);
			std::cout << yellow << "Enemy State: Suspicious" << white << "\r";
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
		DirectX::XMFLOAT4A guardPos = guard->getPosition();
		Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

		guard->SetAlertVector(m_grid->FindPath(guardTile, guard->GetCurrentPathNode()->tile));
		guard->setEnemeyState(High_Alert);
		std::cout << green << "Enemy Transition: Investigating Sight -> High Alert" << white << std::endl;
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
		DirectX::XMFLOAT4A guardPos = guard->getPosition();
		Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);
        //Might craaa
		guard->SetAlertVector(m_grid->FindPath(guardTile, guard->GetCurrentPathNode()->tile));
		std::cout << green << "Enemy Transition: Investigating Sound -> High Alert" << white << std::endl;
		guard->setEnemeyState(High_Alert);
	}
}

void EnemyHandler::_patrolling(Enemy * guard)
{
	if (guard->getVisCounter() >= ALERT_TIME_LIMIT || guard->getSoundLocation().percentage > SOUND_LEVEL) //"Huh?!" - Tim Allen
	{
		std::cout << green << "Enemy Transition: Patrolling -> Suspicious" << white << std::endl;
		guard->setEnemeyState(Suspicious);
		guard->setClearestPlayerLocation(DirectX::XMFLOAT4A(0, 0, 0, 1));
		guard->setLoudestSoundLocation(Enemy::SoundLocation());
		guard->setBiggestVisCounter(0);
		FMOD_VECTOR at = { guard->getPosition().x, guard->getPosition().y, guard->getPosition().z };
		AudioEngine::PlaySoundEffect(RipSounds::g_grunt, &at, AudioEngine::Enemy);
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
		if (guard->getBiggestVisCounter() >= ALERT_TIME_LIMIT*1.5)
			_alert(guard); //what was that?
		else if (guard->getLoudestSoundLocation().percentage > SOUND_LEVEL*1.5)
			_alert(guard, true); //what was that noise?
		else
		{
			guard->SetActTimer(0.0f);
			guard->setEnemeyState(Patrolling);
			std::cout << green << "Enemy Transition: Suspicious -> Patrolling" << white << std::endl;
			//Must have been nothing...
		}
	}
}

void EnemyHandler::_coolingDown(Enemy * guard, const double & dt)
{
	guard->AddActTimer(dt);
	if (guard->GetActTimer() > 2)
	{
		guard->SetActTimer(0.0f);
		guard->setEnemeyState(Patrolling);
	}
}
