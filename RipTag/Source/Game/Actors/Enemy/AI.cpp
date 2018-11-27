#include "RipTagPCH.h"
#include "AI.h"

AI::AI()
{

}

AI::AI(Enemy * owner)
{
	m_owner = owner;
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
	case AITransitionState::InvestigateSound:
		this->_onInvestigateSound();
		break;
	case AITransitionState::InvestigateSight:
		this->_onInvestigateSight();
		break;
	case AITransitionState::Observe:
		this->_onObserve();
		break;
	case AITransitionState::SearchArea:
		this->_onSearchArea();
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
	}

	switch (m_state)
	{
	case AIState::Investigating_Sight:
#ifdef _DEBUG
		std::cout << yellow << "Enemy State: Investigating Sight" << white << "\r";
#endif

		if (timer > 0.3f)
		{
			timer = 0.0f;
			this->_investigatingSight(deltaTime);
		}
		if (m_transState == AITransitionState::NoTransitionState)
		{
			if (m_alertPath.size() != 0)
			{
				_MoveToAlert(m_alertPath.at(0), deltaTime);
			}
		}
		m_owner->_detectTeleportSphere();
		break;
	case AIState::Investigating_Sound:
#ifdef _DEBUG
		std::cout << yellow << "Enemy State: Investigating Sound" << white << "\r";
#endif

		if (timer > 0.3f)
		{
			timer = 0.0f;
			this->_investigatingSound(deltaTime);
		}
		if (m_transState == AITransitionState::NoTransitionState)
			if (m_alertPath.size() != 0)
			{
				_MoveToAlert(m_alertPath.at(0), deltaTime);
			}
		m_owner->_detectTeleportSphere();
		break;
	case AIState::Investigating_Room:
#ifdef _DEBUG
		std::cout << yellow << "Enemy State: Investigating Room" << white << "\r";
#endif

		this->_investigatingRoom(deltaTime);
		m_owner->_detectTeleportSphere();
		if (m_transState == AITransitionState::NoTransitionState)
			if (m_alertPath.size() != 0)
			{
				_MoveToAlert(m_alertPath.at(0), deltaTime);
			}
		break;
	case AIState::High_Alert:
#ifdef _DEBUG
		std::cout << yellow << "Enemy State: High Alert" << white << "\r";
#endif

		this->_highAlert(deltaTime);
		m_owner->_detectTeleportSphere();
		break;
	case AIState::Patrolling:
#ifdef _DEBUG
		std::cout << yellow << "Enemy State: Patrolling" << white << "\r";
#endif

		this->_patrolling(deltaTime);
		m_owner->_detectTeleportSphere();
		break;
	case AIState::Suspicious:
#ifdef _DEBUG
		std::cout << yellow << "Enemy State: Suspicious" << white << "\r";
#endif

		this->_suspicious(deltaTime);
		m_owner->_detectTeleportSphere();
		break;
	case AIState::Scanning_Area:
#ifdef _DEBUG
		std::cout << yellow << "Enemy State: Scanning Area" << white << "\r";
#endif

		this->_scanningArea(deltaTime);
		m_owner->_detectTeleportSphere();
		break;
	case AIState::Possessed:
#ifdef _DEBUG
		std::cout << yellow << "Enemy State: Possessed" << white << "\r";
#endif
		_possessed(deltaTime);
		break;
	case AIState::Disabled:
#ifdef _DEBUG
		std::cout << yellow << "Enemy State: Disabled" << white << "\r";
#endif
		_disabled(deltaTime);
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
#ifdef _DEBUG
	std::cout << green << "Enemy " << m_owner->uniqueID << " Transition: Patrolling -> Suspicious" << white << std::endl;
#endif
	m_owner->setLiniearVelocity(0.0f, m_owner->getLiniearVelocity().y, 0.0f);
	this->m_state = AIState::Suspicious;
	this->m_actTimer = 0;
	m_owner->m_clearestPlayerPos = DirectX::XMFLOAT4A(0, 0, 0, 1);
	m_owner->m_loudestSoundLocation = AI::SoundLocation();
	m_owner->m_biggestVisCounter = 0;
	FMOD_VECTOR at = { m_owner->getPosition().x, m_owner->getPosition().y, m_owner->getPosition().z };
	AudioEngine::PlaySoundEffect(RipSounds::g_grunt, &at, AudioEngine::Enemy);
	this->m_transState = AITransitionState::NoTransitionState;
}

void AI::_onInvestigateSound()
{
	DirectX::XMFLOAT3 soundPos = m_owner->getLoudestSoundLocation().soundPos;
	DirectX::XMFLOAT4A guardPos = m_owner->getPosition();
	Tile soundTile = m_grid->WorldPosToTile(soundPos.x, soundPos.z);
	Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

	if (soundTile.getX() == -1 && soundTile.getY() == -1)
	{
		soundTile = m_grid->GetRandomNearbyTile(guardTile, 0);
	}

	this->SetAlertVector(m_grid->FindPath(guardTile, soundTile));
	




	// If pathfindingThread is finnished
#ifdef _DEBUG
	std::cout << green << "Enemy " << m_owner->uniqueID << " Transition: Suspicious -> Investigate Sound" << white << std::endl;
#endif
	this->m_state = Investigating_Sound;
	this->m_transState = AITransitionState::NoTransitionState;
}

void AI::_onInvestigateSight()
{
	DirectX::XMFLOAT4A playerPos = m_owner->getClearestPlayerLocation();
	DirectX::XMFLOAT4A guardPos = m_owner->getPosition();
	Tile playerTile = m_grid->WorldPosToTile(playerPos.x, playerPos.z);
	Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

	if (playerTile.getX() == -1 && playerTile.getY() == -1)
	{
		playerTile = m_grid->GetRandomNearbyTile(guardTile, 0);
	}

	this->SetAlertVector(m_grid->FindPath(guardTile, playerTile));
	this->m_state = Investigating_Sight;
#ifdef _DEBUG

	std::cout << green << "Enemy " << m_owner->uniqueID << " Transition: Suspicious -> Investigate Sight" << white << std::endl;
#endif
	this->m_transState = AITransitionState::NoTransitionState;
}

void AI::_onObserve()
{
	m_owner->setLiniearVelocity();
	DirectX::XMFLOAT4A guardPos = m_owner->getPosition();
	Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);
	this->m_actTimer = 0;
	//this->SetAlertVector(m_grid->FindPath(guardTile, this->GetCurrentPathNode()->tile));
#ifdef _DEBUG

	std::cout << green << "Enemy " << m_owner->uniqueID << " Transition: Investigating Source -> Scanning Area" << white << std::endl;
#endif
	this->m_state = Scanning_Area;
	this->m_transState = AITransitionState::NoTransitionState;
}

void AI::_onSearchArea()
{
	this->m_actTimer = 0;
	this->m_state = Investigating_Room;
#ifdef _DEBUG

	std::cout << green << "Enemy " << m_owner->uniqueID << " Transition: Scanning Area -> Investigating Area" << white << std::endl;
#endif
	this->m_transState = AITransitionState::NoTransitionState;
	auto pos = m_owner->getPosition();
	Tile guardTile = m_grid->WorldPosToTile(pos.x, pos.z);

	int x = rand() % 4 + 1;
	int y = rand() % 11;

	DirectX::XMFLOAT2 gPos = { (float)guardTile.getX(), (float)guardTile.getY() };
	DirectX::XMFLOAT2 tPos = { (float)x, (float)y };

	float dist = DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&gPos), DirectX::XMLoadFloat2(&tPos))));

	while (dist < 3.0f)
	{
		int x = rand() % 4 + 1;
		int y = rand() % 10;

		DirectX::XMFLOAT2 tPos = { (float)x, (float)y };

		dist = DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&gPos), DirectX::XMLoadFloat2(&tPos))));
	}


	std::vector<Node*> fullPath = m_grid->FindPath(guardTile, Tile(x, y));
	//std::vector<Node*> partOfPath = m_grid->FindPath(Tile(5, 6), Tile(1, 2));
	//fullPath.insert(std::end(fullPath), std::begin(partOfPath), std::end(partOfPath));

	this->SetAlertVector(fullPath);
}

void AI::_onReturnToPatrol()
{
	this->m_actTimer = 0;
	this->m_searchTimer = 0;
	DirectX::XMFLOAT4A guardPos = m_owner->getPosition();
	Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);
	Tile lastPatrolPos = m_grid->WorldPosToTile(m_path.at(m_currentPathNode)->worldPos.x, m_path.at(m_currentPathNode)->worldPos.y);
	this->SetAlertVector(m_grid->FindPath(guardTile, lastPatrolPos));
	this->m_state = Patrolling;
#ifdef _DEBUG

	std::cout << green << "Enemy " << m_owner->uniqueID << " Transition: Suspicious -> Patrolling" << white << std::endl;
#endif
	this->m_transState = AITransitionState::NoTransitionState;
}

void AI::_onBeingPossessed()
{
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
	m_owner->m_knockOutType = Enemy::KnockOutType::Possessed;
	m_owner->DisableEnemy();
	m_owner->setReleased(true);
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


void AI::_investigatingSight(const double deltaTime)
{
	m_owner->getBody()->SetType(e_dynamicBody);
	m_owner->_cameraPlacement(deltaTime);
	m_owner->_CheckPlayer(deltaTime);

	if (this->GetAlertPathSize() > 0)
	{
		if (m_owner->m_visCounter > m_owner->m_biggestVisCounter)
		{
			DirectX::XMFLOAT4A playerPos = m_owner->m_PlayerPtr->getPosition();

			if (m_owner->m_RemotePtr)
			{
				const int * vis = m_owner->getPlayerVisibility();
				if (vis[1] > vis[0])
					playerPos = m_owner->m_RemotePtr->getPosition();
			}


			Node * pathDestination = this->GetAlertDestination();

			if (abs(pathDestination->worldPos.x - playerPos.x) > 5.0f ||
				abs(pathDestination->worldPos.y - playerPos.z) > 5.0f)
			{
				DirectX::XMFLOAT4A guardPos = m_owner->getPosition();
				Tile playerTile = m_grid->WorldPosToTile(playerPos.x, playerPos.z);
				Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

				this->SetAlertVector(m_grid->FindPath(guardTile, playerTile));
			}
		}
	}
	else
	{
		this->m_transState = AITransitionState::Observe;
	}
}
void AI::_investigatingSound(const double deltaTime)
{
	m_owner->getBody()->SetType(e_dynamicBody);
	m_owner->_cameraPlacement(deltaTime);
	m_owner->_CheckPlayer(deltaTime);

	if (this->GetAlertPathSize() > 0)
	{
		SoundLocation tmp = m_owner->m_sl;

		if (Network::Multiplayer::GetInstance()->isServer())
		{
			if (tmp.percentage < m_owner->m_slRemote.percentage)
				tmp = m_owner->m_slRemote;
		}



		if (tmp.percentage > m_owner->m_loudestSoundLocation.percentage)
		{
			DirectX::XMFLOAT3 soundPos = tmp.soundPos;
			Node * pathDestination = this->GetAlertDestination();

			if (abs(pathDestination->worldPos.x - soundPos.x) > 2.0f ||
				abs(pathDestination->worldPos.y - soundPos.z) > 2.0f)
			{
				DirectX::XMFLOAT4A guardPos = m_owner->getPosition();
				Tile playerTile = m_grid->WorldPosToTile(soundPos.x, soundPos.z);
				Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

				this->SetAlertVector(m_grid->FindPath(guardTile, playerTile));
			}
		}
	}
	else
	{
		this->m_transState = AITransitionState::Observe;
	}
}
void AI::_investigatingRoom(const double deltaTime)
{
	m_owner->getBody()->SetType(e_dynamicBody);
	m_owner->_cameraPlacement(deltaTime);
	m_owner->_CheckPlayer(deltaTime);

	this->m_searchTimer += deltaTime;

	if (this->GetAlertPathSize() == 0)
	{
		this->m_transState = AITransitionState::Observe;
	}
	SoundLocation target = m_owner->m_sl;

	if (target.percentage < m_owner->m_slRemote.percentage)
		target = m_owner->m_slRemote;
	if (m_owner->m_visCounter >= ALERT_TIME_LIMIT || target.percentage > SOUND_LEVEL)
	{
		//this->setTransitionState(EnemyTransitionState::Alerted);
	}
	if (this->m_searchTimer > SEARCH_ROOM_TIME_LIMIT)
	{
		this->m_transState = AITransitionState::ReturnToPatrol;
	}
}
void AI::_highAlert(const double deltaTime)
{
	m_owner->getBody()->SetType(e_dynamicBody);
	m_owner->_cameraPlacement(deltaTime);
	m_owner->_CheckPlayer(deltaTime);

	this->m_HighAlertTime = deltaTime;
	if (this->m_HighAlertTime >= HIGH_ALERT_LIMIT)
	{
		this->m_HighAlertTime = 0.0f;
		m_owner->m_clearestPlayerPos = DirectX::XMFLOAT4A(0, 0, 0, 1);
		m_owner->m_loudestSoundLocation = SoundLocation();
		m_owner->m_biggestVisCounter = 0.0f;
		this->m_state = AIState::Suspicious;
#ifdef _DEBUG

		std::cout << green << "Enemy " << m_owner->uniqueID << " Transition: High Alert -> Suspicious" << white << std::endl;
#endif
	}
}
void AI::_suspicious(const double deltaTime)
{
	m_owner->getBody()->SetType(e_dynamicBody);
	m_owner->_cameraPlacement(deltaTime);
	m_owner->_CheckPlayer(deltaTime);
	m_owner->setLiniearVelocity();
	this->m_actTimer += deltaTime;
	float attentionMultiplier = 1.0f; // TEMP will be moved to Enemy
	if (this->m_actTimer > SUSPICIOUS_TIME_LIMIT / 3)
	{
		attentionMultiplier = 1.2f;
	}
	if (m_owner->m_visCounter*attentionMultiplier >= m_owner->m_biggestVisCounter)
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
		m_owner->setBiggestVisCounter(vis[0]*attentionMultiplier);
	
	}

	SoundLocation target = m_owner->m_sl;

	if (target.percentage < m_owner->m_slRemote.percentage)
		target = m_owner->m_slRemote;

	if (target.percentage*attentionMultiplier >= m_owner->m_loudestSoundLocation.percentage)
	{
		Enemy::SoundLocation temp = target;
		temp.percentage *= attentionMultiplier;
		m_owner->m_loudestSoundLocation = temp;
	}
	if (this->m_actTimer > SUSPICIOUS_TIME_LIMIT)
	{
		if (m_owner->m_biggestVisCounter >= ALERT_TIME_LIMIT * 1.5)
			this->m_transState = AITransitionState::InvestigateSight; //what was that?
		else if (m_owner->m_loudestSoundLocation.percentage > SOUND_LEVEL*1.5)
			this->m_transState = AITransitionState::InvestigateSound; //what was that noise?
		else
		{
			this->m_transState = AITransitionState::ReturnToPatrol;
			//Must have been nothing...
		}
	}
}
void AI::_scanningArea(const double deltaTime)
{
	m_owner->getBody()->SetType(e_dynamicBody);
	m_owner->_cameraPlacement(deltaTime);
	m_owner->_CheckPlayer(deltaTime);
	m_owner->setLiniearVelocity();
	this->m_actTimer += deltaTime;
	//Do animation
	if (this->m_actTimer > SUSPICIOUS_TIME_LIMIT)
	{
		if (m_searchTimer != 0)
			m_searchTimer += m_actTimer;
		//CHANGE WHEN WE WANT TO CONNECT MORE TRANSITIONS
		this->m_transState = AITransitionState::ReturnToPatrol;  //this->m_transState = AITransitionState::SearchArea;
	}
}
void AI::_patrolling(const double deltaTime)
{
	m_owner->getBody()->SetType(e_dynamicBody);
	m_owner->_cameraPlacement(deltaTime);
	m_owner->_CheckPlayer(deltaTime);

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
	{
		this->m_transState = AITransitionState::Alerted;
	}
}
void AI::_possessed(const double deltaTime)
{
	m_owner->getBody()->SetType(e_dynamicBody);
	m_owner->_cameraPlacement(deltaTime);
	m_owner->_handleInput(deltaTime);
}
void AI::_disabled(const double deltaTime)
{
	m_owner->getBody()->SetType(e_staticBody);
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
	//m_owner->PhysicsComponent::p_setRotation(m_owner->p_camera->getYRotationEuler().x + DirectX::XMConvertToRadians(85), m_owner->p_camera->getYRotationEuler().y, m_owner->p_camera->getYRotationEuler().z);
	m_owner->m_visCounter = 0;
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
	m_owner->_cameraPlacement(deltaTime);
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

//------------------------------Public------------------------------------
void AI::setGrid(Grid * grid)
{
	m_grid = grid;
}

void AI::SetPathVector(std::vector<Node*> path)
{
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

		XMVECTOR vdir = XMVector4Transform(enemyPos, playerView);
		XMFLOAT2 dir = XMFLOAT2(DirectX::XMVectorGetX(vdir), DirectX::XMVectorGetZ(vdir));
		vdir = XMLoadFloat2(&dir);
		vdir = XMVector2Normalize(vdir);

		XMStoreFloat2(&dir, vdir);
		return dir;
	}
	return XMFLOAT2(0, 0);
}
