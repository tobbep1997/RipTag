#include "RipTagPCH.h"
#include "Player.h"
#include "../../../Engine/EngineSource/Helper/AnimationDebugHelper.h"

void Player::SetThrowing(bool throwing)
{
	m_IsThrowing = throwing;
}

//#todoREMOVE
float Player::m_currentPitch = 0.0f;

Player::Player() : Actor(), CameraHolder(), PhysicsComponent(), HUDComponent()
{
	Manager::g_textureManager.loadTextures("CROSS");
	Manager::g_textureManager.loadTextures("CROSSHAND");
	Manager::g_textureManager.loadTextures("CROSSPHASE");
	Manager::g_textureManager.loadTextures("CROSSPOSSESS");
	Manager::g_textureManager.loadTextures("BLACK");
	Manager::g_textureManager.loadTextures("VISIBILITYICON");
	Manager::g_textureManager.loadTextures("WHITE");


	//float convertion = (float)Input::GetPlayerFOV() / 100;
	//p_initCamera(new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 0.1f, 110.0f));
	p_initCamera(new Camera(DirectX::XMConvertToRadians(Input::GetPlayerFOV()), 16.0f / 9.0f, 0.1f, 50.0f));
	p_camera->setPosition(0, 0, 0);
	m_lockPlayerInput = false;

	//Ability stuff
	{

		TeleportAbility * m_teleport = new TeleportAbility();
		m_teleport->setOwner(this);
		m_teleport->setIsLocal(true);
		m_teleport->Init();

		DisableAbility * m_dis = new DisableAbility();
		m_dis->setOwner(this);
		m_dis->setIsLocal(true);
		m_dis->Init();

		BlinkAbility * m_blink = new BlinkAbility();
		m_blink->setOwner(this);
		m_blink->setIsLocal(true);
		m_blink->Init();

		PossessGuard * m_possess = new PossessGuard();
		m_possess->setOwner(this);
		m_possess->setIsLocal(true);
		m_possess->Init();

		m_abilityComponents1 = new AbilityComponent*[m_nrOfAbilitys];
		m_abilityComponents1[0] = m_teleport;
		m_abilityComponents1[1] = m_dis;

		m_abilityComponents2 = new AbilityComponent*[m_nrOfAbilitys];
		m_abilityComponents2[0] = m_blink;
		m_abilityComponents2[1] = m_possess;

		m_currentAbility = (Ability)0;

		//By default always this set
		m_activeSet = m_abilityComponents1;

		SetAbilitySet(1);
	}

	_initSoundHUD();

	SetFirstPersonModel();

	this->p_camera->setPerspectiv(Camera::Perspectiv::Player);
	m_footSteps.emitter = AudioEngine::Player;
	m_footSteps.owner = this;
	m_footSteps.loudness = 1.0f;

	m_smokeBomb.emitter = AudioEngine::Player; 
	m_smokeBomb.owner = this;
	m_smokeBomb.loudness = 0.6f; 

	m_head = DBG_NEW PhysicsComponent();
}

Player::Player(RakNet::NetworkID nID, float x, float y, float z) : Actor(), CameraHolder(), PhysicsComponent()
{
	p_initCamera(new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 0.1f, 50.0f));
	p_camera->setPosition(x, y, z);
	m_lockPlayerInput = false;
	m_footSteps.emitter = AudioEngine::Player;
	m_footSteps.owner = this;
	m_footSteps.loudness = 1.0f;

	m_smokeBomb.emitter = AudioEngine::Player;
	m_smokeBomb.owner = this;
	m_smokeBomb.loudness = 1.5f;
}

Player::~Player()
{
	if (m_FirstPersonModel)
		delete m_FirstPersonModel;
	for (unsigned short int i = 0; i < m_nrOfAbilitys; i++)
		delete m_abilityComponents1[i];
	delete[] m_abilityComponents1;
	for (unsigned short int i = 0; i < m_nrOfAbilitys; i++)
		delete m_abilityComponents2[i];
	delete[] m_abilityComponents2;

	m_head->Release(*RipExtern::g_world);
	delete m_head;
	HUDComponent::removeHUD();

	for (int i = 0; i < MAX_ENEMY_CIRCLES; i++)
	{
		m_enemyCircles[i]->Release();
		delete m_enemyCircles[i];
	}
	delete[] m_abilityCircle;
}

void Player::Init(b3World& world, b3BodyType bodyType, float x, float y, float z)
{
	PhysicsComponent::Init(world, bodyType, x / 2.0f, y / 2.0f, z / 2.0f, false, 0);
	this->getBody()->SetObjectTag("PLAYER");
	this->getBody()->AddToFilters("TELEPORT");

	CreateShape(b3Vec3(0, y*0.70, 0), b3Vec3(x/2, y/2, z/2), 1.0f, 1.0f, "UPPERBODY");
	CreateShape(b3Vec3(0, y*1.5, 0), b3Vec3(0.5f, 0.4f, 0.5f), 1.0f, 1.0f, "HEAD", true);
	m_standHeight = y*0.6;
	m_crouchHeight = y*0.2;
	setUserDataBody(this);
	
	m_head->Init(world, bodyType, 0.4, 0.4, 0.4, false, 0);
	m_head->getBody()->GetShapeList()->SetObjectTag("HEADO");
	m_head->setGravityScale(0);
	m_head->setObjectTag("HEADO");
	m_head->getBody()->AddToFilters("PLAYER");

	setEntityType(EntityType::PlayerType);
	setColor(10, 10, 0, 1);

	setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	setScale(1.0f, 1.0f, 1.0f);
	setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	setTextureTileMult(2, 2);

	setHidden(true);
	m_footSteps.emitter = AudioEngine::Player;
	m_footSteps.owner = this;
	m_footSteps.loudness = 1.0f;

	m_smokeBomb.emitter = AudioEngine::Player;
	m_smokeBomb.owner = this;
	m_smokeBomb.loudness = 1.5f;
}

void Player::BeginPlay()
{

}
#include <math.h>
void Player::Update(double deltaTime)
{
#ifndef _DEPLOY
	_cheats();
#endif

	/*if (getLiniearVelocity().y > 5.0f)
		setLiniearVelocity(getLiniearVelocity().x, 5.0f, getLiniearVelocity().z);*/

	{
		using namespace DirectX;
		//calculate walk direction (-1, 1, based on camera) and movement speed
		{
			///Speed
			auto physSpeed = this->getLiniearVelocity();
			float speed = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSet(physSpeed.x, 0.0, physSpeed.z, 0)));
			m_currentSpeed = std::clamp(std::fabs(speed), 0.0f, 6.0f);

			///Walk dir
				//Get camera direction and normalize on X,Z plane
			auto cameraDir = p_camera->getDirection();
			XMVECTOR cameraDirNormalized = XMVector3Normalize(XMVectorSet(cameraDir.x, 0.0f, cameraDir.z, 0.0));
			///assert(XMVectorGetX(XMVector3Length(cameraDirNormalized)) != 0.0f);

			auto XZCameraDir = XMVectorSet(cameraDir.x, 0.0, cameraDir.z, 0.0);
			auto XZMovement = XMVectorSet(physSpeed.x, 0.0, physSpeed.z, 0.0);
			auto XZCameraDirNormalized = XMVector3Normalize(XZCameraDir);
			auto XZMovementNormalized = XMVector3Normalize(XZMovement);
			///AssertHasLength(XZCameraDir);
			//AssertHasLength(XZMovement);

				//Get dot product of cam dir and player movement
			auto dot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMVectorSet(physSpeed.x, 0, physSpeed.z, 0.0)), cameraDirNormalized));
			dot = std::clamp(dot, -0.999999f, 0.999999f);
			//Convert to degrees
			m_currentDirection = XMConvertToDegrees(std::acos(dot));
			//Negate if necessary
			float inverter = (XMVectorGetY(XMVector3Cross(XZMovement, XZCameraDir)));

			m_currentDirection *= (inverter > 0.0)
				? -1.0
				: 1.0;
			m_currentDirection = std::clamp(m_currentDirection, -180.0f, 180.0f);

			//Do first-person turn rate thing
			{
				//World dir
				{
					auto dotWorld = XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMVectorSet(0.0, 0.0, 1.0, 0.0)), cameraDirNormalized));
					dotWorld = std::clamp(dotWorld, -0.999999f, 0.999999f);
					m_currentWorldDirection = XMConvertToDegrees(std::acos(dotWorld));
					float worldInverter = (XMVectorGetY(XMVector3Cross(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0), cameraDirNormalized)));

					m_currentWorldDirection *= (worldInverter > 0.0)
						? -1.0
						: 1.0;
				}

				float degreesPerSecond = 0.0f;

				if (m_lastDirection < -145 && m_currentWorldDirection > 145)
					degreesPerSecond = (m_currentWorldDirection - m_lastDirection - 360) / deltaTime;
				else if (m_lastDirection > 145 && m_currentWorldDirection < -145)
					degreesPerSecond = (m_currentWorldDirection - m_lastDirection + 360) / deltaTime;
				else
					degreesPerSecond = (m_lastDirection - m_currentWorldDirection) / deltaTime;

				m_currentTurnSpeed = degreesPerSecond;



				m_lastDirection = m_currentWorldDirection;

			}
		}

		if (m_IsMoving())
		{
			if (!m_IsThrowing)
			{
				m_FirstPersonModel->getAnimationPlayer()->GetLayerMachine()->ActivateLayerIfInactive("bob");
			}
		}
		else
		{
			m_FirstPersonModel->getAnimationPlayer()->GetLayerMachine()->BlendOutLayer("bob");
		}
	}
	const DirectX::XMFLOAT4A xmLP = p_camera->getPosition();
	FMOD_VECTOR fvLP = { xmLP.x, xmLP.y, xmLP.z, };
	//std::cout << getPosition().x << " " << getPosition().y << " " << getPosition().z << std::endl;

	using namespace DirectX;

	_hasWon();
	if (m_lockPlayerInput == false)
	{
		if (InputHandler::getWindowFocus())
		{
			_handleInput(deltaTime);
		}
	}

	m_HUDcircleFiller->setRadie((totVis)*.5f);

	this->getAnimationPlayer()->Update(deltaTime);

	//m_activeSet[m_currentAbility]->Update(deltaTime);


	for (int i = 0; i < m_nrOfAbilitys; i++)
	{
		
		m_activeSet[i]->Update(deltaTime);
		

		/*if (i != m_currentAbility)
		{
			m_activeSet[i]->updateCooldown(deltaTime);
		}*/
	}


	_cameraPlacement(deltaTime);
	_updateFMODListener(deltaTime, xmLP);
	//HUDComponent::HUDUpdate(deltaTime);


	HUDComponent::ResetStates();
	//HUDComponent::setSelectedQuad(m_currentAbility);
	for (int i = 0; i < m_nrOfAbilitys; i++)
	{
		Circle * current = m_abilityCircle[i];
		if (m_activeSet[i]->getPercentage() <= 0.0f)
		{
			current->setAngle(360.0f);
		}
		else
			current->setAngle(m_activeSet[i]->getPercentage() * 360.0f);
	}
	
	for (unsigned int i = 0; i < m_nrOfAbilitys; ++i)
	{
		Circle * current = m_abilityCircle[i+2];
		if(m_activeSet[i]->GetAbilityChargeMax() == 1)
		{
			current->setInnerRadie(100.0f);	
		}
		current->setAngle(m_activeSet[i]->getAbilityChargesPercent() * 360.0f);
	}

	p_addRotation(0, p_camera->getYRotationEuler().y + DirectX::XM_PI * .5f, 0);

	_updateFirstPerson(deltaTime);
}

void Player::PhysicsUpdate()
{
	p_updatePhysics(this);
	_collision();
	PhysicsComponent::p_setRotation(0, p_camera->getEulerRotation().y, 0);
	p_addRotation(0, DirectX::XM_PI * 1.5f, 0);


	b3Vec3 vel = this->getLiniearVelocity();
	b3Vec3 pos = this->getBody()->GetTransform().translation + this->getBody()->GetShapeList()->GetTransform().translation;
	m_head->p_setRotation(0, p_camera->getYRotationEuler().y, 0);
	m_head->p_setPosition(pos.x, pos.y, pos.z);
	m_head->setLiniearVelocity(vel.x, vel.y, vel.z);
}

void Player::setPosition(const float& x, const float& y, const float& z, const float& w)
{
	Transform::setPosition(x, y, z, w);
	PhysicsComponent::p_setPosition(x, y, z);
}

void Player::setPosition(const DirectX::XMFLOAT4A& pos)
{
	Transform::setPosition(pos);
	PhysicsComponent::p_setPosition(pos.x, pos.y, pos.z);
}

void Player::setPlayAnimation(bool playAnimation)
{
	m_playAnimation = playAnimation; 
}

const bool & Player::getHeadbobbingActive() const
{
	return m_headBobbingActive; 
}

const bool & Player::getExitPause() const
{
	return m_exitPause; 
}

const float & Player::getVisability() const
{
	return m_visability;
}

const int & Player::getFullVisability() const
{
	return g_fullVisability;
}

const bool & Player::getPlayerLocked() const
{
	return m_lockPlayerInput; 
}

Animation::AnimationPlayer* Player::GetFirstPersonAnimationPlayer()
{
	return m_FirstPersonModel->getAnimationPlayer();
}

const AudioEngine::Listener & Player::getFMODListener() const
{
	return m_FMODlistener;
}

void Player::SetAbilitySet(int set)
{
	set = std::clamp(set, 1, 2);
	if (set == 1)
		m_activeSet = m_abilityComponents1;
	else if (set == 2)
		m_activeSet = m_abilityComponents2;

	m_activeSetID = set;

	SetModelAndTextures(set);
	_loadHUD();
}

void Player::SetModelAndTextures(int set)
{
	switch (set)
	{
	case 1:
	{
		this->setModel(Manager::g_meshManager.getSkinnedMesh("PLAYER1"));
		this->setTexture(Manager::g_textureManager.getTexture("PLAYER1"));
		break;
	}
	case 2:
	{
		this->setModel(Manager::g_meshManager.getSkinnedMesh("PLAYER2"));
		this->setTexture(Manager::g_textureManager.getTexture("PLAYER2"));
		break;
	}
	default:
	{
		this->setModel(Manager::g_meshManager.getSkinnedMesh("PLAYER1"));
		this->setTexture(Manager::g_textureManager.getTexture("PLAYER1"));
		break;
	}
	}
}

void Player::setEnemyPositions(std::vector<Enemy*> & enemys)
{
	using namespace DirectX;
	std::vector<DirectX::XMFLOAT2> relativEnemyPostions;
	totVis = 0;
	maxVis = 0;
	for (int i = 0; i < enemys.size(); i++)
	{

		DirectX::XMFLOAT2 pos = enemys[i]->GetDirectionToPlayer(getPosition(), *getCamera());
		if (fabs(pos.x) > 0 || fabs(pos.y) > 0)
		{
			if (enemys[i]->getPlayerVisibility()[0] > 0.0f)
			{
				relativEnemyPostions.push_back(pos);

				if (enemys[i]->getTotalVisibility() > totVis)
				{
					totVis = enemys[i]->getTotalVisibility();
					maxVis = enemys[i]->getMaxVisibility();
				}
			}
		}
	}
	XMFLOAT2A finalPos = m_HUDcircle->getPosition();
	m_currentEnemysVisable = 0;
	for (int i = 0; i < relativEnemyPostions.size() && i < MAX_ENEMY_CIRCLES; i++)
	{
		m_currentEnemysVisable++;

		m_enemyCircles[i]->setPosition(XMFLOAT2A(finalPos.x + (relativEnemyPostions[i].x * (m_HUDcircle->getScale().x / 4.0f)),
			finalPos.y + (relativEnemyPostions[i].y * (m_HUDcircle->getScale().y / 4.0f))));
	}
}

AudioEngine::SoundDesc & Player::getSmokeBombDesc() 
{
	return m_smokeBomb; 
}

const Ability Player::getCurrentAbility() const
{
	return m_currentAbility;
}

TeleportAbility * Player::getTeleportAbility()
{
	TeleportAbility* tp = (TeleportAbility *)m_abilityComponents1[0];
	return tp;
}

unsigned int Player::getNrOfRocks()
{
	return m_rockCounter;
}

bool Player::GetMapPicked()
{
	return m_MapPicked;
}

const int Player::getInteractRayId()
{
	return m_interactRayId;
}

const bool Player::sameInteractRayId(int id)
{
	if (id == m_lastInteractRayId)
	{
		m_lastInteractRayId = m_interactRayId;
		return true;
	}
	return false;
}

void Player::InstaWin()
{
	hasWon = true;
}

void Player::Draw()
{
	for (int i = 0; i < m_nrOfAbilitys; i++)
	{
		m_activeSet[i]->Draw();
	}
	Drawable::Draw();

	DrawHUDComponents();

	m_FirstPersonModel->setEntityType(EntityType::FirstPersonPlayer);
	m_FirstPersonModel->Draw();
}

void Player::DrawHUDComponents()
{
	//if our input is locked we are possessing a guard, do not draw the HUD then
	if (!IsInputLocked())
	{
		HUDComponent::HUDDraw();
		m_HUDcircleFiller->Draw();
		m_HUDcircle->Draw();
		for (unsigned short i = 0; i < m_currentEnemysVisable; i++)
		{
			m_enemyCircles[i]->Draw();
		}
		m_soundLevelHUD.Draw();
	}
}

void Player::LockPlayerInput()
{
	m_lockPlayerInput = true;
}

bool Player::IsInputLocked()
{
	return m_lockPlayerInput;
}

void Player::UnlockPlayerInput()
{
	m_lockPlayerInput = false;
}

void Player::SetCurrentVisability(const float & guard)
{
	this->m_visability = guard;
}

void Player::SetCurrentSoundPercentage(const float & percentage)
{
	this->m_soundPercentage = percentage;
	m_soundLevelHUD.forg->setV(m_soundPercentage);
	m_soundLevelHUD.forg->setColor(m_soundPercentage, 1.0f - m_soundPercentage, 1.0f - m_soundPercentage);
	m_soundLevelHUD.bckg->setOutlineColor(m_soundPercentage, 1.0f - m_soundPercentage, 0.0f, 0.5f);
}

void Player::SetFirstPersonModel()
{
	if (m_FirstPersonModel)
	{
		delete m_FirstPersonModel;
	}
	m_FirstPersonModel = new BaseActor();
	m_FirstPersonModel->CastShadows(false);
	m_FirstPersonModel->setEntityType(EntityType::FirstPersonPlayer);

	auto fpsmodel = Manager::g_meshManager.getSkinnedMesh("ARMS");
	m_FirstPersonModel->setModel(fpsmodel);
	m_FirstPersonModel->setTexture(Manager::g_textureManager.getTexture("ARMS"));

	//Animation stuff
	{

		///---Clips and poses---
		auto idleClip = Manager::g_animationManager.getAnimation("ARMS", "IDLE_ANIMATION").get();
		auto runClip = Manager::g_animationManager.getAnimation("ARMS", "RUN_ANIMATION").get();
		auto bobClip = Manager::g_animationManager.getAnimation("ARMS", "BOB_ANIMATION").get();
		auto thrwRdyClip = Manager::g_animationManager.getAnimation("ARMS", "THROW_READY_ANIMATION").get();
		auto thrwThrwClip = Manager::g_animationManager.getAnimation("ARMS", "THROW_THROW_ANIMATION").get();
		auto phaseClip = Manager::g_animationManager.getAnimation("ARMS", "PHASE_ANIMATION").get();
		auto turnLeftPose = &Manager::g_animationManager.getAnimation("ARMS", "TURN_LEFT_ANIMATION").get()->m_SkeletonPoses[0];
		auto turnRightPose = &Manager::g_animationManager.getAnimation("ARMS", "TURN_RIGHT_ANIMATION").get()->m_SkeletonPoses[0];
		///---------------------
		auto animPlayer = m_FirstPersonModel->getAnimationPlayer();
		animPlayer->SetSkeleton(Manager::g_animationManager.getSkeleton("ARMS"));
		auto& machine = animPlayer->InitStateMachine(3);

		auto runState = machine->AddLoopState("run", runClip);
		runState->SetDefaultBlendTime(.65f);

		auto idleState = machine->AddLoopState("idle", idleClip);
		idleState->SetDefaultBlendTime(.05f);
		idleState->SetSpecificBlendTime(runState, 0.3f);

		auto& idleToRun = idleState->AddOutState(runState);
		auto& runToIdle = runState->AddOutState(idleState);
		idleToRun.AddTransition(&m_moveSpeed, MOVE_SPEED * SPRINT_MULT - 0.01f, SM::COMPARISON_GREATER_THAN);
		runToIdle.AddTransition(&m_moveSpeed, MOVE_SPEED * SPRINT_MULT - 0.01f, SM::COMPARISON_LESS_THAN);

		auto throwReadyState = machine->AddPlayOnceState("throw_ready", thrwRdyClip);
		throwReadyState->SetDefaultBlendTime(0.1f);
		auto phaseState = machine->AddAutoTransitionState("phase", phaseClip, idleState);
		phaseState->SetDefaultBlendTime(0.001f);
		machine->SetState("idle");


		auto throwFinishState = machine->AddAutoTransitionState("throw_throw", thrwThrwClip, idleState);
		throwFinishState->SetDefaultBlendTime(0.0f);
		idleState->SetSpecificBlendTime(throwFinishState, .4f);

		auto& layerMachine = animPlayer->InitLayerMachine(Manager::g_animationManager.getSkeleton("ARMS").get());
		auto additiveState = layerMachine->AddBasicLayer("bob", bobClip, .3f, .3f);
		auto turnState = layerMachine->Add1DPoseLayer("turn", &m_currentTurnSpeed, -180.0f, 180.0f, { {turnLeftPose, -180.0f}, {turnRightPose, 180.0f} });
		additiveState->MakeDriven(&m_currentSpeed, 0.0, 1.5, true);
		layerMachine->ActivateLayer("bob");
		layerMachine->ActivateLayer("turn");

		animPlayer->Play();
	}
}

void Player::setHeadbobbingActive(bool active)
{
	m_headBobbingActive = active; 
}

void Player::SendOnUpdateMessage()
{
	Network::ENTITYUPDATEPACKET packet = Network::ENTITYUPDATEPACKET(
		Network::ID_PLAYER_UPDATE,
		Network::Multiplayer::GetInstance()->GetNetworkID(),
		PlayerState::Idle,
		this->getPosition(),
		this->getCamera()->getYRotationEuler()
	);


	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), PacketPriority::LOW_PRIORITY);
}

void Player::SendOnAbilityUsed()
{
	using namespace Network;
	ENTITYABILITYPACKET packet;

	//Same for every ability packet
	packet.id = ID_TIMESTAMP;
	packet.timeStamp = RakNet::GetTime();
	packet.m_id = ID_PLAYER_ABILITY;
	packet.ability = (unsigned int)NONE;
	packet.isCommonUpadate = false;


	TeleportAbility * tp_ptr = dynamic_cast<TeleportAbility*>(m_abilityComponents1[m_currentAbility]);
	DisableAbility * dis_ptr = dynamic_cast<DisableAbility*>(m_abilityComponents1[m_currentAbility]);
	//VisabilityAbility * vis_ptr = dynamic_cast<VisabilityAbility*>(m_abilityComponents1[m_currentAbility]);
	//unique based on active ability
	switch (this->m_currentAbility)
	{
	case Ability::TELEPORT:
		if (m_activeSetID == 1)
		{
			packet.ability = (unsigned int)TELEPORT;
			packet.start = tp_ptr->getStart();
			packet.velocity = tp_ptr->getVelocity();
			packet.state = tp_ptr->getState();
		}
		break;
	case Ability::DISABLE:
		if (m_activeSetID == 1)
		{
			packet.ability = (unsigned int)DISABLE;
			packet.start = dis_ptr->getStart();
			packet.velocity = dis_ptr->getVelocity();
			packet.state = dis_ptr->getState();
		}
		break;
	}

	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(ENTITYABILITYPACKET), PacketPriority::LOW_PRIORITY);
}

void Player::SendAbilityUpdates()
{
	using namespace Network;
	ENTITYABILITYPACKET packet;

	//Same for every ability packet
	packet.id = ID_TIMESTAMP;
	packet.timeStamp = RakNet::GetTime();
	packet.m_id = ID_PLAYER_ABILITY;
	packet.ability = (unsigned int)NONE;
	packet.isCommonUpadate = true;

	if (m_activeSetID == 1)
	{
		TeleportAbility * tp_ptr = dynamic_cast<TeleportAbility*>(m_abilityComponents1[Ability::TELEPORT]);
		DisableAbility * dis_ptr = dynamic_cast<DisableAbility*>(m_abilityComponents1[Ability::DISABLE]);

		packet.ability = (unsigned int)TELEPORT;
		packet.start = tp_ptr->getStart();
		packet.velocity = tp_ptr->getVelocity();
		packet.state = tp_ptr->getState();

		Network::Multiplayer::SendPacket((const char*)&packet, sizeof(ENTITYABILITYPACKET), PacketPriority::LOW_PRIORITY);

		packet.ability = (unsigned int)DISABLE;
		packet.start = dis_ptr->getStart();
		packet.velocity = dis_ptr->getVelocity();
		packet.state = dis_ptr->getState();

		Network::Multiplayer::SendPacket((const char*)&packet, sizeof(ENTITYABILITYPACKET), PacketPriority::LOW_PRIORITY);
	}
}

void Player::SendOnAnimationUpdate(double dt)
{
	using namespace DirectX;

	static double accumulatedTime = 0.0;
	static const double FREQUENCY = 1.0 / 50.0; //20 ms
	accumulatedTime += dt;

	if (accumulatedTime >= FREQUENCY)
	{
		accumulatedTime -= FREQUENCY;

		{
			///Speed
			b3Vec3 physSpeed = this->getLiniearVelocity();

			//if y speed is zero, set isInAir to false,
			if (std::abs(physSpeed.y) < 0.001f)
				m_isInAir = false;

			float speed = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSet(physSpeed.x, 0.0, physSpeed.z, 0)));
			m_currentSpeed = std::clamp(std::fabs(speed), 0.0f, 6.0f);

			///Walk dir
				//Get camera direction and normalize on X,Z plane
			XMFLOAT4A cameraDir = p_camera->getDirection();
			XMVECTOR cameraDirNormalized = XMVector3Normalize(XMVectorSet(cameraDir.x, 0.0f, cameraDir.z, 0.0));
			///assert(XMVectorGetX(XMVector3Length(cameraDirNormalized)) != 0.0f);

			XMVECTOR XZCameraDir = XMVectorSet(cameraDir.x, 0.0, cameraDir.z, 0.0);
			XMVECTOR XZMovement = XMVectorSet(physSpeed.x, 0.0, physSpeed.z, 0.0);
			XMVECTOR XZCameraDirNormalized = XMVector3Normalize(XZCameraDir);
			XMVECTOR XZMovementNormalized = XMVector3Normalize(XZMovement);
			///AssertHasLength(XZCameraDir);
			//AssertHasLength(XZMovement);

				//Get dot product of cam dir and player movement
			float dot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMVectorSet(physSpeed.x, 0, physSpeed.z, 0.0)), cameraDirNormalized));
			dot = std::clamp(dot, -0.999999f, 0.999999f);
			//Convert to degrees
			m_currentDirection = XMConvertToDegrees(std::acos(dot));
			//Negate if necessary
			float inverter = (XMVectorGetY(XMVector3Cross(XZMovement, XZCameraDir)));

			m_currentDirection *= (inverter > 0.0)
				? -1.0
				: 1.0;
			m_currentDirection = std::clamp(m_currentDirection, -180.0f, 180.0f);
			///AssertNotNAN(m_currentDirection);
		}
		Network::ENTITYANIMATIONPACKET packet(
			Network::ID_PLAYER_ANIMATION,
			Network::Multiplayer::GetInstance()->GetNetworkID(),
			this->m_currentDirection,
			this->m_currentSpeed,
			this->m_currentPitch,
			this->m_currentPeek,
			this->getCamera()->getYRotationEuler());
		Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::ENTITYANIMATIONPACKET), PacketPriority::LOW_PRIORITY);
	}
}

void Player::RegisterThisInstanceToNetwork()
{
	Network::Multiplayer::addToOnSendFuncMap("Jump", std::bind(&Player::SendOnUpdateMessage, this));
	Network::Multiplayer::addToOnSendFuncMap("MoveRight", std::bind(&Player::SendOnUpdateMessage, this));
	Network::Multiplayer::addToOnSendFuncMap("MoveLeft", std::bind(&Player::SendOnUpdateMessage, this));
	Network::Multiplayer::addToOnSendFuncMap("MoveForward", std::bind(&Player::SendOnUpdateMessage, this));
	Network::Multiplayer::addToOnSendFuncMap("MoveBackward", std::bind(&Player::SendOnUpdateMessage, this));
	Network::Multiplayer::addToOnSendFuncMap("AbilityPressed", std::bind(&Player::SendOnAbilityUsed, this));
	Network::Multiplayer::addToOnSendFuncMap("AbilityReleased", std::bind(&Player::SendOnAbilityUsed, this));
	Network::Multiplayer::addToOnSendFuncMap("Ability2Pressed", std::bind(&Player::SendOnAbilityUsed, this));
	Network::Multiplayer::addToOnSendFuncMap("Ability2Released", std::bind(&Player::SendOnAbilityUsed, this));
}

void Player::_collision()
{
	ContactListener::S_Contact con;
	std::string tagA, tagB;

	for (int i = 0; i < (int)RipExtern::g_contactListener->GetNrOfEndContacts(); i++)
	{
		con = RipExtern::g_contactListener->GetEndContact(i);
		tagA = con.a->GetBody()->GetObjectTag();
		tagB = con.b->GetBody()->GetObjectTag();

		if ( tagA == "HEADO" || tagB == "HEADO")
		{
				m_allowPeek = true;
				m_recentHeadCollision = true;
		}
	}
	for (int i = 0; i < (int)RipExtern::g_contactListener->GetNrOfBeginContacts(); i++)
	{
		con = RipExtern::g_contactListener->GetBeginContact(i);
		tagA = con.a->GetBody()->GetObjectTag();
		tagB = con.b->GetBody()->GetObjectTag();

		if ((tagA == "HEADO" || tagB == "HEADO") && !(con.a->IsSensor() || con.b->IsSensor()) )
		{
			m_allowPeek = false;
			peekDir = -LastPeekDir;
			m_peekRangeA = m_peektimer;
			m_peekRangeB = 0;
		}

		if ((tagA == "ENEMY" && tagB == "PLAYER"))
		{
			Enemy * ptrE = (Enemy*)con.a->GetBody()->GetUserData();
			AIState state = ptrE->GetState();

			if (state != AIState::Disabled && state != AIState::Possessed && state != AIState::NoState)
				hasLost = true;
		}
		else if ((tagA == "PLAYER" && tagB == "ENEMY"))
		{
			Enemy * ptrE = (Enemy*)con.b->GetBody()->GetUserData();
			AIState state = ptrE->GetState();

			if (state != AIState::Disabled && state != AIState::Possessed && state != AIState::NoState)
				hasLost = true;
		}
	}
}

void Player::_handleInput(double deltaTime)
{
	if (Input::MouseLock() && !m_kp.unlockMouse)
	{
		m_kp.unlockMouse = true;
		unlockMouse = !unlockMouse;
	}
	else if (!Input::MouseLock())
		m_kp.unlockMouse = false;

	if (Input::OnAbilityPressed() && !Input::OnAbility2Pressed())
		m_currentAbility = (Ability)0;
	else if (Input::OnAbility2Pressed() && !Input::OnAbilityPressed())
		m_currentAbility = (Ability)1;


	_onSprint();
	_onCrouch();
	_scrollMovementMod();
	_onMovement(deltaTime);
	//_onJump();
	//_onAbility(deltaTime);
	_onInteract();
	_onPeak(deltaTime);
	_onRotate(deltaTime);
	_objectInfo(deltaTime);
}

void Player::_onMovement(double deltaTime)
{
	using namespace DirectX;
	XMFLOAT4A forward = p_camera->getDirection();

	float yDir = forward.y;
	XMFLOAT4 UP = XMFLOAT4(0, 1, 0, 0);
	XMFLOAT4 RIGHT;
	//GeT_RiGhT;

	XMVECTOR vForward = XMLoadFloat4A(&forward);
	XMVECTOR vUP = XMLoadFloat4(&UP);
	XMVECTOR vRight;

	vRight = XMVector3Normalize(XMVector3Cross(vUP, vForward));
	vForward = XMVector3Normalize(XMVector3Cross(vRight, vUP));

	XMStoreFloat4A(&forward, vForward);
	XMStoreFloat4(&RIGHT, vRight);
	float x = 0;
	float z = 0;

	DirectX::XMFLOAT2 dir = { Input::MoveRight(), Input::MoveForward() };
	DirectX::XMVECTOR vDir = DirectX::XMLoadFloat2(&dir);
	float length = DirectX::XMVectorGetX(DirectX::XMVector2Length(vDir));
	if (length > 1.0)
		vDir = DirectX::XMVector2Normalize(vDir);
	DirectX::XMStoreFloat2(&dir, vDir);


	if (fabs(Input::MoveRight()) > 1.0f || fabs(Input::MoveForward()) > 1.0f)
	{
		dir.x *= m_scrollMoveModifier;
		dir.y *= m_scrollMoveModifier;
	}

	x = dir.x * m_moveSpeed  * RIGHT.x;
	x += dir.y * m_moveSpeed * forward.x;
	z = dir.y * m_moveSpeed * forward.z;
	z += dir.x * m_moveSpeed * RIGHT.z;

	m_currentMoveSpeed = DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMVectorSet(x, z, 0, 0)));

	if (Input::MoveForward() != 0 || Input::MoveRight() != 0)
	{
		m_VlastSpeed = DirectX::XMVECTOR{ x,getLiniearVelocity().y,z };
	}


	if (Input::MoveForward() == 0 && Input::MoveRight() == 0)
	{
		DirectX::XMVECTOR end = DirectX::XMVECTOR{ 0,getLiniearVelocity().y,0 };
		m_VlastSpeed = DirectX::XMVectorLerp(m_VlastSpeed, end, deltaTime * 9);

		x = DirectX::XMVectorGetX(m_VlastSpeed);
		z = DirectX::XMVectorGetZ(m_VlastSpeed);
	}
	setLiniearVelocity(x, getLiniearVelocity().y, z);


	if (RipExtern::g_rayListener->hasRayHit(m_headBobRayId))
	{
		RayCastListener::Ray& ray = RipExtern::g_rayListener->ConsumeProcessedRay(m_headBobRayId);
		RayCastListener::RayContact& con = ray.GetRayContact(0);
		for (int i = 0; i < ray.getNrOfContacts(); i++)
		{
			con = ray.GetRayContact(i);
			if (con.contactShape->GetObjectTag() == "NULL")
			{
				if (fabs(con.normal.y) < 0.999f)
				{
					p_setPosition(getPosition().x, getPosition().y, getPosition().z);
					break;
				}
			}
		}
	}
	if (Input::MoveForward() == 0 && Input::MoveRight() == 0)
	{
		if (m_headBobRayId = -100)
			m_headBobRayId = RipExtern::g_rayListener->PrepareRay(this->getBody(), p_camera->getPosition(), DirectX::XMFLOAT4A{ 0,-1,0,0 }, 1.0f);
	}
}

void Player::_scrollMovementMod()
{
	float moveMod = Input::MouseMovementModifier();
	if (moveMod != 0.0f)
	{
		m_scrollMoveModifier += 0.05*moveMod;
		m_scrollMoveModifier = std::clamp(m_scrollMoveModifier, 0.2f, 0.9f);
	}

	if (Input::ResetMouseMovementModifier())
	{
		m_scrollMoveModifier = 0.9f;
	}
}

void Player::_onSprint()
{
	if(p_moveState != Crouching)
		if (Input::MoveForward() != 0 || Input::MoveRight() != 0)
		{
			unsigned int sprintInputType = Input::Sprinting();

			if (sprintInputType == Input::InputType::Gamepad && Input::MoveForward() > 0.9)
			{
				if (p_moveState != Sprinting && !m_toggleSprint)
					_startSprint(sprintInputType);
				else if (p_moveState == Sprinting && !m_toggleSprint)
					_startWalk();
				m_toggleSprint = true;//Gamepad Input
			}
			else if (sprintInputType == Input::InputType::Keyboard && Input::MoveForward() > 0.9)
				_startSprint(sprintInputType); //Keyboard Input
			else if (p_moveState == Idle || m_prevSprintInputType == Input::InputType::Keyboard || (m_prevSprintInputType == Input::InputType::Gamepad && Input::MoveForward() < 0.9))
				_startWalk(); //No Input
			else
				m_toggleSprint = false;
		}
		else
		{
			m_moveSpeed = 0;
			p_moveState = Idle;
		}
}

void Player::_onCrouch()
{
	unsigned int crouchInputType = Input::Crouch();
	if (crouchInputType == Input::InputType::Gamepad)
	{
		if (p_moveState != Crouching && !m_toggleCrouch)
			_activateCrouch(crouchInputType);
		else if (p_moveState == Crouching && !m_toggleCrouch)
			_deActivateCrouch(); 			
		m_toggleCrouch = true;
	}
	else if(crouchInputType == Input::InputType::Keyboard)
	{
		if (p_moveState != Crouching)
			_activateCrouch(crouchInputType);
		else
			m_prevClickCrouch = Input::InputType::Keyboard;
	}
	else
	{
		m_toggleCrouch = false;
		if (m_prevClickCrouch == Input::InputType::Keyboard && p_moveState == Crouching)
			_deActivateCrouch();
	}
}

void Player::_onRotate(double deltaTime)
{
	static DirectX::XMFLOAT2 s_rot = { 0.0f, 0.0f };

	if (!unlockMouse)
	{
		const float deltaY = Input::TurnUp();
		const float deltaX = Input::TurnRight();

		if (deltaY || deltaX)
		{
			s_rot.x -= deltaX * Input::GetPlayerMouseSensitivity().x * deltaTime;
			s_rot.y -= deltaY * Input::GetPlayerMouseSensitivity().y * deltaTime;

			if (s_rot.y > 88.0f)
				s_rot.y = 88.0f;
			else if (s_rot.y < -88.0f)
				s_rot.y = -88.0f;

			const float radX = DirectX::XMConvertToRadians(s_rot.x);
			const float radY = DirectX::XMConvertToRadians(s_rot.y);

			DirectX::XMFLOAT4A lookTo = { 0.0f,0.0f,0.0f,0.0f };
			lookTo.x = cos(radX) * cos(radY);
			lookTo.y = sin(radY);
			lookTo.z = sin(radX) * cos(radY);
			p_camera->setLookTo(lookTo);
		}
	}
	m_currentPitch = -p_camera->getPitch().x;
}

void Player::_onPeak(double deltaTime)
{
	if (Input::PeekRight() != 0) //Cap max peak range based on key input
	{
		if (m_allowPeek)
		{
			if (m_recentHeadCollision)
			{
				peekDir = LastPeekDir;
				m_peekRangeA = m_peektimer;
				m_peekRangeB = 0;
			}
			else
			{
				if (fabs(Input::PeekRight()) < fabs(m_peektimer))
				{
					m_peekRangeA = m_peektimer;
					m_peekRangeB = -m_peektimer;
				}
				else
				{
					m_peekRangeA = Input::PeekRight();
					m_peekRangeB = -Input::PeekRight();
				}

				if (Input::PeekRight() > 0) //Left Side
					peekDir = 1;
				if (Input::PeekRight() < 0) //Right Side
					peekDir = -1;

				if (peekDir != LastPeekDir) //Change in Direction
				{
					LastPeekDir = peekDir;
				}
				
			}
		}
	}
	else //Return to default pos
	{
		if (m_peekRangeA == m_peektimer)
		{
			LastPeekDir = peekDir;
		}
		peekDir = -LastPeekDir;
		m_peekRangeA = m_peektimer;
		m_peekRangeB = 0;
		m_recentHeadCollision = false;

	}

	m_currentPeek = /*peekDir * */m_peektimer;

}

void Player::_onInteract()
{
	if (RipExtern::g_rayListener->hasRayHit(m_interactRayId))
	{
		const int tempRayId = m_interactRayId;
		RayCastListener::Ray& ray = RipExtern::g_rayListener->ConsumeProcessedRay(m_interactRayId);
		RayCastListener::RayContact& con = ray.getClosestContact(true);
		/*for (int i = 0; i < ray->getNrOfContacts(); i++)
		{*/
			if (ray.getOriginBody()->GetObjectTag() == getBody()->GetObjectTag())
			{
				if (con.contactShape->GetBody()->GetObjectTag() == "ITEM")
				{
					//do the pickups
				}
				else if (con.contactShape->GetBody()->GetObjectTag() == "LEVER")
				{
					static_cast<Lever*>(con.contactShape->GetBody()->GetUserData())->handleContact(con);
				}
				else if (con.contactShape->GetBody()->GetObjectTag() == "TORCH")
				{
					static_cast<Torch*>(con.contactShape->GetBody()->GetUserData())->handleContact(con);
				}
				else if (con.contactShape->GetBody()->GetObjectTag() == "ENEMY")
				{

				}
				else if (con.contactShape->GetBody()->GetObjectTag() == "BLINK_WALL")
				{
					//std::cout << "illusory wall ahead" << std::endl;
				}
				else if (con.contactShape->GetBody()->GetObjectTag() == "ROCK_PICKUP")
				{
					//lol wtf is dis
					Rock * rock = static_cast<Rock*>(con.contactShape->GetBody()->GetUserData());
					if (m_rockCounter < MAXROCKS)
					{
						rock->DeleteRock();
						m_rockCounter++;
					}
				}
				else if (con.contactShape->GetBody()->GetObjectTag() == "MAP")
				{
					//Mange vafan, autolol p� dej
					Map * autoLol = static_cast<Map*>(con.contactShape->GetBody()->GetUserData());
					autoLol->DeleteMap();
					m_MapPicked = true;
					//std::cout << "MAP" << std::endl;
				}
				
			}
		//}
	}

	if (Input::Interact())
	{
		if (m_kp.interact == false)
		{
			if (m_interactRayId == -100)
				m_interactRayId = RipExtern::g_rayListener->PrepareRay(this->getBody(), this->getCamera()->getPosition(), this->getCamera()->getDirection(), Player::INTERACT_RANGE);

			m_kp.interact = true;
		}

	}
	else
	{
		m_kp.interact = false;
	}
}

void Player::_onAbility(double dt)
{
	this->m_activeSet[m_currentAbility]->Update(dt);
}

//Sends a ray every second and check if there is relevant data for the object to show on the screen
void Player::_objectInfo(double deltaTime)
{
	const int tempId = m_objectInfoRayId;
	//m_cross->setString("");
	if (RipExtern::g_rayListener->hasRayHit(m_objectInfoRayId))
	{
		RayCastListener::Ray& ray = RipExtern::g_rayListener->ConsumeProcessedRay(m_objectInfoRayId);
		RayCastListener::RayContact& cContact = ray.getClosestContact(true);

		/*std::string ass = cContact.contactShape->GetBody()->GetObjectTag();

		m_cross->setString(ass);*/

		float interactFractionRange = Player::INTERACT_RANGE / Player::OBJECT_INFO_RANGE;
		if (cContact.contactShape->GetBody()->GetObjectTag() == "LEVER" && cContact.fraction <= interactFractionRange)
		{
			m_cross->setUnpressedTexture("CROSSHAND");
			m_cross->setScale(DirectX::XMFLOAT2A(0.6f / 16.0, 0.6f / 9.0f));
		}
		else if (cContact.contactShape->GetBody()->GetObjectTag() == "TORCH" && cContact.fraction <= interactFractionRange)
		{
			m_cross->setUnpressedTexture("CROSSHAND");
			m_cross->setScale(DirectX::XMFLOAT2A(0.6f / 16.0, 0.6f / 9.0f));
			//Snuff out torches
		}
		else if (m_activeSetID == 2)
		{
			if (cContact.contactShape->GetBody()->GetObjectTag() == "ENEMY" && cContact.fraction <= (PossessGuard::RANGE / Player::OBJECT_INFO_RANGE))
			{
				m_cross->setUnpressedTexture("CROSSPOSSESS");
				m_cross->setScale(DirectX::XMFLOAT2A(1.2f / 16.0, 1.2f / 9.0f));
			}
			else if (cContact.contactShape->GetBody()->GetObjectTag() == "BLINK_WALL" && cContact.fraction <= (BlinkAbility::RANGE / Player::OBJECT_INFO_RANGE))
			{
				m_cross->setUnpressedTexture("CROSSPHASE");
				m_cross->setScale(DirectX::XMFLOAT2A(0.9f / 16.0, 0.9f / 9.0f));
			}
			else
			{
				m_cross->setUnpressedTexture("CROSS");
				m_cross->setScale(DirectX::XMFLOAT2A(0.1f / 16.0, 0.1f / 9.0f));
			}



		}
		else
		{
			m_cross->setUnpressedTexture("CROSS");
			m_cross->setScale(DirectX::XMFLOAT2A(0.1f / 16.0, 0.1f / 9.0f));
		}
	}
	else if (tempId != m_objectInfoRayId)
	{
		m_cross->setUnpressedTexture("CROSS");
		m_cross->setScale(DirectX::XMFLOAT2A(0.1f / 16.0, 0.1f / 9.0f));
	}
	
	if (m_objectInfoTime >= 0.1f)
	{
		if (m_objectInfoRayId == -100)
			m_objectInfoRayId = RipExtern::g_rayListener->PrepareRay(getBody(), getCamera()->getPosition(), getCamera()->getDirection(), Player::OBJECT_INFO_RANGE);

		m_objectInfoTime = 0;
	}
	m_objectInfoTime += deltaTime;
}

void Player::_updateFirstPerson(float deltaTime)
{
	using namespace DirectX;

	const auto offset = XMMatrixMultiply(XMMatrixTranspose(XMMatrixTranslation(0.0, -1.23f, -.45)), XMMatrixScaling(.1, .1, .1));
	m_FirstPersonModel->ForceWorld(XMMatrixMultiply(XMMatrixInverse(nullptr, XMLoadFloat4x4A(&CameraHolder::getCamera()->getView())), offset));

	if(m_playAnimation)
	m_FirstPersonModel->getAnimationPlayer()->Update(deltaTime);
}

void Player::_cameraPlacement(double deltaTime)
{
	//Head Movement
	b3Vec3 upperBodyLocal = this->getBody()->GetShapeList()->GetNext()->GetTransform().translation;
	b3Vec3 headPosLocal = this->getBody()->GetShapeList()->GetTransform().translation;


	//-------------------------------------------Peeking--------------------------------------------// 
	
	m_peektimer += peekDir * (float)deltaTime *m_peekSpeed;

	if (m_peekRangeB > m_peekRangeA)
		m_peektimer = std::clamp(m_peektimer, m_peekRangeA, m_peekRangeB);
	else
		m_peektimer = std::clamp(m_peektimer, m_peekRangeB, m_peekRangeA);

	//Offsets to the sides to slerp between
	b3Vec3 peekOffsetLeft;
	b3Vec3 peekOffsetRight;

	DirectX::XMFLOAT4A forward = p_camera->getDirection();

	DirectX::XMFLOAT4 UP = DirectX::XMFLOAT4(0, 1, 0, 0);
	DirectX::XMFLOAT4A right;
	DirectX::XMVECTOR vForward = DirectX::XMLoadFloat4A(&forward);
	DirectX::XMVECTOR vUP = DirectX::XMLoadFloat4(&UP);
	DirectX::XMVECTOR vRight;

	vRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vUP, vForward));
	vForward = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vRight, vUP));

	XMStoreFloat4A(&forward, vForward);
	XMStoreFloat4(&right, vRight);
	peekOffsetLeft.x = -1;
	peekOffsetLeft.y = 0.;// +((upperBodyLocal.y*0.5)* (1 - fabs(m_peektimer)));
	peekOffsetLeft.z = 1;

	peekOffsetRight.x = 1;
	peekOffsetRight.y = 0;// +((upperBodyLocal.y*0.5)* (1 - fabs(m_peektimer)));
	peekOffsetRight.z = -1;

	headPosLocal = _slerp(peekOffsetRight, peekOffsetLeft, (m_peektimer + 1)*0.5);
	headPosLocal.x *= forward.z;
	//headPosLocal.y *= forward.y;
	headPosLocal.z *= forward.x;
	headPosLocal += upperBodyLocal;
	//-------------------------------------------Crouch-------------------------------------------// 

	m_crouchAnimSteps += crouchDir * (float)deltaTime*m_crouchSpeed;
	m_crouchAnimSteps = std::clamp(m_crouchAnimSteps, 0.0f, 1.0f);
	headPosLocal.y += lerp(m_standHeight, m_crouchHeight, m_crouchAnimSteps);

	if (m_crouchAnimSteps == 1 || m_crouchAnimSteps == 0) //Animation Finished
	{
		crouchDir = 0;
	}

	//--------------------------------------Camera movement---------------------------------------// 
	b3Vec3 headPosWorld = this->getBody()->GetTransform().translation + headPosLocal;
	DirectX::XMFLOAT4A pos = DirectX::XMFLOAT4A(headPosWorld.x, headPosWorld.y, headPosWorld.z, 1.0f);
	p_camera->setPosition(pos);
	//Camera Tilt
	p_CameraTilting(deltaTime, m_peektimer);

	static float lastOffset = 0.0f;
	static bool hasPlayed = true;
	static int last = 0;

	float offsetY = 0;

	//Head Bobbing
	if (m_headBobbingActive)
	{
		offsetY = p_viewBobbing(deltaTime, m_currentMoveSpeed, this->getBody());
		pos.y += offsetY;
	}

	//Footsteps
	if (p_moveState == Walking || p_moveState == Sprinting)
	{
		if (!hasPlayed)
		{
			if (lastOffset < offsetY)
			{
				hasPlayed = true;
				auto xmPos = getPosition();
				FMOD_VECTOR at = { xmPos.x, xmPos.y, xmPos.z };
				b3Vec3 vel = getLiniearVelocity();
				DirectX::XMVECTOR vVel = DirectX::XMVectorSet(vel.x, vel.y, vel.z, 0.0f);
				float speed = DirectX::XMVectorGetX(DirectX::XMVector3Length(vVel));
				m_footSteps.loudness = speed + (speed * p_moveState * 0.5f);
				FMOD::Channel * c = nullptr;
				float vol = 1.0f;
				
				int index = -1;
				if (speed > 3.0f) // running
				{
					while (index == -1 || index == last)
					{
						index = rand() % (int)RipSounds::g_hardStep.size();
					}
					c = AudioEngine::PlaySoundEffect(RipSounds::g_hardStep[index], &at, &m_footSteps);
					vol = speed / 5.0f;
				}
				else
				{
					while (index == -1 || index == last)
					{
						index = rand() % (int)RipSounds::g_sneakStep.size();
					}
					c = AudioEngine::PlaySoundEffect(RipSounds::g_sneakStep[index], &at, &m_footSteps);
					vol = speed / 3.0f;
				}
				c->setVolume(std::clamp(vol, 0.0f, 1.0f));

				last = index;
			}
		}
		else
		{
			if (lastOffset > offsetY)
			{
				hasPlayed = false;
			}
		}

		lastOffset = offsetY;
	}

	this->getBody()->GetShapeList()->SetTransform(headPosLocal, getBody()->GetQuaternion());
	//p_camera->setPosition(pos);
}

void Player::_updateFMODListener(double deltaTime, const DirectX::XMFLOAT4A & xmLastPos)
{
	const DirectX::XMFLOAT4A & xmDir = p_camera->getDirection();
	const DirectX::XMFLOAT4A & xmPos = p_camera->getPosition();
	const DirectX::XMFLOAT4A & xmRight = p_camera->getRight();
	DirectX::XMFLOAT3 xmUp;
	DirectX::XMVECTOR vDir = DirectX::XMLoadFloat4A(&xmDir);
	DirectX::XMVECTOR vRight = DirectX::XMLoadFloat4A(&xmRight);
	DirectX::XMVECTOR vUp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vDir, vRight));
	DirectX::XMStoreFloat3(&xmUp, vUp);

	//std::cout << xmPos.x << ", " << xmPos.y << ", " << xmPos.z << std::endl;

	FMOD_VECTOR vel;
	vel.x = (xmPos.x - xmLastPos.x) / deltaTime;
	vel.y = (xmPos.y - xmLastPos.y) / deltaTime;
	vel.z = (xmPos.z - xmLastPos.z) / deltaTime;

	m_FMODlistener.pos = { xmPos.x, xmPos.y, xmPos.z };
	m_FMODlistener.up = { xmUp.x, xmUp.y, xmUp.z };
	m_FMODlistener.forward = { xmDir.x, xmDir.y, xmDir.z };
	m_FMODlistener.vel = vel;
}
void Player::_activateCrouch(const unsigned int inputType)
{
	this->getBody()->GetShapeList()->GetNext()->SetSensor(true);
	crouchDir = 1;
	m_moveSpeed = MOVE_SPEED * 0.5f;
	p_moveState = Crouching;
	m_prevClickCrouch = inputType;

	this->getAnimationPlayer()->GetLayerMachine()->ActivateLayer("crouch");
	if (Network::Multiplayer::GetInstance()->isConnected())
	{
		Network::COMMONEVENTPACKET packet(Network::NETWORKMESSAGES::ID_PLAYER_CROUCH_BEGIN);
		Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), PacketPriority::LOW_PRIORITY);
	}
}

void Player::_deActivateCrouch()
{
	this->getBody()->GetShapeList()->GetNext()->SetSensor(false);
	crouchDir = -1;
	m_moveSpeed = MOVE_SPEED;
	p_moveState = Idle;
	m_prevClickCrouch = Input::InputType::None;

	this->getAnimationPlayer()->GetLayerMachine()->PopLayer("crouch");
	if (Network::Multiplayer::GetInstance()->isConnected())
	{
		Network::COMMONEVENTPACKET packet(Network::NETWORKMESSAGES::ID_PLAYER_CROUCH_END);
		Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), PacketPriority::LOW_PRIORITY);
	}
}

void Player::_startSprint(const unsigned int inputType)
{
	m_moveSpeed = MOVE_SPEED * SPRINT_MULT;
	p_moveState = Sprinting;
	m_scrollMoveModifier = 0.9f;
	m_prevSprintInputType = inputType;
}

void Player::_startWalk()
{
	m_moveSpeed = MOVE_SPEED;
	p_moveState = Walking;
	m_prevSprintInputType = 0;
}

void Player::SendOnWinState()
{
	Network::ENTITYSTATEPACKET packet(Network::ID_PLAYER_WON, 0, this->hasWon);

	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), PacketPriority::LOW_PRIORITY);
}

void Player::_hasWon()
{
	ContactListener::S_Contact con;
	for (int i = 0; i < (int)RipExtern::g_contactListener->GetNrOfBeginContacts(); i++)
	{
		std::string Object_A_Tag = RipExtern::g_contactListener->GetBeginContact(i).a->GetBody()->GetObjectTag();
		std::string Object_B_Tag = RipExtern::g_contactListener->GetBeginContact(i).b->GetBody()->GetObjectTag();

		if (Object_A_Tag == "PLAYER" || Object_A_Tag == "WIN_BOX")
		{
			if (Object_B_Tag == "PLAYER" || Object_B_Tag == "WIN_BOX")
			{
				hasWon = true;
				SendOnWinState();
				return;
			}
		}
	}
	for (int i = 0; i < (int)RipExtern::g_contactListener->GetNrOfEndContacts(); i++)
	{
		std::string Object_A_Tag = RipExtern::g_contactListener->GetEndContact(i).a->GetBody()->GetObjectTag();
		std::string Object_B_Tag = RipExtern::g_contactListener->GetEndContact(i).b->GetBody()->GetObjectTag();

		if (Object_A_Tag == "PLAYER" || Object_A_Tag == "WIN_BOX")
		{
			if (Object_B_Tag == "PLAYER" || Object_B_Tag == "WIN_BOX")
			{
				hasWon = false;
				SendOnWinState();
				return;
			}
		}
	}

}

b3Vec3 Player::_slerp(b3Vec3 start, b3Vec3 end, float percent)
{
	// Dot product - the cosine of the angle between 2 vectors.
	float dot = b3Dot(start, end);
	// Clamp it to be in the range of Acos()
	// This may be unnecessary, but floating point
	// precision can be a fickle mistress.
	dot = std::clamp(dot, -1.0f, 1.0f);
	// Acos(dot) returns the angle between start and end,
	// And multiplying that by percent returns the angle between
	// start and the final result.
	float theta = std::acosf(dot)*percent;
	//float theta = mathf.Acos(dot)*percent;
	b3Vec3 tempStart = start;
	tempStart.x *= dot;
	tempStart.y *= dot;
	tempStart.z *= dot;
	b3Vec3 relativeVec = end - tempStart;
	b3Normalize(relativeVec);   // Orthonormal basis
	// The final result.
	tempStart = start;
	tempStart.x *= std::cos(theta);
	tempStart.y *= std::cos(theta);
	tempStart.z *= std::cos(theta);

	b3Vec3 tempRelativeVec = relativeVec;
	tempRelativeVec.x *= std::sin(theta);
	tempRelativeVec.y *= std::sin(theta);
	tempRelativeVec.z *= std::sin(theta);


	return (tempStart + tempRelativeVec);
}



void Player::setExitPause(bool exitPause)
{
	m_exitPause = exitPause; 
}

void Player::_loadHUD()
{
	_unloadHUD();

	if (m_activeSetID == 1)
		HUDComponent::InitHUDFromFile(PlayerOneHUDPath);
	else
		HUDComponent::InitHUDFromFile(PlayerTwoHUDPath);

	m_abilityCircle = new Circle*[4];
	m_abilityCircle[0] = new Circle();
	m_abilityCircle[0]->init(DirectX::XMFLOAT2A(.05f, .2f), DirectX::XMFLOAT2A(2.2f / 16.0f, 2.2f / 9.0f));
	m_abilityCircle[0]->setRadie(.53f);
	m_abilityCircle[0]->setInnerRadie(.46f);
	m_abilityCircle[0]->setUnpressedTexture("DAB");
	m_abilityCircle[0]->setAngle(360);

	m_abilityCircle[1] = new Circle();
	m_abilityCircle[1]->init(DirectX::XMFLOAT2A(.1f, .08f), DirectX::XMFLOAT2A(2.2f / 16.0f, 2.2f / 9.0f));
	m_abilityCircle[1]->setRadie(.53f);
	m_abilityCircle[1]->setInnerRadie(.46f);
	m_abilityCircle[1]->setUnpressedTexture("DAB");
	m_abilityCircle[1]->setAngle(360);

	m_abilityCircle[2] = new Circle();
	m_abilityCircle[2]->init(DirectX::XMFLOAT2A(.05f, .2f), DirectX::XMFLOAT2A(2.2f / 16.0f, 2.2f / 9.0f));
	m_abilityCircle[2]->setRadie(.44f);
	m_abilityCircle[2]->setInnerRadie(.40f);
	m_abilityCircle[2]->setUnpressedTexture("DAB");
	m_abilityCircle[2]->setAngle(360);
	m_abilityCircle[2]->setColor(1,0,0,1);

	m_abilityCircle[3] = new Circle();
	m_abilityCircle[3]->init(DirectX::XMFLOAT2A(.1f, .08f), DirectX::XMFLOAT2A(2.2f / 16.0f, 2.2f / 9.0f));
	m_abilityCircle[3]->setRadie(.44f);
	m_abilityCircle[3]->setInnerRadie(.40f);
	m_abilityCircle[3]->setUnpressedTexture("DAB");
	m_abilityCircle[3]->setAngle(360);
	m_abilityCircle[3]->setColor(1, 0, 0, 1);

	

	HUDComponent::AddQuad(m_abilityCircle[0]);
	HUDComponent::AddQuad(m_abilityCircle[1]);
	HUDComponent::AddQuad(m_abilityCircle[2]);
	HUDComponent::AddQuad(m_abilityCircle[3]);

	m_cross = HUDComponent::GetQuad("Cross");
	m_cross->setScale(DirectX::XMFLOAT2A(.1f / 16.0, .1f / 9.0f));


	m_HUDcircle = dynamic_cast<Circle*>(HUDComponent::GetQuad("ViewCircle"));
	m_HUDcircle->setScale(DirectX::XMFLOAT2A(m_HUDcircle->getScale().x / 16.0f, m_HUDcircle->getScale().y / 9.0f));
	dynamic_cast<Circle*>(m_HUDcircle)->setInnerRadie(0.45f);


	m_HUDcircleFiller = dynamic_cast<Circle*>(HUDComponent::GetQuad("ViewFiller"));
	m_HUDcircleFiller->setScale(DirectX::XMFLOAT2A(m_HUDcircleFiller->getScale().x / 16.0f, m_HUDcircleFiller->getScale().y / 9.0f));
	dynamic_cast<Circle*>(m_HUDcircleFiller)->setInnerRadie(-1.0f);

	if (!m_enemyCircles.empty())
	{
		for (int i = 0; i < m_enemyCircles.size(); i++)
		{
			m_enemyCircles[i]->Release();
			delete m_enemyCircles[i];
		}
		m_enemyCircles.clear();
	}
	for (int i = 0; i < MAX_ENEMY_CIRCLES; i++)
	{
		Circle * c = new Circle();
		c->init(DirectX::XMFLOAT2A(0.95f, 0.15f), DirectX::XMFLOAT2A(.25f / 16.0f, .25f / 9.0f));
		c->setRadie(.5f);
		c->setUnpressedTexture("SPHERE");
		m_enemyCircles.push_back(c);
	}
}

void Player::_unloadHUD()
{
	HUDComponent::removeHUD();
}

void Player::_initSoundHUD()
{
	Quad * soundBack = new Quad;
	Quad * soundfor = new Quad;
	float outline = 5.0f;
	DirectX::XMFLOAT2 scl = { 105.0f, 247.0f };

	/*	soundBack->init({ 0.025f, 0.97f }, { scl.x / InputHandler::getViewportSize().x, scl.y / InputHandler::getViewportSize().y });
		soundBack->setUnpressedTexture("WHITE");
		soundBack->setType(Quad::QuadType::Outlined);
		soundBack->setRadie(outline);
		soundBack->setPivotPoint(Quad::PivotPoint::upperLeft);
		soundBack->setOutlineColor(1, 1, 0, 0.5f);
		soundBack->setColor(0.2f, 0.0f, 0.8f, 0.3f);
		*/
	soundfor->init(
		{ 0.031f, 0.965f },
		{ scl.x / 1280.0f, scl.y / 720.0f});


	soundfor->setUnpressedTexture("DAB");
	soundfor->setPivotPoint(Quad::PivotPoint::upperLeft);
	soundfor->setColor(0, 0, 1);

	m_soundLevelHUD.bckg = soundBack;
	m_soundLevelHUD.forg = soundfor;
}

void Player::_cheats()
{
	//Swap ability set cheat
	if (InputHandler::isKeyPressed(InputHandler::Ctrl) && InputHandler::wasKeyPressed('O'))
	{
		if (m_activeSetID == 1)
		{
			SetAbilitySet(2);
		}
		else if (m_activeSetID == 2)
		{
			SetAbilitySet(1);
		}
	}
}


