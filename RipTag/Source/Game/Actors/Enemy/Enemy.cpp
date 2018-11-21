#include "RipTagPCH.h"
#include "Enemy.h"

#include "EngineSource/3D Engine/Extern.h"
#include "EngineSource/3D Engine/RenderingManager.h"
#include "EngineSource/3D Engine/Components/Camera.h"
#include "EngineSource/3D Engine/Model/Managers/MeshManager.h"
#include "EngineSource/3D Engine/Model/Meshes/AnimationPlayer.h"
#include "EngineSource/3D Engine/Model/Managers/TextureManager.h"
#include "EngineSource/3D Engine/3DRendering/Rendering/VisabilityPass/Component/VisibilityComponent.h"
#include "2D Engine/Quad/Components/HUDComponent.h"

//#todoREMOVE
#include "../../../Engine/EngineSource/Helper/AnimationDebugHelper.h"

Enemy::Enemy(b3World* world, unsigned int id, float startPosX, float startPosY, float startPosZ) : Actor(), CameraHolder(), PhysicsComponent()
{
	this->uniqueID = id;
	this->p_initCamera(new Camera(DirectX::XMConvertToRadians(150.0f / 2.0f), 250.0f / 150.0f, 0.1f, 50.0f));
	m_vc = new VisibilityComponent();
	m_vc->Init(this->p_camera);
	this->setDir(1, 0, 0);
	this->getCamera()->setFarPlane(20);
	this->setModel(Manager::g_meshManager.getSkinnedMesh("GUARD"));
	this->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	this->getAnimationPlayer()->SetSkeleton(Manager::g_animationManager.getSkeleton("GUARD"));

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

DirectX::XMFLOAT2 Enemy::GetDirectionToPlayer(const DirectX::XMFLOAT4A& player, Camera& playerCma)
{
	using namespace DirectX;

	if (m_visCounter > 0)
	{
		XMMATRIX playerView = XMMatrixTranspose(XMLoadFloat4x4A(&playerCma.getView()));
		XMVECTOR enemyPos = XMLoadFloat4A(&getPosition());

		XMVECTOR vdir = XMVector4Transform(enemyPos, playerView);
		XMFLOAT2 dir = XMFLOAT2(DirectX::XMVectorGetX(vdir), DirectX::XMVectorGetZ(vdir));
		vdir = XMLoadFloat2(&dir);
		vdir = XMVector2Normalize(vdir);

		XMStoreFloat2(&dir, vdir);
		return dir;
	}
	return XMFLOAT2(0, 0);
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

	_handleStates(deltaTime);

	static float floor = 999.0f, roof = -1.0f;

	auto lol = getLiniearVelocity();
	float speed = DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMVectorSet(lol.x, lol.z, 0.0f, 0.0f)));

	if (speed < floor && speed > 0.01f)
		floor = speed;
	if (speed > roof)
		roof = speed;

#ifdef _DEBUG
	std::cout << std::endl << green <<"Speed: " << speed << "\nRoof: " << roof << "\nFloor: " << floor << "\nMaxDif: " << roof - floor << white << std::endl;
#endif // _DEBUG


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

	if (!m_disabled)
	{
		getBody()->SetType(e_dynamicBody);
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

				RayCastListener::Ray * r = RipExtern::g_rayListener->ShotRay(PhysicsComponent::getBody(), getPosition(), DirectX::XMFLOAT4A(
					direction.x,
					direction.y,
					direction.z,
					direction.w),
					getLenght(getPosition(), DirectX::XMFLOAT4A(
						direction.x,
						direction.y,
						direction.z,
						direction.w)));
				if (r)
				{
					/*std::cout << "-------------------------------------------------" << std::endl;
					for (int i = 0; i < r->getNrOfContacts(); i++)
					{						
						std::cout << r->GetRayContacts()[i]->contactShape->GetBody()->GetObjectTag() << std::endl;
					}*/
				}
			}
			
		}

		if (!m_inputLocked)
		{
			_handleInput(deltaTime);
		}
		else
		{
			if (m_state == Suspicious || m_state == Scanning_Area)
			{
				getBody()->SetAwake(false);
			}
			else if (m_alertPath.size() > 0 )
			{
				if (m_state != High_Alert)
				{
					_MoveToAlert(m_alertPath.at(0), deltaTime);
				}
			}
			else
			{
				if (m_path.size() > 0)
				{
					if (m_pathNodes.size() == 0)
					{
						Manager::g_textureManager.loadTextures("FOOT");
						Manager::g_meshManager.loadStaticMesh("FOOT");
						for (unsigned int i = 0; i < m_path.size(); ++i)
						{
							
							Drawable * temp = new Drawable();
							temp->setModel(Manager::g_meshManager.getStaticMesh("FOOT"));
							temp->setTexture(Manager::g_textureManager.getTexture("FOOT"));
							temp->setScale({ 0.2f, 0.2f, 0.2f, 1.0f });
							temp->setPosition(m_path.at(i)->worldPos.x, m_startYPos, m_path.at(i)->worldPos.y);

							if (i + 1 < m_path.size())
							{
								temp->setRotation(0, _getPathNodeRotation({ m_path.at(i)->worldPos.x, m_path.at(i)->worldPos.y }, { m_path.at(i+1)->worldPos.x, m_path.at(i+1)->worldPos.y }),0);
							}
							

							temp->SetTransparant(true);
							m_pathNodes.push_back(temp);
						}
						
					}
					/*float posY = 0.04 * sin(1 * DirectX::XM_PI*m_sinWaver*0.5f) + 4.4f;
					for (auto path : m_pathNodes)
					{
						path->setPosition(path->getPosition().x, posY , path->getPosition().z);
					}*/
					if (m_nodeFootPrintsEnabled == true)
					{
						if (m_currentPathNode != 0)
						{
							Drawable * temp = m_pathNodes.at(m_currentPathNode - 1);
							temp->setPosition(temp->getPosition().x, temp->getPosition().y - deltaTime * 2, temp->getPosition().z);
						}
					}
					_MoveTo(m_path.at(m_currentPathNode), deltaTime);
				}
			}
		}

		_cameraPlacement(deltaTime);
		
		if (m_inputLocked)
		{
			_CheckPlayer(deltaTime);
		}
	}
	else
	{
		getBody()->SetType(e_dynamicBody);
		switch (m_knockOutType)
		{

		case Possessed:
			if (m_released)
			{
				m_possesionRecoverTimer += deltaTime;
				if (m_possesionRecoverTimer >= m_possessionRecoverMax)
				{
					m_disabled = false;
					m_released = false; 
					m_possesionRecoverTimer = 0; 
				}
			}
			break; 
		case Stoned:
			
			m_knockOutTimer += deltaTime;
			if (m_knockOutMaxTime <= m_knockOutTimer)
			{
				m_disabled = false;
				m_knockOutTimer = 0;
			}
			break; 
		}
		//PhysicsComponent::p_setRotation(p_camera->getYRotationEuler().x + DirectX::XMConvertToRadians(85), p_camera->getYRotationEuler().y, p_camera->getYRotationEuler().z);
		m_visCounter = 0;
	}
	
}

void Enemy::ClientUpdate(double deltaTime)
{
	if (getAnimationPlayer())
		getAnimationPlayer()->Update(deltaTime);
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
	this->setPosition(packet->pos.x, packet->pos.y, packet->pos.z, packet->pos.y);
	this->setRotation(packet->rot);
}

void Enemy::sendNetworkUpdate()
{
	Network::ENEMYUPDATEPACKET packet;
	packet.uniqueID = uniqueID;
	packet.pos = getPosition();
	packet.rot = getEulerRotation();
	packet.moveSpeed = m_currentMoveSpeed;

	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), PacketPriority::LOW_PRIORITY);
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
	_possessed(deltaTime);
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

void Enemy::_TempGuardPath(bool x, double deltaTime)
{
	//p_camera->Rotate(0.0f, .5f * 5 * deltaTime, 0.0f);
	   
	//ImGui::Begin("be");
	//ImGui::Text("lel %f", p_camera->getYRotationEuler().y);
	//ImGui::End();


	//setRotation(p_camera->getYRotationEuler());
	//PhysicsComponent::p_setPositionRot(getPosition().x, getPosition().y, getPosition().z,p_camera->getYRotationEuler().x, p_camera->getYRotationEuler().y, p_camera->getYRotationEuler().z);
	//PhysicsComponent::p_setRotation(p_camera->getYRotationEuler().x, p_camera->getYRotationEuler().y, p_camera->getYRotationEuler().z);
	/* p_camera->getYRotationEuler();*/
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

void Enemy::SetPathVector(std::vector<Node*> path)
{
	m_path = path;
}

Node * Enemy::GetCurrentPathNode() const
{
	return m_path.at(m_currentPathNode);
}

void Enemy::SetAlertVector(std::vector<Node*> alertPath)
{
	if (m_alertPath.size() > 0)
	{
		for (int i = 0; i < m_alertPath.size(); i++)
			delete m_alertPath.at(i);
		m_alertPath.clear();
	}
	m_alertPath = alertPath;
}

size_t Enemy::GetAlertPathSize() const
{
	return m_alertPath.size();
}

Node * Enemy::GetAlertDestination() const
{
	return m_alertPath.at(m_alertPath.size() - 1);
}

EnemyState Enemy::getEnemyState() const
{
	return m_state;
}

void Enemy::setEnemeyState(EnemyState state)
{
	m_state = state;
}

EnemyTransitionState Enemy::getTransitionState() const
{
	return m_transState;
}

void Enemy::setTransitionState(EnemyTransitionState state)
{
	m_transState = state;
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

float Enemy::getTotalVisablilty() const
{
	return m_visCounter / m_visabilityTimer;
}

float Enemy::getMaxVisability() const
{
	return m_visCounter;
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
		if (m_currentPathNode != 0)
		{
			m_pathNodes.at(m_currentPathNode - 1)->Draw();
		}

		for (unsigned int i = m_currentPathNode; i < m_pathNodes.size(); ++i)
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

void Enemy::SetLenghtToPlayer(const DirectX::XMFLOAT4A& playerPos)
{
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&getPosition()), DirectX::XMLoadFloat4A(&playerPos));
	vec = DirectX::XMVector3LengthEst(vec);
	m_lenghtToPlayer = DirectX::XMVectorGetX(vec);
}

void Enemy::SetPlayerPointer(Player* player)
{
	m_PlayerPtr = player;
}

void Enemy::AddActTimer(double deltaTime)
{
	m_actTimer += deltaTime;
}

float Enemy::GetActTimer() const
{
	return m_actTimer;
}

void Enemy::SetActTimer(const float& time)
{
	m_actTimer = time;
}

void Enemy::AddHighAlertTimer(double deltaTime)
{
	m_HighAlertTime += deltaTime;
}

float Enemy::GetHighAlertTimer() const
{
	return m_HighAlertTime;
}

void Enemy::SetHightAlertTimer(const float& time)
{
	m_HighAlertTime = time;
}

void Enemy::setGrid(Grid * grid)
{
	m_grid = grid;
}

float Enemy::getVisCounter() const
{
	return m_visCounter;
}

void Enemy::_possessed(double deltaTime)
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
			RayCastListener::Ray* ray = RipExtern::g_rayListener->ShotRay(this->getBody(), this->getCamera()->getPosition(), this->getCamera()->getDirection(), Enemy::INTERACT_RANGE, false);
			if (ray)
			{
				for (RayCastListener::RayContact* con : ray->GetRayContacts())
				{
					if (*con->consumeState != 2)
					{
						if (con->originBody->GetObjectTag() == getBody()->GetObjectTag())
						{
							if (con->contactShape->GetBody()->GetObjectTag() == "LEVER")
							{
								*con->consumeState += 1;
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
			}

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
		DirectX::XMFLOAT4A pos = DirectX::XMFLOAT4A(headPosWorld.x, headPosWorld.y, headPosWorld.z, 1.0f);
		p_camera->setPosition(pos);
		//Camera Tilt
		p_CameraTilting(deltaTime, m_peektimer);

		static float lastOffset = 0.0f;
		static bool hasPlayed = true;
		static int last = 0;

		//Head Bobbing
		float offsetY = p_viewBobbing(deltaTime, m_cameraSpeed, this->getBody());

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

bool Enemy::_MoveTo(Node* nextNode, double deltaTime)
{
	_playFootsteps(deltaTime);
	if (abs(nextNode->worldPos.x - getPosition().x) <= 1.0f && abs(nextNode->worldPos.y - getPosition().z) <= 1.0f)
	{
		m_lv.newNode = true;
		m_lv.timer = 0.0f;
		m_currentPathNode++;
		if (m_currentPathNode == m_path.size())
		{
			std::reverse(m_path.begin(), m_path.end());
			std::reverse(m_pathNodes.begin(), m_pathNodes.end());

			for (auto node : m_pathNodes)
			{
				node->setPosition(node->getPosition().x, m_startYPos, node->getPosition().z);
			}

			m_isReversed = RipExtern::BoolReverser(m_isReversed);
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

bool Enemy::_MoveToAlert(Node * nextNode, double deltaTime)
{
	_playFootsteps(deltaTime);
	if (abs(nextNode->worldPos.x - getPosition().x) <= 1 && abs(nextNode->worldPos.y - getPosition().z) <= 1)
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
		float visPres = (float)m_vc->getVisibilityForPlayers()[0] / (float)Player::g_fullVisability;

		if (m_lenghtToPlayer < m_lengthToPlayerSpan)
		{
			visPres *= 1.5;
		}

		if (visPres > 0)
		{
			m_visCounter += visPres * deltaTime;
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

void Enemy::_Move(Node * nextNode, double deltaTime)
{
	// Get dirction to target
	float x = nextNode->worldPos.x - getPosition().x;
	float y = nextNode->worldPos.y - getPosition().z;

	float angle = atan2(y, x);

	float dx = cos(angle) * m_guardSpeed;
	float dy = sin(angle) * m_guardSpeed;

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

		m_lv.timer += deltaTime / REVERSE_SPEED;
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
		b3Vec3 lastVel = getLiniearVelocity();
		DirectX::XMFLOAT2 xmLastVel = { lastVel.x, lastVel.z };

		DirectX::XMVECTOR vLastVel = XMLoadFloat2(&xmLastVel);
		float l = DirectX::XMVectorGetX(DirectX::XMVector2Length(vLastVel));
		if (l < 0.01f)
		{
			vLastVel = DirectX::XMVectorSet(p_camera->getForward().x, p_camera->getForward().z, 0.0f,0.0f);
		}


		DirectX::XMFLOAT2 tmp = { x, y };
		DirectX::XMVECTOR vVel = XMLoadFloat2(&tmp);
		float dot = DirectX::XMVectorGetX(DirectX::XMVector2Dot(DirectX::XMVector2Normalize(vVel), (DirectX::XMVector2Normalize(vLastVel))));


		if (dot <= 0.0f)
		{
			m_lv.turnState = true;
			m_lv.timer = 0.0f;
			DirectX::XMFLOAT4A lastDir = p_camera->getForward();
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

			m_lv.timer += deltaTime / REVERSE_SPEED;
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
			m_lv.timer += deltaTime / TURN_SPEED;
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

	_RotateGuard(vel.x * deltaTime, vel.y * deltaTime, angle, deltaTime);
	auto lol = getLiniearVelocity();
	vel.x *= !m_lv.turnState;
	vel.y *= !m_lv.turnState;

	DirectX::XMStoreFloat2(&vel, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&vel)));

	setLiniearVelocity(vel.x * m_guardSpeed, getLiniearVelocity().y, vel.y * m_guardSpeed);
}

float Enemy::_getPathNodeRotation(DirectX::XMFLOAT2 first, DirectX::XMFLOAT2 last)
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

			RayCastListener::Ray * r = RipExtern::g_rayListener->ShotRay(PhysicsComponent::getBody(), getPosition(), DirectX::XMFLOAT4A(
				direction.x,
				direction.y,
				direction.z,
				direction.w),
				getLenght(getPosition(), DirectX::XMFLOAT4A(
					direction.x,
					direction.y,
					direction.z,
					direction.w)));
			if (r)
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

void Enemy::_handleStates(const double deltaTime)
{
	static float timer = 0.0f;
	timer += deltaTime;

	switch (m_transState)
	{
	case EnemyTransitionState::None:
		break;
	case EnemyTransitionState::Alerted:
		this->_onAlerted();
		break;
	case EnemyTransitionState::InvestigateSound:
		this->_onInvestigateSound();
		break;
	case EnemyTransitionState::InvestigateSight:
		this->_onInvestigateSight();
		break;
	case EnemyTransitionState::Observe:
		this->_onObserve();
		break;
	case EnemyTransitionState::SearchArea:
		this->_onSearchArea();
		break;
	case EnemyTransitionState::ReturnToPatrol:
		this->_onReturnToPatrol();
		break;
	case EnemyTransitionState::BeingPossessed:
		this->_onBeingPossessed();
		break;
	case EnemyTransitionState::BeingDisabled:
		this->_onBeingDisabled();
		break;
	}

	switch (m_state)
	{
	case EnemyState::Investigating_Sight:
		if (timer > 0.3f)
		{
			timer = 0.0f;
			//_investigating(currentGuard);
			this->_investigatingSight(deltaTime);
		}
		_detectTeleportSphere();
		
#ifdef _DEBUG

		std::cout << yellow << "Enemy State: Investigating Sight" << white << "\r";
#endif
		
		break;
	case EnemyState::Investigating_Sound:
		if (timer > 0.3f)
		{
			timer = 0.0f;
			//_investigateSound(currentGuard);
			this->_investigatingSound(deltaTime);
		}
		_detectTeleportSphere();
#ifdef _DEBUG

		std::cout << yellow << "Enemy State: Investigating Sound" << white << "\r";
#endif
		break;
	case EnemyState::Investigating_Room:
#ifdef _DEBUG

		std::cout << yellow << "Enemy State: Investigating Room" << white << "\r";
#endif
		//_investigateRoom(currentGuard, timer);
		this->_investigatingRoom(deltaTime);
		_detectTeleportSphere();
		break;
	case EnemyState::High_Alert:
		//_highAlert(currentGuard, deltaTime);
		this->_highAlert(deltaTime);
#ifdef _DEBUG

		std::cout << yellow << "Enemy State: High Alert" << white << "\r";
#endif
		_detectTeleportSphere();
		break;
	case EnemyState::Patrolling:
		//_patrolling(currentGuard);
		this->_patrolling(deltaTime);
#ifdef _DEBUG

		std::cout << yellow << "Enemy State: Patrolling" << white << "\r";
#endif
		_detectTeleportSphere();
		break;
	case EnemyState::Suspicious:
		//_suspicious(currentGuard, deltaTime);
		this->_suspicious(deltaTime);
#ifdef _DEBUG

		std::cout << yellow << "Enemy State: Suspicious" << white << "\r";
#endif
		_detectTeleportSphere();
		break;
	case EnemyState::Scanning_Area:
		//_ScanArea(currentGuard, deltaTime);
		this->_scanningArea(deltaTime);
#ifdef _DEBUG

		std::cout << yellow << "Enemy State: Scanning Area" << white << "\r";
#endif
		_detectTeleportSphere();
		break;
	case EnemyState::Possessed:

		break;
	case EnemyState::Disabled:

		break;
	}
}

void Enemy::_onAlerted()
{
#ifdef _DEBUG

	std::cout << green << "Enemy " << this->uniqueID <<  " Transition: Patrolling -> Suspicious" << white << std::endl;
#endif
	this->m_state = Suspicious;
	this->m_actTimer = 0;
	this->m_clearestPlayerPos = DirectX::XMFLOAT4A(0, 0, 0, 1);
	this->m_loudestSoundLocation = Enemy::SoundLocation();
	this->m_biggestVisCounter = 0;
	FMOD_VECTOR at = { this->getPosition().x, this->getPosition().y, this->getPosition().z };
	AudioEngine::PlaySoundEffect(RipSounds::g_grunt, &at, AudioEngine::Enemy);
	this->m_transState = EnemyTransitionState::None;
}

void Enemy::_onInvestigateSound()
{
	DirectX::XMFLOAT3 soundPos = this->getLoudestSoundLocation().soundPos;
	DirectX::XMFLOAT4A guardPos = this->getPosition();
	Tile soundTile = m_grid->WorldPosToTile(soundPos.x, soundPos.z);
	Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

	this->SetAlertVector(m_grid->FindPath(guardTile, soundTile));
	this->m_state = Investigating_Sound;
#ifdef _DEBUG

	std::cout << green << "Enemy " << this->uniqueID << " Transition: Suspicious -> Investigate Sound" << white << std::endl;
#endif
	this->m_transState = EnemyTransitionState::None;
}

void Enemy::_onInvestigateSight()
{
	DirectX::XMFLOAT4A playerPos = this->getClearestPlayerLocation();
	DirectX::XMFLOAT4A guardPos = this->getPosition();
	Tile playerTile = m_grid->WorldPosToTile(playerPos.x, playerPos.z);
	Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

	this->SetAlertVector(m_grid->FindPath(guardTile, playerTile));
	this->m_state = Investigating_Sight;
#ifdef _DEBUG

	std::cout << green << "Enemy " << this->uniqueID << " Transition: Suspicious -> Investigate Sight" << white << std::endl;
#endif
	this->m_transState = EnemyTransitionState::None;
}

void Enemy::_onObserve()
{
	DirectX::XMFLOAT4A guardPos = this->getPosition();
	Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);
	this->m_actTimer = 0;
	//this->SetAlertVector(m_grid->FindPath(guardTile, this->GetCurrentPathNode()->tile));
#ifdef _DEBUG

	std::cout << green << "Enemy " << this->uniqueID << " Transition: Investigating Source -> Scanning Area" << white << std::endl;
#endif
	this->m_state = Scanning_Area;
	this->m_transState = EnemyTransitionState::None;
}

void Enemy::_onSearchArea()
{
	this->m_actTimer = 0;
	this->m_state = Investigating_Room;
#ifdef _DEBUG

	std::cout << green << "Enemy " << this->uniqueID << " Transition: Scanning Area -> Investigating Area" << white << std::endl;
#endif
	this->m_transState = EnemyTransitionState::None;
	auto pos = getPosition();
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

void Enemy::_onReturnToPatrol()
{
	this->m_actTimer = 0;
	this->m_searchTimer = 0;
	this->m_state = Patrolling;
#ifdef _DEBUG

	std::cout << green << "Enemy " << this->uniqueID << " Transition: Suspicious -> Patrolling" << white << std::endl;
#endif
	this->m_transState = EnemyTransitionState::None;
}

void Enemy::_onBeingPossessed()
{
	this->m_state = EnemyState::Possessed;
	this->m_transState = EnemyTransitionState::None;
}

void Enemy::_onBeingDisabled()
{
	this->m_state = EnemyState::Disabled;
	this->m_transState = EnemyTransitionState::None;
}


void Enemy::_investigatingSight(const double deltaTime)
{
	if (this->GetAlertPathSize() > 0)
	{
		if (this->m_visCounter > this->m_biggestVisCounter)
		{
			DirectX::XMFLOAT4A playerPos = m_PlayerPtr->getPosition();
			Node * pathDestination = this->GetAlertDestination();

			if (abs(pathDestination->worldPos.x - playerPos.x) > 5.0f ||
				abs(pathDestination->worldPos.y - playerPos.z) > 5.0f)
			{
				DirectX::XMFLOAT4A guardPos = this->getPosition();
				Tile playerTile = m_grid->WorldPosToTile(playerPos.x, playerPos.z);
				Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

				this->SetAlertVector(m_grid->FindPath(guardTile, playerTile));
			}
		}
	}
	else
	{
		this->m_transState = EnemyTransitionState::Observe;
	}
}
void Enemy::_investigatingSound(const double deltaTime)
{
	if (this->GetAlertPathSize() > 0)
	{
		if (this->m_sl.percentage > this->m_loudestSoundLocation.percentage)
		{
			DirectX::XMFLOAT3 soundPos = this->m_sl.soundPos;
			Node * pathDestination = this->GetAlertDestination();

			if (abs(pathDestination->worldPos.x - soundPos.x) > 2.0f ||
				abs(pathDestination->worldPos.y - soundPos.z) > 2.0f)
			{
				DirectX::XMFLOAT4A guardPos = this->getPosition();
				Tile playerTile = m_grid->WorldPosToTile(soundPos.x, soundPos.z);
				Tile guardTile = m_grid->WorldPosToTile(guardPos.x, guardPos.z);

				this->SetAlertVector(m_grid->FindPath(guardTile, playerTile));
			}
		}
	}
	else
	{
		this->m_transState = EnemyTransitionState::Observe;
	}
}
void Enemy::_investigatingRoom(const double deltaTime)
{
	this->m_searchTimer += deltaTime;
	
	if (this->GetAlertPathSize() == 0)
	{
		this->setTransitionState(EnemyTransitionState::Observe);
	}
	if (this->getVisCounter() >= ALERT_TIME_LIMIT || this->getSoundLocation().percentage > SOUND_LEVEL)
	{
		//this->setTransitionState(EnemyTransitionState::Alerted);
	}
	else if (this->m_searchTimer > SEARCH_ROOM_TIME_LIMIT)
	{
		this->setTransitionState(EnemyTransitionState::ReturnToPatrol);
	}
	else if(this->m_transState != EnemyTransitionState::Observe)
	{
		_MoveToAlert(m_alertPath.at(0), deltaTime);
	}
}
void Enemy::_highAlert(const double deltaTime)
{
	this->m_HighAlertTime = deltaTime;
	if (this->GetHighAlertTimer() >= HIGH_ALERT_LIMIT)
	{
		this->SetHightAlertTimer(0.f);
		this->setClearestPlayerLocation(DirectX::XMFLOAT4A(0, 0, 0, 1));
		this->setLoudestSoundLocation(Enemy::SoundLocation());
		this->setBiggestVisCounter(0);
		this->m_state = EnemyState::Suspicious;
#ifdef _DEBUG

		std::cout << green << "Enemy " << this->uniqueID << " Transition: High Alert -> Suspicious" << white << std::endl;
#endif
	}
}
void Enemy::_suspicious(const double deltaTime)
{

	this->m_actTimer += deltaTime;
	float attentionMultiplier = 1.0f; // TEMP will be moved to Enemy
	if (this->m_actTimer > SUSPICIOUS_TIME_LIMIT / 3)
	{
		attentionMultiplier = 1.2f;
	}
	if (this->m_visCounter*attentionMultiplier >= this->m_biggestVisCounter)
	{
		this->setClearestPlayerLocation(m_PlayerPtr->getPosition());
		this->setBiggestVisCounter(this->getVisCounter()*attentionMultiplier);
		/*b3Vec3 dir(guard->getPosition().x - m_player->getPosition().x, guard->getPosition().y - m_player->getPosition().y, guard->getPosition().z - m_player->getPosition().z);
		b3Normalize(dir);
		guard->setDir(dir.x, dir.y, dir.y);*/
	}
	if (this->m_sl.percentage*attentionMultiplier >= this->m_loudestSoundLocation.percentage)
	{
		Enemy::SoundLocation temp = this->m_sl;
		temp.percentage *= attentionMultiplier;
		this->m_loudestSoundLocation = temp;
		/*b3Vec3 dir(guard->getPosition().x - guard->getLoudestSoundLocation().soundPos.x, guard->getPosition().y - guard->getLoudestSoundLocation().soundPos.y, guard->getPosition().z - guard->getLoudestSoundLocation().soundPos.z);
		b3Normalize(dir);
		guard->setDir(dir.x, dir.y, dir.y);*/
	}
	if (this->m_actTimer > SUSPICIOUS_TIME_LIMIT)
	{
		if (this->m_biggestVisCounter >= ALERT_TIME_LIMIT * 1.5)
			this->m_transState = EnemyTransitionState::InvestigateSight; //what was that?
		else if (this->m_loudestSoundLocation.percentage > SOUND_LEVEL*1.5)
			this->m_transState = EnemyTransitionState::InvestigateSound; //what was that noise?
		else
		{
			this->m_transState = EnemyTransitionState::ReturnToPatrol;
			//Must have been nothing...
		}
	}
}
void Enemy::_scanningArea(const double deltaTime)
{
	this->m_actTimer += deltaTime;
	//Do animation
	if (this->m_actTimer > SUSPICIOUS_TIME_LIMIT)
	{
		this->m_transState = EnemyTransitionState::SearchArea;
	}
}
void Enemy::_patrolling(const double deltaTime)
{
	if (this->getVisCounter() >= ALERT_TIME_LIMIT || this->getSoundLocation().percentage > SOUND_LEVEL) //"Huh?!" - Tim Allen
	{
		this->m_transState = EnemyTransitionState::Alerted;
	}
}
void Enemy::_disabled()
{

}
		