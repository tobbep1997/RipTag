#include "RipTagPCH.h"
#include "AI.h"

AI::AI()
{
	m_alerted.owner = nullptr;
	m_alerted.emitter = AudioEngine::Enemy;
	m_alerted.loudness = 1.5f;
}

AI::AI(Enemy * owner)
{
	m_owner = owner;
	m_alerted.owner = owner;
	m_alerted.emitter = AudioEngine::Enemy;
	m_alerted.loudness = 1.5f;
}

AI::~AI()
{
	m_owner = nullptr;

	for (int i = 0; i < m_path.size(); i++)
	{
		delete m_path.at(i);
	}
	m_path.clear();
	for (int i = 0; i < m_alertPath.size(); i++)
	{
		delete m_alertPath.at(i);
	}
	m_alertPath.clear();
}

void AI::handleStates(const double deltaTime)
{
	static float timer = 0.0f;
	timer += deltaTime;

	switch (m_transState)
	{
	case AITransitionState::NoTransitionState:
		break;
	case AITransitionState::Alerted:
		this->_onAlerted();
		break;
	case AITransitionState::Investigate:
		this->_onInvestigate();
		break;
	case AITransitionState::Observe:
		this->_onObserve();
		break;
	case AITransitionState::ReturnToPatrol:
		this->_onReturnToPatrol();
		break;
	case AITransitionState::BeingPossessed:
		this->_onBeingPossessed();
		break;
	case AITransitionState::BeingDisabled:
		this->_onBeingDisabled();
		break;
	case AITransitionState::ExitingPossess:
		this->_onExitingPossessed();
		break;
	case AITransitionState::ExitingDisable:
		this->_onExitingDisabled();
		break;
	case AITransitionState::TorchFound:
		this->_onTorchFound();
		break;
	case AITransitionState::TorchHandled:
		this->_onTorchHandled();
		break;
	}

	switch (m_state)
	{
	case AIState::Investigating:
		m_owner->_CheckPlayer(deltaTime);
		if (timer > 0.3f)
		{
			timer = 0.0f;
			this->_investigating(deltaTime);
		}
		if (m_transState == AITransitionState::NoTransitionState)
		if (m_alertPath.size() != 0)
		{
			_MoveToAlert(m_alertPath.at(0), deltaTime);
		}
		m_owner->_detectTeleportSphere();
		break;
	case AIState::Patrolling:
		this->_patrolling(deltaTime);
		m_owner->_detectTeleportSphere();
		break;
	case AIState::Suspicious:
		this->_suspicious(deltaTime);
		m_owner->_detectTeleportSphere();
		break;
	case AIState::Possessed:
		_possessed(deltaTime);
		break;
	case AIState::Disabled:
		_disabled(deltaTime);
		break;
	case AIState::TorchHandling:
		_torchHandling(deltaTime);
		break;
	}
}
void AI::handleStatesClient(const double deltaTime)
{
	switch (m_transState)
	{
	case AITransitionState::BeingDisabled:
		_onBeingDisabled();
		break;
	case AITransitionState::ExitingDisable:
		_onExitingDisabled();
		break;
	case AITransitionState::BeingPossessed:
		_onBeingPossessed();
		break;
	case AITransitionState::ExitingPossess:
		_onExitingPossessed();
		break;
	}

	switch (m_state)
	{
	case AIState::Disabled:
		_disabled(deltaTime);
		break;
	case AIState::Possessed:
		_possessed(deltaTime);
		break;
	}
}

void AI::_onAlerted()
{
	//Set animation and send 
	{
		m_owner->getAnimationPlayer()->GetStateMachine()->SetState("aware");
		if (Network::Multiplayer::GetInstance()->isConnected())
		{
			Network::ENEMYANIMATIONSTATEPACKET packet(m_owner->getUniqueID(), "aware");
			Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::ENEMYANIMATIONSTATEPACKET), PacketPriority::LOW_PRIORITY);
		}
	}

	m_owner->setLiniearVelocity(0.0f, m_owner->getLiniearVelocity().y, 0.0f);
	this->m_state = AIState::Suspicious;
	this->m_actTimer = 0;

	FMOD_VECTOR at = { m_owner->getPosition().x, m_owner->getPosition().y, m_owner->getPosition().z };
	AudioEngine::PlaySoundEffect(RipSounds::g_grunt, &at, &m_alerted);

	this->m_transState = AITransitionState::NoTransitionState;
}
void AI::_onInvestigate()
{
	//Set animation and send to client
	{
		m_owner->getAnimationPlayer()->GetStateMachine()->SetState("walk_state");
		if (Network::Multiplayer::GetInstance()->isConnected())
		{
			Network::ENEMYANIMATIONSTATEPACKET packet(m_owner->getUniqueID(), "walk_state");
			Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::ENEMYANIMATIONSTATEPACKET), PacketPriority::LOW_PRIORITY);
		}
	}
	DirectX::XMFLOAT4A playerPos = m_owner->getClearestPlayerLocation();
	DirectX::XMFLOAT3 soundPos = m_owner->getLoudestSoundLocation().soundPos;
	DirectX::XMFLOAT4A guardPos = m_owner->getPosition();
	Tile playerTile = m_grid->WorldPosToTile(playerPos.x, playerPos.z);
	Tile soundTile = m_grid->WorldPosToTile(soundPos.x, soundPos.z);
	Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

	if (this->m_followSight)
	{
		if (!(playerTile.getX() == -1))
			playerTile = m_grid->GetRandomNearbyUnblockedTile(playerTile);
		this->SetAlertVector(m_grid->FindPath(guardTile, playerTile));
#ifdef _DEBUG
		std::cout << green << "Enemy " << m_owner->uniqueID << " Transition: Suspicious -> Investigating Sight" << white << std::endl;
#endif
	}
	else
	{
		if (!(soundTile.getX() == -1))
			soundTile = m_grid->GetRandomNearbyUnblockedTile(soundTile);
		this->SetAlertVector(m_grid->FindPath(guardTile, soundTile));
#ifdef _DEBUG
		std::cout << green << "Enemy " << m_owner->uniqueID << " Transition: Suspicious -> Investigating Sound" << white << std::endl;
#endif
	}

	this->m_state = Investigating;
	this->m_transState = AITransitionState::NoTransitionState;
}
void AI::_onObserve()
{
	//Set animation and send to client
	{
		m_owner->getAnimationPlayer()->GetStateMachine()->SetState("walk_state");
		if (Network::Multiplayer::GetInstance()->isConnected())
		{
			Network::ENEMYANIMATIONSTATEPACKET packet(m_owner->getUniqueID(), "walk_state");
			Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::ENEMYANIMATIONSTATEPACKET), PacketPriority::LOW_PRIORITY);
		}
	}

	m_owner->m_loudestSoundLocation.percentage = 0.0f;
	m_owner->m_biggestVisCounter = 0.0f;
	m_owner->setLiniearVelocity();
	this->m_actTimer = 0;
	this->m_searchTimer = 0;
	this->m_state = AIState::Suspicious;
	this->m_transState = AITransitionState::NoTransitionState;
}
void AI::_onReturnToPatrol()
{
	//Set animation and send to client
	{
		m_owner->getAnimationPlayer()->GetStateMachine()->SetState("walk_state");
		if (Network::Multiplayer::GetInstance()->isConnected())
		{
			Network::ENEMYANIMATIONSTATEPACKET packet(m_owner->getUniqueID(), "walk_state");
			Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::ENEMYANIMATIONSTATEPACKET), PacketPriority::LOW_PRIORITY);
		}
	}

	m_owner->m_loudestSoundLocation.percentage = 0.0f;
	m_owner->m_biggestVisCounter = 0.0f;
	this->m_actTimer = 0;
	this->m_searchTimer = 0;
	DirectX::XMFLOAT4A guardPos = m_owner->getPosition();
	Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);
	Tile lastPatrolPos = m_path.at(m_currentPathNode)->tile;
	this->SetAlertVector(m_grid->FindPath(guardTile, lastPatrolPos));
	this->m_state = Patrolling;
	this->m_transState = AITransitionState::NoTransitionState;
}
void AI::_onBeingPossessed()
{
	m_owner->getAnimationPlayer()->GetLayerMachine()->PopAll();
	m_owner->UnlockEnemyInput();
	m_owner->setReleased(false);
	m_owner->setHidden(true);

	CameraHandler::setActiveCamera(m_owner->getCamera());

	this->m_state = AIState::Possessed;
	this->m_transState = AITransitionState::NoTransitionState;
}
void AI::_onBeingDisabled()
{
	///Set knocked animation
	auto animationPlayer = m_owner->getAnimationPlayer();
	if (animationPlayer)
		animationPlayer->GetStateMachine()->SetState("knocked_state");

	m_owner->DisableEnemy();
	m_owner->m_knockOutType = Enemy::KnockOutType::Stoned;
	this->m_state = AIState::Disabled;
	this->m_transState = AITransitionState::NoTransitionState;
}
void AI::_onExitingPossessed()
{
	m_owner->getAnimationPlayer()->GetStateMachine()->SetState("knocked_state");

	m_owner->m_knockOutType = Enemy::KnockOutType::Possessed;
	m_owner->removePossessor(); 
	m_owner->setHidden(false);

	this->m_state = AIState::Disabled;
	this->m_transState = AITransitionState::NoTransitionState;
}
void AI::_onExitingDisabled()
{
	///Exit knocked state
	auto animationPlayer = m_owner->getAnimationPlayer();
	if (animationPlayer)
		animationPlayer->GetStateMachine()->SetState("walk_state");

	m_owner->m_disabled = false;
	m_owner->m_released = false;
	m_owner->m_possesionRecoverTimer = 0;
	m_owner->m_knockOutTimer = 0;
	this->m_state = AIState::NoState;
	this->m_transState = AITransitionState::Observe;
}
void AI::_onTorchFound()
{
	//Do we need to do anything else here? 
	this->m_state = AIState::TorchHandling;

}

void AI::_onTorchHandled()
{
	m_state = AIState::NoState;
	m_transState = AITransitionState::ReturnToPatrol;
}

void AI::_investigating(const double deltaTime)
{
	
	m_timers[T_Investigate] += deltaTime;
	bool hasTimedOut = m_timers[T_Investigate] >= m_timeOutPoints[T_Investigate];
	if (hasTimedOut)
	{
		m_timers[T_Investigate] = 0.0;
		m_state = AIState::NoState;
		m_transState = AITransitionState::ReturnToPatrol;
	}

	m_owner->getBody()->SetType(e_dynamicBody);
	

	if (this->GetAlertPathSize() > 0)
	{
		SoundLocation tmp = m_owner->m_sl;
		if (Network::Multiplayer::GetInstance()->isServer())
		{
			if (tmp.percentage < m_owner->m_slRemote.percentage)
				tmp = m_owner->m_slRemote;
		}

		DirectX::XMFLOAT3 soundPos = tmp.soundPos;
		Node * pathDestination = this->GetAlertDestination();
		DirectX::XMFLOAT4A playerPos = m_owner->m_PlayerPtr->getPosition();
		Tile targetTile;
		Tile guardTile;

		if (m_owner->m_visCounter > m_owner->m_biggestVisCounter)
		{
			// Get the remote players position
			if (m_owner->m_RemotePtr)
			{
				const int * vis = m_owner->getPlayerVisibility();
				if (vis[1] > vis[0])
					playerPos = m_owner->m_RemotePtr->getPosition();
			}
			// Check and prioritize if the guard has seen a player recently
			if (abs(pathDestination->worldPos.x - playerPos.x) > 2.0f ||
				abs(pathDestination->worldPos.y - playerPos.z) > 2.0f)
			{
				DirectX::XMFLOAT4A guardPos = m_owner->getPosition();
				targetTile = m_grid->WorldPosToTile(playerPos.x, playerPos.z);
				guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);
				this->SetAlertVector(m_grid->FindPath(guardTile, targetTile));
			}
		}
		else if (tmp.percentage > m_owner->m_loudestSoundLocation.percentage)
		{
			// Check if the guard has heard anything recently
			if (abs(pathDestination->worldPos.x - soundPos.x) > 2.0f ||
				abs(pathDestination->worldPos.y - soundPos.z) > 2.0f)
			{
				DirectX::XMFLOAT4A guardPos = m_owner->getPosition();
				targetTile = m_grid->WorldPosToTile(soundPos.x, soundPos.z);
				guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);
				this->SetAlertVector(m_grid->FindPath(guardTile, targetTile));
			}
		}
	}
	else
	{
		this->m_transState = AITransitionState::Observe;
	}
}
void AI::_suspicious(const double deltaTime)
{

	m_timers[T_Suspicious] += deltaTime;
	bool hasTimedOut = m_timers[T_Possessed] >= m_timeOutPoints[T_Suspicious];
	if (hasTimedOut)
	{
		m_timers[T_Suspicious] = 0.0;
		m_state = AIState::NoState;
		m_transState = AITransitionState::ReturnToPatrol;
	}


	m_owner->getBody()->SetType(e_dynamicBody);
	m_owner->_CheckPlayer(deltaTime);
	m_owner->setLiniearVelocity();
	this->m_actTimer += deltaTime;
	float attentionMultiplier = 1.0f; // TEMP will be moved to Enemy
	if (this->m_actTimer > SUSPICIOUS_TIME_LIMIT / 3)
	{
		attentionMultiplier = 1.2f;
	}
	if (m_owner->m_visCounter * attentionMultiplier >= m_owner->m_biggestVisCounter)
	{

		DirectX::XMFLOAT4A playerPos = m_owner->m_PlayerPtr->getPosition();
		const int * vis = m_owner->getPlayerVisibility();
		
		int	visValue = vis[0];

		if (m_owner->m_RemotePtr)
		{
			if (vis[1] > vis[0])
			{
				playerPos = m_owner->m_RemotePtr->getPosition();
				visValue = vis[1];
			}
		}

		m_owner->setClearestPlayerLocation(playerPos);
		m_owner->setBiggestVisCounter(visValue * attentionMultiplier);
	}

	SoundLocation target = m_owner->m_sl;
	if (target.percentage < m_owner->m_slRemote.percentage)
		target = m_owner->m_slRemote;

	if (target.percentage * attentionMultiplier >= m_owner->m_loudestSoundLocation.percentage)
	{
		Enemy::SoundLocation temp = target;
		temp.percentage *= attentionMultiplier;
		m_owner->m_loudestSoundLocation = temp;
	}
	if (this->m_actTimer > SUSPICIOUS_TIME_LIMIT)
	{
		if (m_owner->m_biggestVisCounter >= ALERT_TIME_LIMIT * 1.5)
		{
			this->m_transState = AITransitionState::Investigate; //what was that?
			this->m_followSight = 1;
		}
		else if (m_owner->m_loudestSoundLocation.percentage > SOUND_LEVEL * 1.5)
		{
			this->m_transState = AITransitionState::Investigate;
			this->m_followSight = 0;
		}
		else
		{
			this->m_transState = AITransitionState::ReturnToPatrol;
			//Must have been nothing...
		}
	}
}
void AI::_patrolling(const double deltaTime)
{
	m_owner->getBody()->SetType(e_dynamicBody);
	m_owner->_CheckPlayer(deltaTime);
	this->_checkTorches(deltaTime);

	if (m_alertPath.size() > 0)
	{
		_MoveToAlert(m_alertPath.at(0), deltaTime);
	}
	else if (m_path.size() > 0)
	{
		if (m_owner->m_pathNodes.size() == 0)
		{
			Manager::g_textureManager.loadTextures("FOOT");
			Manager::g_meshManager.loadStaticMesh("FOOT");
			for (unsigned int i = 0; i < m_path.size(); ++i)
			{
				Drawable * temp = new Drawable();
				temp->setModel(Manager::g_meshManager.getStaticMesh("FOOT"));
				temp->setTexture(Manager::g_textureManager.getTexture("FOOT"));
				temp->setScale({ 0.2f, 0.2f, 0.2f, 1.0f });
				temp->setPosition(m_path.at(i)->worldPos.x, m_owner->m_startYPos, m_path.at(i)->worldPos.y);

				if (i + 1 < m_path.size())
				{
					temp->setRotation(0, m_owner->_getPathNodeRotation({ m_path.at(i)->worldPos.x, m_path.at(i)->worldPos.y }, { m_path.at(i + 1)->worldPos.x, m_path.at(i + 1)->worldPos.y }), 0);
				}

				temp->SetTransparant(true);
				m_owner->m_pathNodes.push_back(temp);
			}

		}
		/*float posY = 0.04 * sin(1 * DirectX::XM_PI*m_sinWaver*0.5f) + 4.4f;
		for (auto path : m_pathNodes)
		{
			path->setPosition(path->getPosition().x, posY , path->getPosition().z);
		}*/
		if (m_owner->m_nodeFootPrintsEnabled == true)
		{
			if (m_currentPathNode != 0)
			{
				Drawable * temp = m_owner->m_pathNodes.at(m_currentPathNode - 1);
				temp->setPosition(temp->getPosition().x, temp->getPosition().y - deltaTime * 2, temp->getPosition().z);
			}
		}
		_MoveTo(m_path.at(m_currentPathNode), deltaTime);
	}

	SoundLocation target = m_owner->m_sl;

	if (target.percentage < m_owner->m_slRemote.percentage)
		target = m_owner->m_slRemote;

	if (m_owner->m_visCounter >= ALERT_TIME_LIMIT || target.percentage > SOUND_LEVEL) //"Huh?!" - Tim Allen
		this->m_transState = AITransitionState::Alerted;
}
void AI::_possessed(const double deltaTime)
{

	m_timers[T_Possessed] += deltaTime;
	bool hasTimedOut = m_timers[T_Possessed] >= m_timeOutPoints[T_Possessed];
	if (hasTimedOut)
	{
		m_timers[T_Possessed] = 0.0;
		m_state = AIState::NoState;
		m_transState = AITransitionState::ExitingPossess; 
	}
	else
	{
		m_owner->getBody()->SetType(e_dynamicBody);

		m_owner->_handleInput(deltaTime);
	}
}
void AI::_disabled(const double deltaTime)
{

	m_timers[T_Disabled] += deltaTime;
	bool hasTimedOut = m_timers[T_Disabled] >= m_timeOutPoints[T_Disabled];
	if (hasTimedOut)
	{
		m_timers[T_Disabled] = 0.0;
		m_state = AIState::NoState;
		m_transState = AITransitionState::ReturnToPatrol;
	}

	m_owner->getBody()->SetType(e_dynamicBody);
	switch (m_owner->m_knockOutType)
	{

	case Possessed:
		if (m_owner->m_released)
		{
			m_owner->m_possesionRecoverTimer += deltaTime;
			if (m_owner->m_possesionRecoverTimer >= m_owner->m_possessionRecoverMax)
			{
				m_transState = AITransitionState::ExitingDisable;
			}
		}
		break;
	case Stoned:

		m_owner->m_knockOutTimer += deltaTime;
		if (m_owner->m_knockOutMaxTime <= m_owner->m_knockOutTimer)
		{
			m_transState = AITransitionState::ExitingDisable;
		}
		break;
	}
	//m_owner->p_setRotation()
	m_owner->getBody()->SetAngularVelocity(b3Vec3(0, 0, 0));
	m_owner->getBody()->SetLinearVelocity(b3Vec3(0, 0, 0));
	//m_owner->PhysicsComponent::p_setRotation(m_owner->p_camera->getYRotationEuler().x + DirectX::XMConvertToRadians(85), m_owner->p_camera->getYRotationEuler().y, m_owner->p_camera->getYRotationEuler().z);
	m_owner->m_visCounter = 0;
}

void AI::_torchHandling(const double deltaTime)
{
	//this is to ensure that the AI never locks up in this state
	this->m_timers[T_TorchHandling] += deltaTime;

	bool hasTimedOut = this->m_timers[T_TorchHandling] >= m_timeOutPoints[T_TorchHandling];
	//Make sure we actually have a torch, if not, return to patrolling
	if (!m_currentTorch || hasTimedOut)
	{
		this->m_timers[T_TorchHandling] = 0.0;
		m_state = AIState::NoState;
		m_transState = AITransitionState::ReturnToPatrol;
	}
	//Rotate towards the torch from the guard's direction, when we are ready do the actual animation
	if (!m_activeTorch)
	{
		//Queue animation here ELIAS
		if (_RotateToCurrentTorch(deltaTime))
			m_activeTorch = true;
	}
	//If we are active, add to timer and when we reach our desired timepoint, execute order 66. 
	if (m_activeTorch)
	{
		m_timerTorch += deltaTime;
		if (m_timerTorch >= m_igniteAt)
		{
			m_timerTorch = 0.0;
			m_activeTorch = false;

			m_currentTorch->Interact();
			m_currentTorch = nullptr;

			m_transState = AITransitionState::TorchHandled;
		}
	}
}

void AI::_Move(Node * nextNode, double deltaTime)
{
	// Get dirction to target
	float x = nextNode->worldPos.x - m_owner->getPosition().x;
	float y = nextNode->worldPos.y - m_owner->getPosition().z;

	float angle = atan2(y, x);

	float dx = cos(angle) * m_owner->m_guardSpeed;
	float dy = sin(angle) * m_owner->m_guardSpeed;

	DirectX::XMFLOAT2 vel = { dx, dy };
	if (m_lv.turnState)
	{
		DirectX::XMFLOAT2 newDir;
		DirectX::XMFLOAT2 oldDir;
		if (!m_lv.next)
		{
			oldDir = m_lv.lastDir;
			newDir = m_lv.middleTarget;
		}
		else
		{
			oldDir = m_lv.middleTarget;
			newDir = { x, y };
		}

		m_lv.timer += deltaTime / m_owner->REVERSE_SPEED;
		m_lv.timer = min(1.0f, m_lv.timer);

		DirectX::XMVECTOR old, target, vNew;
		old = DirectX::XMLoadFloat2(&oldDir);
		target = DirectX::XMLoadFloat2(&newDir);
		vNew = DirectX::XMVectorLerp(old, target, m_lv.timer);
		DirectX::XMStoreFloat2(&newDir, vNew);

		vel.x = newDir.x;
		vel.y = newDir.y;
		angle = atan2(vel.x, vel.y);

		if (m_lv.timer >= 0.9999f)
		{
			m_lv.timer = 0.0f;
			if (!m_lv.next)
			{
				m_lv.next = true;
			}
			else
			{
				m_lv.turnState = false;
				m_lv.next = false;
			}
		}

	}
	else if (m_lv.newNode)
	{
		b3Vec3 lastVel = m_owner->getLiniearVelocity();
		DirectX::XMFLOAT2 xmLastVel = { lastVel.x, lastVel.z };

		DirectX::XMVECTOR vLastVel = XMLoadFloat2(&xmLastVel);
		float l = DirectX::XMVectorGetX(DirectX::XMVector2Length(vLastVel));
		if (l < 0.01f)
		{
			vLastVel = DirectX::XMVectorSet(m_owner->p_camera->getForward().x, m_owner->p_camera->getForward().z, 0.0f, 0.0f);
		}


		DirectX::XMFLOAT2 tmp = { x, y };
		DirectX::XMVECTOR vVel = XMLoadFloat2(&tmp);
		float dot = DirectX::XMVectorGetX(DirectX::XMVector2Dot(DirectX::XMVector2Normalize(vVel), (DirectX::XMVector2Normalize(vLastVel))));


		if (dot <= 0.0f)
		{
			m_lv.turnState = true;
			m_lv.timer = 0.0f;
			DirectX::XMFLOAT4A lastDir = m_owner->p_camera->getForward();
			m_lv.lastDir = { lastDir.x, lastDir.z };

			DirectX::XMFLOAT3 xm1, xm2;
			xm1 = { lastDir.x, 0.0f, lastDir.z };
			xm2 = { 0.0f, 1.0f, 0.0f };

			DirectX::XMVECTOR v1, v2;
			v1 = DirectX::XMLoadFloat3(&xm1);
			v2 = DirectX::XMLoadFloat3(&xm2);

			DirectX::XMVECTOR v3 = DirectX::XMVector3Cross(v1, v2);

			float dotLeft = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(v3), DirectX::XMVector3Normalize(vVel)));

			if (dotLeft > 0.0f)
			{
				DirectX::XMFLOAT3 xm3;
				DirectX::XMStoreFloat3(&xm3, v3);
				m_lv.middleTarget = { xm3.x, xm3.z };
			}
			else
			{
				DirectX::XMVECTOR v4 = DirectX::XMVector3Cross(v2, v1);
				DirectX::XMFLOAT3 xm3;
				DirectX::XMStoreFloat3(&xm3, v4);
				m_lv.middleTarget = { xm3.x, xm3.z };
			}

			DirectX::XMFLOAT2 newDir;
			DirectX::XMFLOAT2 oldDir;
			if (!m_lv.next)
			{
				oldDir = m_lv.lastDir;
				newDir = m_lv.middleTarget;
			}
			else
			{
				oldDir = m_lv.middleTarget;
				newDir = { x, y };
			}

			m_lv.timer += deltaTime / m_owner->REVERSE_SPEED;
			m_lv.timer = min(1.0f, m_lv.timer);

			DirectX::XMVECTOR old, target, vNew;
			old = DirectX::XMLoadFloat2(&oldDir);
			target = DirectX::XMLoadFloat2(&newDir);
			vNew = DirectX::XMVectorLerp(old, target, m_lv.timer);
			DirectX::XMStoreFloat2(&newDir, vNew);

			vel.x = newDir.x;
			vel.y = newDir.y;
			angle = atan2(vel.x, vel.y);

			if (m_lv.timer >= 0.9999f)
			{
				m_lv.timer = 0.0f;
				if (!m_lv.next)
				{
					m_lv.next = true;
				}
				else
				{
					m_lv.turnState = false;
					m_lv.next = false;
				}
			}
		}
		else
		{
			m_lv.timer += deltaTime / m_owner->TURN_SPEED;
			m_lv.timer = min(1.0f, m_lv.timer);
			DirectX::XMVECTOR lerp = DirectX::XMVectorLerp(vLastVel, vVel, m_lv.timer);

			DirectX::XMStoreFloat2(&vel, lerp);

			if (m_lv.timer >= 0.9999f)
			{
				m_lv.timer = 0.0f;
				if (m_lv.newNode)
					m_lv.next = false;
				else
					m_lv.newNode = false;
			}
		}
	}

	m_owner->_RotateGuard(vel.x * deltaTime, vel.y * deltaTime, angle, deltaTime);
	DirectX::XMStoreFloat2(&vel, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&vel)));
	m_owner->setLiniearVelocity(vel.x * m_owner->m_guardSpeed, m_owner->getLiniearVelocity().y, vel.y * m_owner->m_guardSpeed);
	
}
bool AI::_MoveTo(Node* nextNode, double deltaTime)
{
	//std::cout << "\r" << m_owner->getPosition().x << " " << m_owner->getPosition().z << std::endl;
	m_owner->_playFootsteps(deltaTime);
	if (abs(nextNode->worldPos.x - m_owner->getPosition().x) <= 1.5f && abs(nextNode->worldPos.y - m_owner->getPosition().z) <= 1.5f)
	{
		m_lv.newNode = true;
		m_lv.timer = 0.0f;
		m_currentPathNode++;
		if (m_currentPathNode == m_path.size())
		{
			std::reverse(m_path.begin(), m_path.end());
			std::reverse(m_owner->m_pathNodes.begin(), m_owner->m_pathNodes.end());

			for (auto node : m_owner->m_pathNodes)
			{
				node->setPosition(node->getPosition().x, m_owner->m_startYPos, node->getPosition().z);
			}

			m_owner->m_isReversed = RipExtern::BoolReverser(m_owner->m_isReversed);
			m_currentPathNode = 0;
			return true;
		}
	}
	else
	{
		_Move(nextNode, deltaTime);
	}
	return false;
}
bool AI::_MoveToAlert(Node * nextNode, double deltaTime)
{
	m_owner->_playFootsteps(deltaTime);
	if (abs(nextNode->worldPos.x - m_owner->getPosition().x) <= 1.5 && abs(nextNode->worldPos.y - m_owner->getPosition().z) <= 1.5)
	{
		delete nextNode;
		m_alertPath.erase(m_alertPath.begin());
		m_lv.newNode = true;
		m_lv.timer = 0.0f;
		m_lv.next = false;
		m_lv.turnState = false;
	}
	else
	{
		_Move(nextNode, deltaTime);
	}
	return false;
}
float AI::_getPathNodeRotation(DirectX::XMFLOAT2 first, DirectX::XMFLOAT2 last)
{
	if (first.x > last.x)
	{
		//Up
		return 90;
	}
	else if (first.x < last.x)
	{
		//Down
		return 90;
	}
	else if (first.y > last.y)
	{
		//Up
		return 0;
	}
	else if (first.y < last.y)
	{
		//left
		return 0;

	}

	return 0;
}
bool AI::_RotateToCurrentTorch(double deltaTime)
{
	//https://stackoverflow.com/questions/14066933/direct-way-of-computing-clockwise-angle-between-2-vectors
	m_owner->setVelocity({ 0.0f, 0.0f, 0.0f });
	using namespace DirectX;
	//Get the current direction and construct a directional vector from guard's pos towards the torch pos 
	
	//Put Everything into the x-z plane
	XMFLOAT4A camDir = m_owner->getCamera()->getDirection();
	camDir.y = 0.0f;

	XMFLOAT4A guardPos = m_owner->getPosition();
	guardPos.y = 0.0f;

	XMFLOAT4A torchPos = m_currentTorch->getPosition();
	torchPos.y = 0.0f;

	auto currentDirection =  XMVector3Normalize( XMLoadFloat4A(&camDir) );
	auto directionToTorch = XMVector3Normalize( XMVectorSubtract( XMLoadFloat4A(&guardPos), XMLoadFloat4A(&torchPos) ) );

	auto normal = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); //Is already a normalized vector

	//Compute the DOT product between our directional vectors
	auto dot = XMVectorGetX( XMVector3Dot(currentDirection, directionToTorch) );
	//Compute the determinant with the triple product
	auto det = XMVectorGetX( XMVector3Dot(normal, XMVector3Cross(currentDirection, directionToTorch)) );

	auto angle = atan2f(det, dot);

	std::string fileName = "Enemy_" + std::to_string(m_owner->uniqueID) + "_Angles.txt";

	FILE * pFile = fopen(fileName.c_str(), "a");
	if (pFile == NULL)
		PostQuitMessage(0);
	if (fprintf(pFile, "Angle %f\n", angle) < 0)
		PostQuitMessage(0);
	fclose(pFile);
	//if the angle is small enough we are probably facing the torch enough by now, return true
	if (angle <= 0.0f)
		return true;
	
	m_owner->_RotateGuard(0.5f, 0.5f, angle, deltaTime);

	return false;
}
void AI::_checkTorches(float dt)
{
	static const float radiusSquared = CHECK_TORCHES_RADIUS * CHECK_TORCHES_RADIUS;
	static bool shotRay = false;
	m_checkTorchesTimer += dt;

	if (shotRay)
	{
		if (RipExtern::g_rayListener->hasRayHit(m_torchRay))
		{
			RayCastListener::Ray & ray = RipExtern::g_rayListener->ConsumeProcessedRay(m_torchRay);
			RayCastListener::RayContact con = ray.getClosestContact();
			if (con.contactShape->GetBody()->GetObjectTag() == "TORCH")
				static_cast<Torch*>(con.contactShape->GetBody()->GetUserData())->Interact();
		}
		shotRay = false;
	}

	if (m_checkTorchesTimer >= CHECK_TORCHES_INTERVALL)
	{
		m_checkTorchesTimer -= CHECK_TORCHES_INTERVALL;
		DirectX::XMFLOAT4A origin = m_owner->getPosition();
		for (auto & t : m_owner->m_torches)
		{
			DirectX::XMFLOAT4A point = t->getPosition();
			float distance = ((point.x - origin.x) * (point.x - origin.x) + (point.z - origin.z) * (point.z - origin.z));
			if (distance <= radiusSquared)
			{
				if (t->getTriggerState())
				{
					m_torchRay = RipExtern::g_rayListener->PrepareRay(m_owner->getBody(), m_owner->getPosition(), point);
					shotRay = true;

					return;
				}
			}

		}
	}


}

//------------------------------Public------------------------------------
void AI::setGrid(Grid * grid)
{
	m_grid = grid;
}
void AI::SetPathVector(std::vector<Node*> path)
{
	// Add path.size() > 0 if it can remove the path
	if (m_path.size() > 0)
	{
		for (int i = 0; i < m_path.size(); i++)
			delete m_path.at(i);
		m_path.clear();
	}
	m_currentPathNode = 0;
	m_path = path;
}
Node * AI::GetCurrentPathNode() const
{
	return m_path.at(m_currentPathNode);
}
int AI::GetCurrentPathNodeGridID() const
{
	return this->m_currentPathNode;
}
void AI::SetAlertVector(std::vector<Node*> alertPath)
{
	if (m_alertPath.size() > 0)
	{
		for (int i = 0; i < m_alertPath.size(); i++)
			delete m_alertPath.at(i);
		m_alertPath.clear();
	}
	m_alertPath = alertPath;
}
bool AI::GetPathEmpty() const
{
	return m_path.empty();
}
size_t AI::GetAlertPathSize() const
{
	return m_alertPath.size();
}
Node * AI::GetAlertDestination() const
{
	return m_alertPath.at(m_alertPath.size() - 1);
}

AIState AI::getAIState() const
{
	return m_state;
}
void AI::setAIState(AIState state)
{
	m_state = state;
}
AITransitionState AI::getTransitionState() const
{
	return m_transState;
}
void AI::setTransitionState(AITransitionState state)
{
	m_transState = state;
}

DirectX::XMFLOAT2 AI::GetDirectionToPlayer(const DirectX::XMFLOAT4A& player, Camera& playerCma)
{
	using namespace DirectX;

	if (m_owner->m_visCounter > 0)
	{
		XMMATRIX playerView = XMMatrixTranspose(XMLoadFloat4x4A(&playerCma.getView()));
		XMVECTOR enemyPos = XMLoadFloat4A(&m_owner->getPosition());
		XMVECTOR vdir = XMVector3TransformCoord(enemyPos, playerView);
		XMFLOAT2 dir = XMFLOAT2(DirectX::XMVectorGetX(vdir), DirectX::XMVectorGetZ(vdir));
		vdir = XMLoadFloat2(&dir);
		vdir = XMVector2Normalize(vdir);

		XMStoreFloat2(&dir, vdir);
		return dir;
	}
	return XMFLOAT2(0, 0);
}
