#include "RipTagPCH.h"
#include "Enemy.h"

#include "EngineSource/3D Engine/Extern.h"
#include "EngineSource/3D Engine/RenderingManager.h"
#include "EngineSource/3D Engine/Components/Camera.h"

#include "EngineSource/3D Engine/Model/Managers/MeshManager.h"

#include "EngineSource/3D Engine/Model/Managers/TextureManager.h"
#include "EngineSource/3D Engine/3DRendering/Rendering/VisabilityPass/Component/VisibilityComponent.h"
#include "2D Engine/Quad/Components/HUDComponent.h"

Enemy::Enemy(b3World* world, unsigned int id, float startPosX, float startPosY, float startPosZ) : Actor(), CameraHolder(), PhysicsComponent(), AI(this)
{
	this->uniqueID = id;
	this->p_initCamera(new Camera(DirectX::XMConvertToRadians(150.0f / 2.0f), 250.0f / 150.0f, 0.1f, 20.0f));
	m_vc = new VisibilityComponent();
	m_vc->Init(this->p_camera);
	this->setDir(1, 0, 0);
	this->getCamera()->setFarPlane(20);
	this->setModel(Manager::g_meshManager.getSkinnedMesh("GUARD"));
	this->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	this->getAnimationPlayer()->SetSkeleton(Manager::g_animationManager.getSkeleton("GUARD"));


	auto& layerMachine = this->getAnimationPlayer()->InitLayerMachine(Manager::g_animationManager.getSkeleton("GUARD").get());
	layerMachine->AddBasicLayer
		("test_layer", Manager::g_animationManager.getAnimation("GUARD", "HEADTURN_ANIMATION").get(), 0.2f, 1.0f);

	{

		{
			std::vector<SharedAnimation> sharedAnimations;
			const char * collection = "PLAYER1";
			int nrOfStates = 8;

			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "IDLE_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_F_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_B_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_FL_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_FR_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_BL_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_BR_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "R_F_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "R_FL_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "R_FR_ANIMATION"));

			this->getAnimationPlayer()->Play();
			this->getAnimationPlayer()->SetSkeleton(Manager::g_animationManager.getSkeleton("GUARD"));

			std::unique_ptr<SM::AnimationStateMachine>& stateMachine = this->getAnimationPlayer()->InitStateMachine(nrOfStates);

			{
				//Blend spaces - forward&backward
				SM::BlendSpace2D * blend_fwd = stateMachine->AddBlendSpace2DState(
					"walk_forward", //state name
					&this->m_currentDirection, //x-axis driver
					&this->m_currentMoveSpeed, //y-axis driver
					-115.f, 115.f, //x-axis bounds
					0.0f, 4.0f //y-axis bounds
				);
				SM::BlendSpace2D * blend_bwd = stateMachine->AddBlendSpace2DState(
					"walk_backward", //state name
					&this->m_currentDirection, //x-axis driver
					&this->m_currentMoveSpeed, //y-axis driver
					-180.f, 180.f, //x-axis bounds
					0.0f, 4.f //y-axis bounds
				);

				//Add blendspace rows 
				//forward
				blend_fwd->AddRow(
					0.0f, //y placement
					{	//uses a vector initializer list for "convinience"
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), -115.f }, //the clip to use and x-placement
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), 0.f },
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), 115.f }
					}
				);
				blend_fwd->AddRow(
					4.f, //y placement
					{	//uses a vector initializer list for "convinience"
						{ sharedAnimations[RemotePlayer::AnimState::RIGHT].get(), -115.f }, //the clip to use and x-placement
						{ sharedAnimations[RemotePlayer::AnimState::FORWARD].get(), 0.f },
						{ sharedAnimations[RemotePlayer::AnimState::LEFT].get(), 115.f }
					}
				);
				blend_fwd->AddRow(
					6.0f, //y placement
					{	//uses a vector initializer list for "convinience"
						{ sharedAnimations[RemotePlayer::AnimState::RUN_FORWARD_RIGHT].get(), -115.f }, //the clip to use and x-placement
						{ sharedAnimations[RemotePlayer::AnimState::RUN_FORWARD].get(), 0.f },
						{ sharedAnimations[RemotePlayer::AnimState::RUN_FORWARD_LEFT].get(), 115.f }
					}
				);
				//
				blend_bwd->AddRow(
					0.0f, //y placement
					{	//uses a vector initializer list for "convinience"
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), -180.f }, //the clip to use and x-placement
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), -90.f },
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), 0.f },
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), 90.f },
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), 180.f }
					}
				);
				blend_bwd->AddRow(
					4.f, //y placement
					{	//uses a vector initializer list for "convinience"
						{ sharedAnimations[RemotePlayer::AnimState::BACKWARD].get(), -180.f }, //the clip to use and x-placement
						{ sharedAnimations[RemotePlayer::AnimState::BACK_RIGHT].get(), -90.f },
						{ sharedAnimations[RemotePlayer::AnimState::FORWARD].get(), 0.f },
						{ sharedAnimations[RemotePlayer::AnimState::BACK_LEFT].get(), 90.f },
						{ sharedAnimations[RemotePlayer::AnimState::BACKWARD].get(), 180.f }
					}
				);
				blend_bwd->AddRow(
					6.0f, //y placement
					{	//uses a vector initializer list for "convinience"
						{ sharedAnimations[RemotePlayer::AnimState::BACKWARD].get(), -180.f }, //the clip to use and x-placement
						{ sharedAnimations[RemotePlayer::AnimState::BACK_RIGHT].get(), -90.f },
						{ sharedAnimations[RemotePlayer::AnimState::FORWARD].get(), 0.f },
						{ sharedAnimations[RemotePlayer::AnimState::BACK_LEFT].get(), 90.f },
						{ sharedAnimations[RemotePlayer::AnimState::BACKWARD].get(), 180.f }
					}
				);

				//Adding out state / transitions
				SM::OutState & fwd_bwd_outstate = blend_fwd->AddOutState(blend_bwd);
				//Add transition condition
				fwd_bwd_outstate.AddTransition(
					&this->m_currentDirection, //referenced variable for comparision
					-115.f, 115.f, //bound range for comparision
					SM::COMPARISON_OUTSIDE_RANGE //comparision condition
				);

				SM::OutState & bwd_fwd_outstate = blend_bwd->AddOutState(blend_fwd);
				//Add transition condition
				bwd_fwd_outstate.AddTransition(
					&this->m_currentDirection, //referenced variable for comparision
					-90.f, 90.f, //bound range for comparision
					SM::COMPARISON_INSIDE_RANGE //comparision condition
				);

				auto crouchClip = Manager::g_animationManager.getAnimation(collection, "CROUCH_POSE_ANIMATION").get();

				auto leanLeftPose = &Manager::g_animationManager.getAnimation(collection, "LEAN_LEFT_ANIMATION").get()->m_SkeletonPoses[0];
				auto leanRightPose = &Manager::g_animationManager.getAnimation(collection, "LEAN_RIGHT_ANIMATION").get()->m_SkeletonPoses[0];

				auto crouchState = layerMachine->AddBasicLayer("crouch", crouchClip, 0.0, 0.0);
				crouchState->UseFirstPoseOnly(true);


				auto idleAnim = Manager::g_animationManager.getAnimation("GUARD", "IDLE_ANIMATION").get();
				auto walkAnim = Manager::g_animationManager.getAnimation("GUARD", "WALK_ANIMATION").get();
				auto awareAnim = Manager::g_animationManager.getAnimation("GUARD", "AWARE_ANIMATION").get();
				auto knockAnim = Manager::g_animationManager.getAnimation("GUARD", "KNOCKED_ANIMATION").get();
				auto walkState = stateMachine->AddBlendSpace1DState("walk_state", &m_currentMoveSpeed, 0.0, 1.5f);
				walkState->AddBlendNodes({ {idleAnim, 0.0}, {walkAnim, 1.5f} });
				walkState->SetDefaultBlendTime(0.2f);
				auto awareState = stateMachine->AddLoopState("aware", awareAnim);
				awareState->SetDefaultBlendTime(0.4f);

				auto knockState = stateMachine->AddLoopState("knocked_state", knockAnim);
				knockState->SetDefaultBlendTime(.1f);
				stateMachine->SetState("walk_state");

				//Out states
				auto& guardWalkToPlayerWalk = walkState->AddOutState(blend_fwd);
				guardWalkToPlayerWalk.AddTransition(&m_IsPossessedByTeammate, true, SM::COMPARISON_EQUAL);

				auto& playerWalkToGuardWalk = blend_fwd->AddOutState(walkState);
				playerWalkToGuardWalk.AddTransition(&m_IsPossessedByTeammate, false, SM::COMPARISON_EQUAL);

				auto& playerWalkBackToGuardWalk = blend_bwd->AddOutState(walkState);
				playerWalkBackToGuardWalk.AddTransition(&m_IsPossessedByTeammate, false, SM::COMPARISON_EQUAL);

				auto& guardAwareToPlayerWalk = awareState->AddOutState(blend_fwd);
				guardAwareToPlayerWalk.AddTransition(&m_IsPossessedByTeammate, true, SM::COMPARISON_EQUAL);
			}
		}


		this->getAnimationPlayer()->Play();

	}
	b3Vec3 pos = b3Vec3(1.0f, 1.25f, 1.0f);
	PhysicsComponent::Init(*world, e_dynamicBody, pos.x/2, pos.y/2, pos.z/2, false, 0); //0.5f, 0.9f, 0.5f //1,0.9,1

	//this->getBody()->SetUserData(Enemy::validate());
	this->getBody()->SetObjectTag("ENEMY");
	CreateShape(b3Vec3(0, pos.y*0.70, 0), b3Vec3(pos.x / 2, pos.y / 2, pos.z / 2), 1.0f, 1.0f, "UPPERBODY");
	CreateShape(b3Vec3(0, pos.y*1.5, 0), b3Vec3(0.3f, 0.35f, 0.3f), 1.0f, 1.0f, "HEAD", true);
	m_standHeight = (pos.y*1.5);
	m_crouchHeight = pos.y * 0.70;
	this->getBody()->SetUserData(this);

	this->setEntityType(EntityType::GuarddType);
	this->setPosition(startPosX, startPosY, startPosZ);
	//setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	this->setModelTransform(DirectX::XMMatrixTranslation(0.0, -0.95, 0.0));
	setScale(.4, .4, .4);
	setTexture(Manager::g_textureManager.getTexture("GUARD"));
	//setTextureTileMult(1, 2);
	m_boundingFrustum = new DirectX::BoundingFrustum(DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&p_camera->getProjection())));

	this->p_camera->setPerspectiv(Camera::Perspectiv::Enemy);

	m_soundFootstep.emitter = AudioEngine::SoundEmitterType::Enemy;
	m_soundFootstep.owner = this;
	//setOutline(true);
}


Enemy::~Enemy()
{
	delete m_vc;
	this->Release(*RipExtern::g_world);
	delete m_boundingFrustum;

	for (auto drawable : m_pathNodes)
	{
		delete drawable;
	}
	m_pathNodes.clear();

}

void Enemy::setDir(const float & x, const float & y, const float & z)
{
	p_camera->setDirection(x, y, z);
}

Camera * Enemy::getCamera()
{
	return p_camera;
}

const int * Enemy::getPlayerVisibility() const
{
	if (m_allowVisability)
	{
		return m_vc->getVisibilityForPlayers();
	}
	else
	{
		int temp[2] = { 0,0 };

		return temp;
	}
}

void Enemy::CullingForVisability(const Transform& player)
{
	if (!m_disabled && getAIState() != AIState::Possessed)
	{
		DirectX::XMVECTOR enemyToPlayer = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&player.getPosition()), DirectX::XMLoadFloat4A(&getPosition()));

		float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(DirectX::XMLoadFloat4A(&p_camera->getDirection())), DirectX::XMVector3Normalize(enemyToPlayer)));
		//float d2 = DirectX::XMVectorGetY(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(DirectX::XMLoadFloat4A(&p_camera->getDirection())), DirectX::XMVector3Normalize(enemyToPlayer)));
		float lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(enemyToPlayer));

		if (d > p_camera->getFOV() / 3.14f && lenght <= (p_camera->getFarPlane() / d) + 3)
		{
			m_allowVisability = true;
			/*enemyX = d;
			enemyY = d2;*/
			
		}
		else
		{
			m_allowVisability = false;
			enemyX = 0;
			enemyY = 0;
		}
	}
	else
	{
		m_allowVisability = false;
		enemyX = 0;
		enemyY = 0;
	}
		
}

void Enemy::setPosition(const float & x, const float & y, const float & z, const float & w)
{
	Transform::setPosition(DirectX::XMFLOAT4A(x, y, z, w));
	PhysicsComponent::p_setPosition(x, y, z);
}

void Enemy::BeginPlay()
{
	
}

void Enemy::Update(double deltaTime)
{
	using namespace DirectX;
	AIState state = getAIState();

	if (!m_lockedByClient)
	{
		handleStates(deltaTime);
		_cameraPlacement(deltaTime);
		auto deltaX = getLiniearVelocity().x * deltaTime;
		auto deltaZ = getLiniearVelocity().z * deltaTime;
		m_currentMoveSpeed = XMVectorGetX(XMVector2Length(XMVectorSet(deltaX, deltaZ, 0.0, 0.0))) / deltaTime;

		m_currentMoveSpeed = (float)((int)(m_currentMoveSpeed * 2.0f + 0.5f)) * 0.5f;
	}

	if (getAnimationPlayer())
		getAnimationPlayer()->Update(deltaTime);
	
	m_sinWaver += deltaTime;

	if (m_PlayerPtr->GetMapPicked())
	{
		m_nodeFootPrintsEnabled = true;
	}

	if (!m_IsPossessedByTeammate)
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

		}
	}

}

void Enemy::ClientUpdate(double deltaTime)
{
	using namespace Network;

	if (!m_IsPossessedByTeammate)
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

		}
	}

	AIState state = getAIState();

	_cameraPlacement(deltaTime);
	//Visibility update
	if (state != AIState::Possessed && state != AIState::Disabled && !m_lockedByClient)
	{

		float visPercLocal = (float)m_vc->getVisibilityForPlayers()[0] / (float)Player::g_fullVisability;
		float lengthToTarget = GetLenghtToPlayer(m_PlayerPtr->getPosition());

		if (lengthToTarget < m_lengthToPlayerSpan)
		{
			visPercLocal *= 1.5;
		}

		if (visPercLocal > 0)
		{
			m_visCounter += visPercLocal * deltaTime;
			if (m_visabilityTimer <= m_visCounter)
			{
				m_found = true;
			}
		}
		else
		{

			if (m_visCounter - deltaTime > 0)
			{
				m_visCounter -= deltaTime;
			}
			else
			{
				m_visCounter = 0;
			}
		}
		setLiniearVelocity(0, 0, 0);
	}

	handleStatesClient(deltaTime);

	if (getAnimationPlayer())
		getAnimationPlayer()->Update(deltaTime);

	

	if (state == AIState::Possessed)
	{
		auto deltaX = getLiniearVelocity().x * deltaTime;
		auto deltaZ = getLiniearVelocity().z * deltaTime;

		m_currentMoveSpeed = XMVectorGetX(XMVector2Length(XMVectorSet(deltaX, deltaZ, 0.0, 0.0))) / deltaTime;
		m_currentMoveSpeed = (float)((int)(m_currentMoveSpeed * 2.0f + 0.5f)) * 0.5f;

		this->sendNetworkUpdate();
	}
	
}

void Enemy::PhysicsUpdate(double deltaTime)
{
	p_updatePhysics(this);
}

void Enemy::Draw()
{
	Drawable::Draw();
}

void Enemy::QueueForVisibility()
{
	AIState state = getAIState();
	bool canDoVis = (state != AIState::Disabled && state != AIState::Possessed);

	if(!canDoVis)
	{
		m_vc->Reset();
	}

	if (m_allowVisability && canDoVis && !ClientLocked())
	{
		m_vc->QueueVisibility();
	}
	
}

void Enemy::LockEnemyInput()
{
	m_inputLocked = true;
}

void Enemy::UnlockEnemyInput()
{
	m_inputLocked = false;
}

void Enemy::DisableEnemy()
{
	m_disabled = true;
	m_vc->Reset();
}

void Enemy::EnableEnemy()
{
	m_disabled = false;
}

bool Enemy::GetDisabledState()
{
	return m_disabled;
}

void Enemy::onNetworkUpdate(Network::ENEMYUPDATEPACKET * packet)
{
	this->m_currentMoveSpeed = packet->moveSpeed;
	this->m_currentDirection = packet->direction;
	this->setPosition(packet->pos.x, packet->pos.y, packet->pos.z, 0.0f);
	p_setRotation(0.0f, packet->rot.y, 0.0f);
	p_camera->setDirection(packet->camDir);
	//m_vc->getCamera()->setDirection(packet->camDir);

}

void Enemy::onNetworkPossessed(Network::ENTITYSTATEPACKET * packet)
{
	m_lockedByClient = packet->condition;
	if (!packet->condition)
	{
		setTransitionState(AITransitionState::ExitingPossess);
		m_IsPossessedByTeammate = false;
	}
	else
	{
		m_vc->Reset();
		m_IsPossessedByTeammate = true;
	}
}

void Enemy::onNetworkDisabled(Network::ENTITYSTATEPACKET * packet)
{
	if (getAIState() != AIState::Possessed || m_lockedByClient)
	{
		if (packet->condition)
		{
			m_vc->Reset();
			this->setTransitionState(AITransitionState::BeingDisabled);
			//CreateEmitter (maybe?)
		}
	}
}

void Enemy::sendNetworkUpdate()
{
	Network::ENEMYUPDATEPACKET packet;
	packet.uniqueID = uniqueID;
	packet.pos = getPosition();
	packet.rot = p_camera->getYRotationEuler();
	packet.camDir = p_camera->getDirection();
	packet.moveSpeed = m_currentMoveSpeed;
	packet.direction = m_currentDirection;

	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), PacketPriority::LOW_PRIORITY);
}

float Enemy::getTotalVisibility()
{
	return m_visCounter;
}

float Enemy::getMaxVisibility()
{
	return m_visabilityTimer;
}

int Enemy::GetGuardUniqueIndex()
{
	return m_guardUniqueIndex;
}

void Enemy::SetGuardUniqueIndex(const int& index)
{
	m_guardUniqueIndex = index;
}

const int Enemy::getInteractRayId()
{
	return m_interactRayId;
}

void Enemy::_handleInput(double deltaTime)
{
	if (Input::MouseLock() && !m_kp.unlockMouse)
	{
		m_kp.unlockMouse = true;
		unlockMouse = !unlockMouse;
	}
	else if (!Input::MouseLock())
		m_kp.unlockMouse = false;

	_onSprint();
	_onCrouch();
	_scrollMovementMod();
	_onMovement(deltaTime);
	_onInteract();
	_onRotate(deltaTime);
	_onReleasePossessed(deltaTime);
	PhysicsComponent::p_setRotation(p_camera->getYRotationEuler().x, p_camera->getYRotationEuler().y, p_camera->getYRotationEuler().z);
}

void Enemy::_onMovement(double deltaTime)
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

	m_cameraSpeed = DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMVectorSet(x, z, 0, 0)));

	setLiniearVelocity(x, getLiniearVelocity().y, z);
}

void Enemy::_scrollMovementMod()
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

void Enemy::_onRotate(double deltaTime)
{
	if (!unlockMouse)
	{
		const float deltaY = Input::TurnUp();
		const float deltaX = Input::TurnRight();
		
		//m_peekRotate = 0;
		if (deltaX)
		{
			p_camera->Rotate(0.0f, deltaX * Input::GetPlayerMouseSensitivity().x * deltaTime * 0.02f, 0.0f);
			//m_vc->getCamera()->Rotate(0.0f, deltaX * Input::GetPlayerMouseSensitivity().x * deltaTime * 0.02f, 0.0f);
		}
		if (deltaY)
		{
			p_camera->Rotate(deltaY * Input::GetPlayerMouseSensitivity().y * deltaTime * 0.02f, 0.0f, 0.0f);
			//m_vc->getCamera()->Rotate(deltaY * Input::GetPlayerMouseSensitivity().y * deltaTime * 0.02f, 0.0f, 0.0f);
			
		}
		/*if (deltaX && (m_peekRotate + deltaX * Input::GetPlayerMouseSensitivity().x * deltaTime) <= 0.5 && (m_peekRotate + deltaX * Input::GetPlayerMouseSensitivity().x * deltaTime) >= -0.5)
		{
			if (Input::PeekRight() > 0.1 || Input::PeekRight() < -0.1)
			{
				m_peekRotate += deltaX * Input::GetPlayerMouseSensitivity().x * deltaTime;
			}
		}
		if (deltaY)
		{
			if ((p_camera->getDirection().y - deltaY * Input::GetPlayerMouseSensitivity().y * deltaTime) < 0.90f)
			{
			}
			else if (p_camera->getDirection().y >= 0.90f)
			{
				p_camera->setDirection(p_camera->getDirection().x, 0.89f, p_camera->getDirection().z);
			}
			if ((p_camera->getDirection().y - deltaY * Input::GetPlayerMouseSensitivity().y * deltaTime) > -0.90f)
			{
				p_camera->Rotate(deltaY * Input::GetPlayerMouseSensitivity().y * deltaTime, 0.0f, 0.0f);
			}
			else if (p_camera->getDirection().y <= -0.90f)
			{
				p_camera->setDirection(p_camera->getDirection().x, -0.89f, p_camera->getDirection().z);
			}
		}*/
	}
}

void Enemy::_IsInSight()
{
	float temp = (float)m_vc->getVisibilityForPlayers()[0] / (float)Player::g_fullVisability;
	temp *= 100;

	//std::cout << m_vc.getVisibilityForPlayers()[0] << std::endl;

	//int ran = rand() % 100 + 1;
#if _DEBUG
	/*ImGui::Begin("Sight");
	ImGui::Text("vis: %f", temp);
	ImGui::Text("FullVis: %f", Player::g_fullVisability);
	ImGui::End();*/
#endif
	//if (ran < temp) 
	//{
	//	std::cout << "Saw you" << std::endl;
	//}
}

Enemy* Enemy::validate()
{
	return this;
}

void Enemy::setPossessor(Player* possessor, float maxDuration, float delay)
{
	m_possessor = possessor;
	m_possessReturnDelay = 1;
}

void Enemy::removePossessor()
{
	if (m_possessor != nullptr)
	{
		m_possessor->UnlockPlayerInput();
		this->getBody()->SetType(e_dynamicBody);
		//this->getBody()->SetAwake(false);
		m_possessor = nullptr;
		m_possessReturnDelay = 0;
		m_released = true; 
		m_disabled = true; 
	}
}

void Enemy::setKnockOutType(KnockOutType knockOutType)
{
	m_knockOutType = knockOutType;
}

void Enemy::setSoundLocation(const SoundLocation & sl)
{
	m_sl = sl;
}

const Enemy::SoundLocation & Enemy::getSoundLocation() const
{
	return m_sl;
}

const Enemy::SoundLocation & Enemy::getLoudestSoundLocation() const
{
	return m_loudestSoundLocation;
}

void Enemy::setLoudestSoundLocation(const SoundLocation & sl)
{
	m_loudestSoundLocation = sl;
}

void Enemy::setCalculatedVisibilityFor(int playerIndex, int value)
{
	m_vc->SetCalculatedVisibilityFor(playerIndex, value);
}

const DirectX::XMFLOAT4A & Enemy::getClearestPlayerLocation() const
{
	return m_clearestPlayerPos;
}

void Enemy::setClearestPlayerLocation(const DirectX::XMFLOAT4A & cpl)
{
	m_clearestPlayerPos = cpl;
}

const float & Enemy::getBiggestVisCounter() const
{
	return m_biggestVisCounter;
}

void Enemy::setBiggestVisCounter(float bvc)
{
	m_biggestVisCounter = bvc;
}

void Enemy::setReleased(bool released)
{
	m_released = released; 
}

bool Enemy::getIfLost()
{
	return m_found;
}

const Enemy::KnockOutType Enemy::getKnockOutType() const
{
	return m_knockOutType; 
}

void Enemy::addTeleportAbility(const TeleportAbility & teleportAbility)
{
	m_teleportBoundingSphere.push_back(teleportAbility.GetBoundingSphere());
}

void Enemy::DrawGuardPath()
{
	if (m_nodeFootPrintsEnabled)
	{
		int counter = 0;
		int node = GetCurrentPathNodeGridID();
		if (node != 0)
		{
			m_pathNodes.at(node - 1)->Draw();
		}

		for (unsigned int i = node; i < m_pathNodes.size(); ++i)
		{
			m_pathNodes.at(i)->Draw();
			counter++;
			if (counter >= m_maxDrawOutNode)
			{
				break;
			}
		}
	}
}

void Enemy::EnableGuardPathPrint()
{
	m_nodeFootPrintsEnabled = true;
}

float Enemy::GetLenghtToPlayer(const DirectX::XMFLOAT4A& playerPos)
{
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&getPosition()), DirectX::XMLoadFloat4A(&playerPos));
	vec = DirectX::XMVector3LengthEst(vec);
	//m_lenghtToPlayer = DirectX::XMVectorGetX(vec);
	return DirectX::XMVectorGetX(vec);
}

void Enemy::SetPlayerPointer(Player* player)
{
	m_PlayerPtr = player;
}

void Enemy::_onReleasePossessed(double deltaTime)
{
	if (m_possessor != nullptr)
	{
		if (m_possessReturnDelay <= 0)
		{
			if (Input::OnCancelAbilityPressed())
			{
				m_possessor->UnlockPlayerInput();
				m_possessor = nullptr;
				//this->CreateBox(1,1,1);
				this->getBody()->SetType(e_dynamicBody);
				//this->getBody()->SetAwake(false);
			}
		}
		else
			m_possessReturnDelay -= deltaTime;
	}
}

void Enemy::_onJump()
{
	if (Input::Jump())
	{
		if (m_kp.jump == false)
		{
			addForceToCenter(0, JUMP_POWER, 0);
			m_kp.jump = true;
		}
	}
	else
	{
		m_kp.jump = false;
	}
}

void Enemy::_onCrouch()
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
	else if (crouchInputType == Input::InputType::Keyboard)
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

void Enemy::_onSprint()
{
	if (p_moveState != Crouching)
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

void Enemy::_onInteract()
{
	if (RipExtern::g_rayListener->hasRayHit(m_interactRayId))
	{
		RayCastListener::Ray& ray = RipExtern::g_rayListener->GetProcessedRay(m_interactRayId);
		RayCastListener::RayContact& con = ray.GetRayContact(0);
		for (int i = 0; i < ray.getNrOfContacts(); i++)
		{
			con = ray.GetRayContact(i);
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
					//Snuff out torches (example)
				}
				else if (con.contactShape->GetBody()->GetObjectTag() == "ENEMY")
				{

					//std::cout << "Enemy Found!" << std::endl;
					//Snuff out torches (example)
				}
			}
		}
	}

	if (Input::Interact())
	{
		if (m_kp.interact == false)
		{
			if (m_interactRayId == -100)
				m_interactRayId = RipExtern::g_rayListener->PrepareRay(this->getBody(), this->getCamera()->getPosition(), this->getCamera()->getDirection(), Enemy::INTERACT_RANGE);

			m_kp.interact = true;
		}

	}
	else
	{
		m_kp.interact = false;
	}
}

void Enemy::_cameraPlacement(double deltaTime)
{
	if (!m_inputLocked)
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

		peekOffsetLeft.x = (upperBodyLocal.x - 1) * p_camera->getDirection().z;
		peekOffsetLeft.y = upperBodyLocal.y;
		peekOffsetLeft.z = (upperBodyLocal.z + 1)* p_camera->getDirection().x;

		peekOffsetRight.x = (upperBodyLocal.x + 1) * p_camera->getDirection().z;
		peekOffsetRight.y = upperBodyLocal.y;
		peekOffsetRight.z = (upperBodyLocal.z - 1) * p_camera->getDirection().x;

		headPosLocal += _slerp(peekOffsetRight, peekOffsetLeft, (m_peektimer + 1)*0.5) - headPosLocal;

		//-------------------------------------------Crouch-------------------------------------------// 

		m_crouchAnimSteps += crouchDir * (float)deltaTime*m_crouchSpeed;
		m_crouchAnimSteps = std::clamp(m_crouchAnimSteps, 0.0f, 1.0f);
		headPosLocal.y += lerp(m_standHeight, m_crouchHeight, m_crouchAnimSteps) - m_standHeight;

		if (m_crouchAnimSteps == 1 || m_crouchAnimSteps == 0) //Animation Finished
		{
			crouchDir = 0;
		}

		//--------------------------------------Camera movement---------------------------------------// 
		b3Vec3 headPosWorld = this->getBody()->GetTransform().translation + headPosLocal;
		DirectX::XMFLOAT4A pos = DirectX::XMFLOAT4A(headPosWorld.x, headPosWorld.y + .75, headPosWorld.z, 1.0f);
		p_camera->setPosition(pos);
		//Camera Tilt
		p_CameraTilting(deltaTime, m_peektimer);

		static float lastOffset = 0.0f;
		static bool hasPlayed = true;
		static int last = 0;

		//Head Bobbing
		float offsetY = 0;//p_viewBobbing(deltaTime, m_cameraSpeed, this->getBody());

		pos.y += offsetY;

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
					int index = -1;
					while (index == -1 || index == last)
					{
						index = rand() % (int)RipSounds::g_armorStepsStone.size();
					}
					FMOD::Channel * c = nullptr;
					c = AudioEngine::PlaySoundEffect(RipSounds::g_armorStepsStone[index], &at, &m_soundFootstep);
					b3Vec3 vel = getLiniearVelocity();
					DirectX::XMVECTOR vVel = DirectX::XMVectorSet(vel.x, vel.y, vel.z, 0.0f);
					float speed = DirectX::XMVectorGetX(DirectX::XMVector3Length(vVel));

					speed *= 0.1;
					speed -= 0.2f;
					c->setVolume(speed);
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
	else
	{
		DirectX::XMFLOAT4A pos = getPosition();
		pos.y += m_standHeight;
		p_camera->setPosition(pos);
		//pos = p_CameraTilting(deltaTime, Input::PeekRight());
		float offsetY = p_viewBobbing(deltaTime, m_moveSpeed, this->getBody());

		pos.y += offsetY;

		//pos.y += p_Crouching(deltaTime, m_crouchAnimStartPos, p_camera->getPosition());
		//p_camera->setPosition(pos);
	}
}

void Enemy::_RotateGuard(float x, float y, float angle, float deltaTime)
{
	DirectX::XMVECTOR newDir = DirectX::XMVector3Normalize(DirectX::XMVectorSet(x, 0, y, 0));
	DirectX::XMFLOAT4A xmDir;
	DirectX::XMStoreFloat4A(&xmDir, newDir);
	p_camera->setDirection(xmDir);
	//m_vc->getCamera()->setDirection(xmDir);
	p_camera->Rotate(0, angle * deltaTime, 0);
	//m_vc->getCamera()->Rotate(0, angle * deltaTime, 0);
	DirectX::XMFLOAT4A cameraRotationY = p_camera->getYRotationEuler();
	p_setRotation(0, cameraRotationY.y, 0);
}

void Enemy::_CheckPlayer(double deltaTime)
{
	if (m_allowVisability)
	{
		float visPercLocal = (float)m_vc->getVisibilityForPlayers()[0] / (float)Player::g_fullVisability;
		float visPercRemote = -1;//(float)m_vc->getVisibilityForPlayers()[1] / (float)Player::g_fullVisability;

		float visPerc;
		float lengthToTarget;

		if (visPercLocal >= visPercRemote)
		{
			lengthToTarget = GetLenghtToPlayer(m_PlayerPtr->getPosition());
			visPerc = visPercLocal;
		}
		else
		{
			visPerc = visPercRemote;
			lengthToTarget = GetLenghtToPlayer(m_RemotePtr->getPosition());
		}


		if (lengthToTarget < m_lengthToPlayerSpan)
		{
			visPerc *= 1.5;
		}
		if (lengthToTarget < m_playerTooCloseInstaLose)
			visPerc *= 5;

		if (visPerc > 0)
		{
			m_visCounter += visPerc * deltaTime;
			
			if (m_visabilityTimer <= m_visCounter)
			{	
				m_visCounter = m_visabilityTimer;
				m_found = true;
			}
		}
		else
		{
			
			if (m_visCounter - deltaTime > 0)
			{
				m_visCounter -= deltaTime;
			}
			else
			{
				m_visCounter = 0;
			}
		}

	}
	else
	{
		if (m_visCounter - deltaTime > 0)
		{
			m_visCounter -= deltaTime;
		}
		else
		{
			m_visCounter = 0;
		}
	}

	//m_vc->Reset();

}

void Enemy::_activateCrouch(const unsigned int inputType)
{
	this->getBody()->GetShapeList()->GetNext()->SetSensor(true);
	crouchDir = 1;
	m_moveSpeed = MOVE_SPEED * 0.5f;
	p_moveState = Crouching;
	m_prevClickCrouch = inputType;
}

void Enemy::_deActivateCrouch()
{
	this->getBody()->GetShapeList()->GetNext()->SetSensor(false);
	crouchDir = -1;
	m_moveSpeed = MOVE_SPEED;
	p_moveState = Idle;
	m_prevClickCrouch = Input::InputType::None;
}

void Enemy::_startSprint(const unsigned int inputType)
{
	m_moveSpeed = MOVE_SPEED * SPRINT_MULT;
	p_moveState = Sprinting;
	m_scrollMoveModifier = 0.9f;
	m_prevSprintInputType = inputType;
}

void Enemy::_startWalk()
{
	m_moveSpeed = MOVE_SPEED;
	p_moveState = Walking;
	m_prevSprintInputType = 0;
}

void Enemy::_playFootsteps(double deltaTime)
{
	m_av.timer += deltaTime * m_currentMoveSpeed * 2; // This should be deltaTime * movementspeed

	if (m_av.timer > DirectX::XM_PI)
		m_av.timer = 0.0f;

	float curve = sin(m_av.timer);

	if ((!m_av.hasPlayed && curve > m_av.lastCurve) || (m_av.hasPlayed && curve < m_av.lastCurve))
	{
		int index = -1;
		while (index == -1 || index == m_av.lastIndex)
		{
			index = rand() % (int)RipSounds::g_armorStepsStone.size();
		}
		FMOD_VECTOR at = { getPosition().x, getPosition().y ,getPosition().z };

		float vol = std::clamp(m_currentMoveSpeed, 0.0f, 1.0f);
		auto channel = AudioEngine::PlaySoundEffect(RipSounds::g_armorStepsStone[index], &at, &m_soundFootstep);

		m_av.lastIndex = index;
		m_av.hasPlayed = !m_av.hasPlayed;
	}

	m_av.lastCurve = curve;
}

b3Vec3 Enemy::_slerp(b3Vec3 start, b3Vec3 end, float percent)
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

void Enemy::_detectTeleportSphere()
{
	//auto dir = p_camera->getDirection();
		//DirectX::XMFLOAT4A forward, right, up;
		//forward = { dir.x, dir.y, dir.z, 0.0};
		//up = { 0,1,0,0.0 };
		//DirectX::XMStoreFloat4A(&right, DirectX::XMVector3Cross(XMLoadFloat4A(&forward), XMLoadFloat4A(&up)));
		//DirectX::XMStoreFloat4A(&forward, DirectX::XMVector3Cross(XMLoadFloat4A(&right), XMLoadFloat4A(&up)));

		//DirectX::XMFLOAT4X4A rotMatrix = 
		//{
		//	right.x,	right.y,	right.z,	0,
		//	0,			1,			0,			0,
		//	forward.x,	forward.y,	forward.z,	0,
		//	0,			0,			0,			1
		//};
		//auto cameraWorld = p_camera->ForceRotation(rotMatrix);
		//this->ForceWorld(cameraWorld);
	DirectX::XMMATRIX viewInv, proj;
	proj = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&p_camera->getProjection()));
	viewInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&p_camera->getView())));

	DirectX::BoundingFrustum::CreateFromMatrix(*m_boundingFrustum, proj);
	m_boundingFrustum->Transform(*m_boundingFrustum, viewInv);

	for (size_t i = 0; i < m_teleportBoundingSphere.size(); i++)
	{

		if (m_boundingFrustum->Intersects(*m_teleportBoundingSphere[i]))
		{
			DirectX::XMFLOAT4 direction = getDirection(getPosition(), DirectX::XMFLOAT4A(m_teleportBoundingSphere[i]->Center.x,
				m_teleportBoundingSphere[i]->Center.y,
				m_teleportBoundingSphere[i]->Center.z,
				0.0f));
		}

	}
}