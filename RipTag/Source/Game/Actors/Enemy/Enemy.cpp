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

	{
		auto& layerMachine = this->getAnimationPlayer()->InitLayerMachine(Manager::g_animationManager.getSkeleton("GUARD").get());
		layerMachine->AddBasicLayer
			("test_layer", Manager::g_animationManager.getAnimation("GUARD", "HEADTURN_ANIMATION").get(), 0.2f, 1.0f);
		
	}
	{
		auto idleAnim = Manager::g_animationManager.getAnimation("GUARD", "IDLE_ANIMATION").get();
		auto walkAnim = Manager::g_animationManager.getAnimation("GUARD", "WALK_ANIMATION").get();
		auto& machine = getAnimationPlayer()->InitStateMachine(1);
		auto state = machine->AddBlendSpace1DState("walk_state", &m_currentMoveSpeed, 0.0, 1.5f);
		state->AddBlendNodes({ {idleAnim, 0.0}, {walkAnim, 1.5f} });
		machine->SetState("walk_state");
		this->getAnimationPlayer()->Play();

	}
	b3Vec3 pos(1, 0.9, 1);
	PhysicsComponent::Init(*world, e_dynamicBody, pos.x, pos.y, pos.z, false, 0); //0.5f, 0.9f, 0.5f //1,0.9,1

	this->getBody()->SetUserData(Enemy::validate());
	this->getBody()->SetObjectTag("ENEMY");
	CreateShape(0, pos.y, 0, pos.x, pos.y, pos.z, "UPPERBODY");
	CreateShape(0, (pos.y*1.5) + 0.25, 0, 0.5, 0.5, 0.1, "HEAD");
	m_standHeight = (pos.y*1.5) + 0.25;
	m_crouchHeight = pos.y * 1.1;
	setUserDataBody(this);

	this->setEntityType(EntityType::GuarddType);
	this->setPosition(startPosX, startPosY, startPosZ);
	//setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	this->setModelTransform(DirectX::XMMatrixTranslation(0.0, -0.9, 0.0));
	setScale(.5, .5, .5);
	setTexture(Manager::g_textureManager.getTexture("GUARD"));
	//setTextureTileMult(1, 2);
	m_boundingFrustum = new DirectX::BoundingFrustum(DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&p_camera->getProjection())));
	
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
	if (!m_disabled)
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
	static double accumulatedTime = 0.0;
	static const double SEND_AI_PACKET_FREQUENCY = 1.0 / 15.0; 


	using namespace DirectX;

	handleStates(deltaTime);

	auto deltaX = getLiniearVelocity().x * deltaTime;
	auto deltaZ = getLiniearVelocity().z * deltaTime;
	m_currentMoveSpeed = XMVectorGetX(XMVector2Length(XMVectorSet(deltaX, deltaZ, 0.0, 0.0))) / deltaTime;

	m_currentMoveSpeed = (float)((int)(m_currentMoveSpeed * 2.0f + 0.5f)) * 0.5f;

	if (getAnimationPlayer())
		getAnimationPlayer()->Update(deltaTime);
	
	m_sinWaver += deltaTime;

	if (m_PlayerPtr->GetMapPicked())
	{
		m_nodeFootPrintsEnabled = true;
	}
}

void Enemy::ClientUpdate(double deltaTime)
{
	_cameraPlacement(deltaTime);
	if (getAnimationPlayer())
		getAnimationPlayer()->Update(deltaTime);
	setLiniearVelocity(0, 0, 0);

	//Visibility update
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
	}
}

void Enemy::PhysicsUpdate(double deltaTime)
{
	p_updatePhysics(this);
}

void Enemy::QueueForVisibility()
{
	if (true == m_allowVisability)
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
	this->setPosition(packet->pos.x, packet->pos.y, packet->pos.z, 0.0f);
	p_setRotation(0.0f, packet->rot.y, 0.0f);
	p_camera->setDirection(packet->camDir);
}

void Enemy::sendNetworkUpdate()
{
	Network::ENEMYUPDATEPACKET packet;
	packet.uniqueID = uniqueID;
	packet.pos = getPosition();
	packet.rot = p_camera->getYRotationEuler();
	packet.camDir = p_camera->getDirection();
	packet.moveSpeed = m_currentMoveSpeed;

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
		float deltaY = Input::TurnUp();
		float deltaX = Input::TurnRight();
		//if (Input::PeekRight() > 0.1 || Input::PeekRight() < -0.1)
		//{

		//}
		//else
		//{
		//	if (m_peekRotate > 0.05f || m_peekRotate < -0.05f)
		//	{
		//		if (m_peekRotate > 0)
		//		{
		//			p_camera->Rotate(0.0f, -0.05f, 0.0f);
		//			m_peekRotate -= 0.05;
		//		}
		//		else
		//		{
		//			p_camera->Rotate(0.0f, +0.05f, 0.0f);
		//			m_peekRotate += 0.05;
		//		}

		//	}
		//	else
		//	{
		//		m_peekRotate = 0;
		//	}
		//	//
		//}
		m_peekRotate = 0;
		if (deltaX && (m_peekRotate + deltaX * Input::GetPlayerMouseSensitivity().x * deltaTime) <= 0.5 && (m_peekRotate + deltaX * Input::GetPlayerMouseSensitivity().x * deltaTime) >= -0.5)
		{
			p_camera->Rotate(0.0f, deltaX * Input::GetPlayerMouseSensitivity().x * deltaTime, 0.0f);
			if (Input::PeekRight() > 0.1 || Input::PeekRight() < -0.1)
			{
				m_peekRotate += deltaX * Input::GetPlayerMouseSensitivity().x * deltaTime;
			}
		}
		if (deltaY)
		{
			if ((p_camera->getDirection().y - deltaY * Input::GetPlayerMouseSensitivity().y * deltaTime) < 0.90f)
			{
				p_camera->Rotate(deltaY * Input::GetPlayerMouseSensitivity().y * deltaTime, 0.0f, 0.0f);
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


		}

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
		this->getBody()->SetType(e_staticBody);
		this->getBody()->SetAwake(false);
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
				this->getBody()->SetType(e_staticBody);
				this->getBody()->SetAwake(false);
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
	if (Input::isUsingGamepad())
	{
		m_currClickCrouch = Input::Crouch();
		if (m_currClickCrouch && !m_prevClickCrouch && m_toggleCrouch == 0)
		{
			_activateCrouch();
			m_toggleCrouch = 1;
		}
		else if (m_currClickCrouch && !m_prevClickCrouch && m_toggleCrouch == 1)
		{
			_deActivateCrouch();
			m_toggleCrouch = 0;

			//Just so we don't end up in an old sprint-mode when deactivating crouch.
			m_toggleSprint = 0;
		}
		m_prevClickCrouch = m_currClickCrouch;
	}
	else
	{
		if (Input::Crouch())
		{
			if (m_kp.crouching == false)
			{
				this->getBody()->GetShapeList()->GetNext()->SetSensor(true);
				crouchDir = 1;

				m_kp.crouching = true;
			}
		}
		else
		{
			if (m_kp.crouching)
			{
				crouchDir = -1;
				this->getBody()->GetShapeList()->GetNext()->SetSensor(false);

				m_kp.crouching = false;
			}
		}
	}

	if (m_kp.crouching)
	{
		m_moveSpeed = MOVE_SPEED * 0.5f;
	}
}

void Enemy::_onSprint()
{
	if (Input::MoveForward() != 0 || Input::MoveRight() != 0)
	{
		if (Input::isUsingGamepad())
		{
			m_currClickSprint = Input::Sprinting();
			if (m_currClickSprint && !m_prevClickSprint && m_toggleSprint == 0 && Input::MoveForward() > 0.9)
			{
				m_toggleSprint = 1;
			}

			if (m_toggleSprint == 1 && Input::MoveForward() > 0.9)
			{
				m_moveSpeed = MOVE_SPEED * SPRINT_MULT;
				p_moveState = Sprinting;
				m_scrollMoveModifier = 0.9f;
			}
			else
			{
				m_toggleSprint = 0;
			}

			if (m_toggleSprint == 0)
			{
				m_moveSpeed = MOVE_SPEED;
				p_moveState = Walking;
			}

			if (Input::MoveForward() == 0)
			{
				p_moveState = Idle;
				m_toggleSprint = 0;
			}

			m_prevClickSprint = m_currClickSprint;
		}
		else
		{
			if (Input::Sprinting())
			{
				m_moveSpeed = MOVE_SPEED * SPRINT_MULT;
				p_moveState = Sprinting;
				m_scrollMoveModifier = 0.9f;

			}
			else
			{
				m_moveSpeed = MOVE_SPEED;
				p_moveState = Walking;
			}
		}
	}
	else
	{
		m_moveSpeed = 0;
		p_moveState = Idle;
	}
}

void Enemy::_onInteract()
{
	if (Input::Interact())
	{
		if (m_kp.interact == false)
		{
			if(m_rayId == -100)
				m_rayId = RipExtern::g_rayListener->PrepareRay(this->getBody(), this->getCamera()->getPosition(), this->getCamera()->getDirection(), Enemy::INTERACT_RANGE);

			m_kp.interact = true;
		}
		else if(m_rayId != -100)
		{
			if (RipExtern::g_rayListener->hasRayHit(m_rayId))
			{
				RayCastListener::Ray* ray = RipExtern::g_rayListener->GetProcessedRay(m_rayId);
				for (int i = 0; i < ray->getNrOfContacts(); i++)
				{
					RayCastListener::RayContact* con = ray->GetRayContact(m_rayId);
					if (ray->getOriginBody()->GetObjectTag() == getBody()->GetObjectTag())
					{
						if (con->contactShape->GetBody()->GetObjectTag() == "LEVER")
						{
							
						}
						else if (con->contactShape->GetBody()->GetObjectTag() == "TORCH")
						{
							//Snuff out torches (example)
						}
						else if (con->contactShape->GetBody()->GetObjectTag() == "ENEMY")
						{

							//std::cout << "Enemy Found!" << std::endl;
							//Snuff out torches (example)
						}
						else if (con->contactShape->GetBody()->GetObjectTag() == "PLAYER")
						{

							//std::cout << "Player Found!" << std::endl;
							//Snuff out torches (example)
						}
					}
				}
			}
			m_rayId = -100;
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
		DirectX::XMFLOAT4A pos = DirectX::XMFLOAT4A(headPosWorld.x, headPosWorld.y, headPosWorld.z, 1.0f);
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
						index = rand() % (int)RipSounds::g_stepsStone.size();
					}
					FMOD::Channel * c = nullptr;
					c = AudioEngine::PlaySoundEffect(RipSounds::g_stepsStone[index], &at, AudioEngine::Player);
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
	p_camera->Rotate(0, angle * deltaTime, 0);
	DirectX::XMFLOAT4A cameraRotationY = p_camera->getYRotationEuler();
	p_setRotation(0, cameraRotationY.y, 0);
}

void Enemy::_CheckPlayer(double deltaTime)
{
	if (m_allowVisability)
	{
		float visPercLocal = (float)m_vc->getVisibilityForPlayers()[0] / (float)Player::g_fullVisability;
		float visPercRemote = (float)m_vc->getVisibilityForPlayers()[1] / (float)Player::g_fullVisability;

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
		if (getAIState() == High_Alert)
		{
			visPerc *= 1.2;
		}

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

}

void Enemy::_activateCrouch()
{
	this->getBody()->GetShapeList()->GetNext()->SetSensor(true);
	crouchDir = 1;
m_kp.crouching = true;
}

void Enemy::_deActivateCrouch()
{
	this->getBody()->GetShapeList()->GetNext()->SetSensor(false);
	crouchDir = -1;
	m_kp.crouching = false;
}

void Enemy::_playFootsteps(double deltaTime)
{
	m_av.timer += deltaTime * m_moveSpeed; // This should be deltaTime * movementspeed

	if (m_av.timer > DirectX::XM_PI)
		m_av.timer = 0.0f;

	float curve = sin(m_av.timer);

	if ((!m_av.hasPlayed && curve > m_av.lastCurve) || (m_av.hasPlayed && curve < m_av.lastCurve))
	{
		int index = -1;
		while (index == -1 || index == m_av.lastIndex)
		{
			index = rand() % (int)RipSounds::g_stepsStone.size();
		}
		FMOD_VECTOR at = { getPosition().x, getPosition().y ,getPosition().z };

		AudioEngine::PlaySoundEffect(RipSounds::g_stepsStone[index], &at, AudioEngine::Enemy)->setVolume(m_moveSpeed * 0.3);
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

			//std::cout << "HOLD OP THE PARTNER" << std::endl;

			//TODO: Fix when ray is corrected

			/*RayCastListener::Ray * r = RipExtern::g_rayListener->ShotRay(PhysicsComponent::getBody(), getPosition(), DirectX::XMFLOAT4A(
				direction.x,
				direction.y,
				direction.z,
				direction.w),
				getLenght(getPosition(), DirectX::XMFLOAT4A(
					direction.x,
					direction.y,
					direction.z,
					direction.w)));*/
			//if (r)
			{
				/*std::cout << "-------------------------------------------------" << std::endl;
				for (int i = 0; i < r->getNrOfContacts(); i++)
				{
					std::cout << r->GetRayContacts()[i]->contactShape->GetBody()->GetObjectTag() << std::endl;
				}*/
			}
		}

	}
}


//void Enemy::_investigatingSight(const double deltaTime)
//{
//	getBody()->SetType(e_dynamicBody);
//	_cameraPlacement(deltaTime);
//	_CheckPlayer(deltaTime);
//
//	if (this->GetAlertPathSize() > 0)
//	{
//		if (this->m_visCounter > this->m_biggestVisCounter)
//		{
//			DirectX::XMFLOAT4A playerPos = m_PlayerPtr->getPosition();
//			
//			if (m_RemotePtr)
//			{
//				const int * vis = m_vc->getVisibilityForPlayers();
//				if (vis[1] > vis[0])
//					playerPos = m_RemotePtr->getPosition();
//			}
//
//
//			Node * pathDestination = this->GetAlertDestination();
//
//			if (abs(pathDestination->worldPos.x - playerPos.x) > 5.0f ||
//				abs(pathDestination->worldPos.y - playerPos.z) > 5.0f)
//			{
//				DirectX::XMFLOAT4A guardPos = this->getPosition();
//				Tile playerTile = m_grid->WorldPosToTile(playerPos.x, playerPos.z);
//				Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);
//
//				this->SetAlertVector(m_grid->FindPath(guardTile, playerTile));
//			}
//		}
//	}
//	else
//	{
//		this->m_transState = EnemyTransitionState::Observe;
//	}
//}
//void Enemy::_investigatingSound(const double deltaTime)
//{
//	getBody()->SetType(e_dynamicBody);
//	_cameraPlacement(deltaTime);
//	_CheckPlayer(deltaTime);
//
//	if (this->GetAlertPathSize() > 0)
//	{
//		SoundLocation tmp = m_sl;
//
//		if (Network::Multiplayer::GetInstance()->isServer())
//		{
//			if (tmp.percentage < m_slRemote.percentage)
//				tmp = m_slRemote;
//		}
//
//		
//
//		if (tmp.percentage > this->m_loudestSoundLocation.percentage)
//		{
//			DirectX::XMFLOAT3 soundPos = tmp.soundPos;
//			Node * pathDestination = this->GetAlertDestination();
//
//			if (abs(pathDestination->worldPos.x - soundPos.x) > 2.0f ||
//				abs(pathDestination->worldPos.y - soundPos.z) > 2.0f)
//			{
//				DirectX::XMFLOAT4A guardPos = this->getPosition();
//				Tile playerTile = m_grid->WorldPosToTile(soundPos.x, soundPos.z);
//				Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);
//
//				this->SetAlertVector(m_grid->FindPath(guardTile, playerTile));
//			}
//		}
//	}
//	else
//	{
//		this->m_transState = EnemyTransitionState::Observe;
//	}
//}
//void Enemy::_investigatingRoom(const double deltaTime)
//{
//	getBody()->SetType(e_dynamicBody);
//	_cameraPlacement(deltaTime);
//	_CheckPlayer(deltaTime);
//
//	this->m_searchTimer += deltaTime;
//	
//	if (this->GetAlertPathSize() == 0)
//	{
//		this->setTransitionState(EnemyTransitionState::Observe);
//	}
//	SoundLocation target = m_sl;
//
//	if (target.percentage < m_slRemote.percentage)
//		target = m_slRemote;
//	if (this->getVisCounter() >= ALERT_TIME_LIMIT || target.percentage > SOUND_LEVEL)
//	{
//		//this->setTransitionState(EnemyTransitionState::Alerted);
//	}
//	else if (this->m_searchTimer > SEARCH_ROOM_TIME_LIMIT)
//	{
//		this->setTransitionState(EnemyTransitionState::ReturnToPatrol);
//	}
//}
//void Enemy::_highAlert(const double deltaTime)
//{
//	getBody()->SetType(e_dynamicBody);
//	_cameraPlacement(deltaTime);
//	_CheckPlayer(deltaTime);
//
//	this->m_HighAlertTime = deltaTime;
//	if (this->GetHighAlertTimer() >= HIGH_ALERT_LIMIT)
//	{
//		this->SetHightAlertTimer(0.f);
//		this->setClearestPlayerLocation(DirectX::XMFLOAT4A(0, 0, 0, 1));
//		this->setLoudestSoundLocation(Enemy::SoundLocation());
//		this->setBiggestVisCounter(0);
//		this->m_state = EnemyState::Suspicious;
//#ifdef _DEBUG
//
//		std::cout << green << "Enemy " << this->uniqueID << " Transition: High Alert -> Suspicious" << white << std::endl;
//#endif
//	}
//}
//void Enemy::_suspicious(const double deltaTime)
//{
//	getBody()->SetType(e_dynamicBody);
//	_cameraPlacement(deltaTime);
//	_CheckPlayer(deltaTime);
//
//	this->m_actTimer += deltaTime;
//	float attentionMultiplier = 1.0f; // TEMP will be moved to Enemy
//	if (this->m_actTimer > SUSPICIOUS_TIME_LIMIT / 3)
//	{
//		attentionMultiplier = 1.2f;
//	}
//	if (this->m_visCounter*attentionMultiplier >= this->m_biggestVisCounter)
//	{
//
//		DirectX::XMFLOAT4A playerPos = m_PlayerPtr->getPosition();
//
//		if (m_RemotePtr)
//		{
//			const int * vis = m_vc->getVisibilityForPlayers();
//			if (vis[1] > vis[0])
//				playerPos = m_RemotePtr->getPosition();
//		}
//
//		this->setClearestPlayerLocation(playerPos);
//		this->setBiggestVisCounter(this->getVisCounter()*attentionMultiplier);
//		/*b3Vec3 dir(guard->getPosition().x - m_player->getPosition().x, guard->getPosition().y - m_player->getPosition().y, guard->getPosition().z - m_player->getPosition().z);
//		b3Normalize(dir);
//		guard->setDir(dir.x, dir.y, dir.y);*/
//	}
//
//	SoundLocation target = m_sl;
//
//	if (target.percentage < m_slRemote.percentage)
//		target = m_slRemote;
//
//	if (target.percentage*attentionMultiplier >= this->m_loudestSoundLocation.percentage)
//	{
//		Enemy::SoundLocation temp = target;
//		temp.percentage *= attentionMultiplier;
//		this->m_loudestSoundLocation = temp;
//		/*b3Vec3 dir(guard->getPosition().x - guard->getLoudestSoundLocation().soundPos.x, guard->getPosition().y - guard->getLoudestSoundLocation().soundPos.y, guard->getPosition().z - guard->getLoudestSoundLocation().soundPos.z);
//		b3Normalize(dir);
//		guard->setDir(dir.x, dir.y, dir.y);*/
//	}
//	if (this->m_actTimer > SUSPICIOUS_TIME_LIMIT)
//	{
//		if (this->m_biggestVisCounter >= ALERT_TIME_LIMIT * 1.5)
//			this->m_transState = EnemyTransitionState::InvestigateSight; //what was that?
//		else if (this->m_loudestSoundLocation.percentage > SOUND_LEVEL*1.5)
//			this->m_transState = EnemyTransitionState::InvestigateSound; //what was that noise?
//		else
//		{
//			this->m_transState = EnemyTransitionState::ReturnToPatrol;
//			//Must have been nothing...
//		}
//	}
//}
//void Enemy::_scanningArea(const double deltaTime)
//{
//	getBody()->SetType(e_dynamicBody);
//	_cameraPlacement(deltaTime);
//	_CheckPlayer(deltaTime);
//
//	this->m_actTimer += deltaTime;
//	//Do animation
//	if (this->m_actTimer > SUSPICIOUS_TIME_LIMIT)
//	{
//		this->m_transState = EnemyTransitionState::SearchArea;
//	}
//}
//void Enemy::_patrolling(const double deltaTime)
//{
//	getBody()->SetType(e_dynamicBody);
//	_cameraPlacement(deltaTime);
//	_CheckPlayer(deltaTime);
//
//	if (m_path.size() > 0)
//	{
//		if (m_pathNodes.size() == 0)
//		{
//			Manager::g_textureManager.loadTextures("FOOT");
//			Manager::g_meshManager.loadStaticMesh("FOOT");
//			for (unsigned int i = 0; i < m_path.size(); ++i)
//			{
//
//				Drawable * temp = new Drawable();
//				temp->setModel(Manager::g_meshManager.getStaticMesh("FOOT"));
//				temp->setTexture(Manager::g_textureManager.getTexture("FOOT"));
//				temp->setScale({ 0.2f, 0.2f, 0.2f, 1.0f });
//				temp->setPosition(m_path.at(i)->worldPos.x, m_startYPos, m_path.at(i)->worldPos.y);
//
//				if (i + 1 < m_path.size())
//				{
//					temp->setRotation(0, _getPathNodeRotation({ m_path.at(i)->worldPos.x, m_path.at(i)->worldPos.y }, { m_path.at(i + 1)->worldPos.x, m_path.at(i + 1)->worldPos.y }), 0);
//				}
//
//
//				temp->SetTransparant(true);
//				m_pathNodes.push_back(temp);
//			}
//
//		}
//		/*float posY = 0.04 * sin(1 * DirectX::XM_PI*m_sinWaver*0.5f) + 4.4f;
//		for (auto path : m_pathNodes)
//		{
//			path->setPosition(path->getPosition().x, posY , path->getPosition().z);
//		}*/
//		if (m_nodeFootPrintsEnabled == true)
//		{
//			if (m_currentPathNode != 0)
//			{
//				Drawable * temp = m_pathNodes.at(m_currentPathNode - 1);
//				temp->setPosition(temp->getPosition().x, temp->getPosition().y - deltaTime * 2, temp->getPosition().z);
//			}
//		}
//		_MoveTo(m_path.at(m_currentPathNode), deltaTime);
//	}
//
//	SoundLocation target = m_sl;
//
//	if (target.percentage < m_slRemote.percentage)
//		target = m_slRemote;
//
//
//	if (this->getVisCounter() >= ALERT_TIME_LIMIT || target.percentage > SOUND_LEVEL) //"Huh?!" - Tim Allen
//	{
//		this->m_transState = EnemyTransitionState::Alerted;
//	}
//}
//void Enemy::_possessed(const double deltaTime)
//{
//	getBody()->SetType(e_dynamicBody);
//	_cameraPlacement(deltaTime);
//	_handleInput(deltaTime);
//}
//void Enemy::_disabled(const double deltaTime)
//{
//	getBody()->SetType(e_staticBody);
//	switch (m_knockOutType)
//	{
//
//	case Possessed:
//		if (m_released)
//		{
//			m_possesionRecoverTimer += deltaTime;
//			if (m_possesionRecoverTimer >= m_possessionRecoverMax)
//			{
//				m_transState = EnemyTransitionState::ExitingDisable;
//			}
//		}
//		break;
//	case Stoned:
//
//		m_knockOutTimer += deltaTime;
//		if (m_knockOutMaxTime <= m_knockOutTimer)
//		{
//			m_transState = EnemyTransitionState::ExitingDisable;
//		}
//		break;
//	}
//	PhysicsComponent::p_setRotation(p_camera->getYRotationEuler().x + DirectX::XMConvertToRadians(85), p_camera->getYRotationEuler().y, p_camera->getYRotationEuler().z);
//	m_visCounter = 0;
//}
//		