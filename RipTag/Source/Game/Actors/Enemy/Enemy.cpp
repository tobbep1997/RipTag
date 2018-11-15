#include "RipTagPCH.h"
#include "Enemy.h"

#include "EngineSource/3D Engine/Extern.h"
#include "EngineSource/3D Engine/RenderingManager.h"
#include "EngineSource/3D Engine/Components/Camera.h"
#include "EngineSource/3D Engine/Model/Managers/MeshManager.h"
#include "EngineSource/3D Engine/Model/Meshes/AnimatedModel.h"
#include "EngineSource/3D Engine/Model/Managers/TextureManager.h"
#include "EngineSource/3D Engine/3DRendering/Rendering/VisabilityPass/Component/VisibilityComponent.h"
#include "2D Engine/Quad/Components/HUDComponent.h"

//#todoREMOVE
#include "../../../Engine/EngineSource/Helper/AnimationDebugHelper.h"

Enemy::Enemy() : Actor(), CameraHolder(), PhysicsComponent()
{
	this->p_initCamera(new Camera(DirectX::XMConvertToRadians(150.0f / 2.0f), 250.0f / 150.0f, 0.1f, 50.0f));
	m_vc = new VisibilityComponent();
	m_vc->Init(this->p_camera);
	m_boundingFrustum = new DirectX::BoundingFrustum(DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&p_camera->getProjection())));
	//setOutline(true);
}

Enemy::Enemy(float startPosX, float startPosY, float startPosZ) : Actor(), CameraHolder()
{
	this->p_initCamera(new Camera(DirectX::XMConvertToRadians(150.0f / 2.0f), 250.0f / 150.0f, 0.1f, 50.0f));
	m_vc = new VisibilityComponent();
	m_vc->Init(this->p_camera);
	this->setPosition(startPosX, startPosY, startPosZ);
	this->setDir(1, 0, 0);
	this->getCamera()->setFarPlane(20);
	this->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	this->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_boundingFrustum = new DirectX::BoundingFrustum(DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&p_camera->getProjection())));
	
	srand(time(NULL));
	//setOutline(true);
}

Enemy::Enemy(b3World* world, float startPosX, float startPosY, float startPosZ) : Actor(), CameraHolder(), PhysicsComponent()
{
	this->p_initCamera(new Camera(DirectX::XMConvertToRadians(150.0f / 2.0f), 250.0f / 150.0f, 0.1f, 50.0f));
	m_vc = new VisibilityComponent();
	m_vc->Init(this->p_camera);
	this->setDir(1, 0, 0);
	this->getCamera()->setFarPlane(20);
	this->setModel(Manager::g_meshManager.getDynamicMesh("STATE"));
	this->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	this->getAnimatedModel()->SetSkeleton(Manager::g_animationManager.getSkeleton("STATE"));

	{
		auto idleAnim = Manager::g_animationManager.getAnimation("STATE", "IDLE_ANIMATION").get();
		auto walkAnim = Manager::g_animationManager.getAnimation("STATE", "WALK_FORWARD_ANIMATION").get();
		auto& machine = getAnimatedModel()->InitStateMachine(1);
		auto state = machine->AddBlendSpace1DState("walk_state", &m_currentMoveSpeed, 0.0, 1.0);
		state->AddBlendNodes({ {idleAnim, 0.0}, {walkAnim, 0.0} });
		//state->AddRow(0.0f, { {idleAnim, 0.0}, {idleAnim, 1.0} });
		//state->AddRow(1.0f, { {idleAnim, 0.0}, {idleAnim, 1.0} });
		machine->SetState("walk_state");
		//this->getAnimatedModel()->SetPlayingClip(Manager::g_animationManager.getAnimation("STATE", "IDLE_ANIMATION").get());
		this->getAnimatedModel()->Play();

		//#todoREMOVE
		/*
		auto& layerMachine = getAnimatedModel()->InitLayerStateMachine(1);
		auto lState = layerMachine->AddBlendSpace1DAdditiveState("pitch_state", &Player::m_currentPitch, -0.9, 0.9);
		std::vector<SM::BlendSpace1DAdditive::BlendSpaceLayerData> layerData;
		SM::BlendSpace1DAdditive::BlendSpaceLayerData up;
		up.clip = Manager::g_animationManager.getAnimation("STATE", "PITCH_UP_ANIMATION").get();
		up.location = .9f;
		up.weight = 1.0f;
		SM::BlendSpace1DAdditive::BlendSpaceLayerData down;
		down.clip = Manager::g_animationManager.getAnimation("STATE", "PITCH_DOWN_ANIMATION").get();
		down.location = -.9f;
		down.weight = 1.0f;

		layerData.push_back(down);
		layerData.push_back(up);

		lState->AddBlendNodes(layerData);
		layerMachine->SetState("pitch_state");
		*/
	}
	b3Vec3 pos(1, 0.9, 1);
	PhysicsComponent::Init(*world, e_staticBody,pos.x, pos.y, pos.z, false, 0); //0.5f, 0.9f, 0.5f //1,0.9,1

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
	this->setModelTransform(DirectX::XMMatrixTranslation(0.0, -1.0, 0.0));
	setScale(.015f, .015f, .015f);
	setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	setTextureTileMult(2, 2);
	m_boundingFrustum = new DirectX::BoundingFrustum(DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&p_camera->getProjection())));

	//setOutline(true);
}


Enemy::~Enemy()
{
	delete m_vc;
	this->Release(*this->getBody()->GetScene());
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
	if (getAnimatedModel())
		getAnimatedModel()->Update(deltaTime);
	
	m_sinWaver += deltaTime;

	if (m_PlayerPtr->GetMapPicked())
	{
		m_nodeFootPrintsEnabled = true;
	}

	if (!m_disabled)
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

		if (!m_inputLocked)
		{
			_handleInput(deltaTime);
		}
		else
		{
			if (m_alertPath.size() > 0 )
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

		_CheckPlayer(deltaTime);
	}
	else
	{
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
		PhysicsComponent::p_setRotation(p_camera->getYRotationEuler().x + DirectX::XMConvertToRadians(85), p_camera->getYRotationEuler().y, p_camera->getYRotationEuler().z);
		m_visCounter = 0;
	}
	getBody()->SetType(e_dynamicBody);
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

	if (Input::MoveRight() > 1.0f || Input::MoveForward() > 1.0f)
	{
		dir.x *= m_scrollMoveModifier;
		dir.y *= m_scrollMoveModifier;
	}

	DirectX::XMVECTOR vDir = DirectX::XMLoadFloat2(&dir);
	float length = DirectX::XMVectorGetX(DirectX::XMVector2Length(vDir));

	if (length > 1.0)
		vDir = DirectX::XMVector2Normalize(vDir);

	DirectX::XMStoreFloat2(&dir, vDir);

	x = dir.x * m_moveSpeed  * RIGHT.x;
	x += dir.y * m_moveSpeed * forward.x;
	z = dir.y * m_moveSpeed * forward.z;
	z += dir.x * m_moveSpeed * RIGHT.z;

	//p_setPosition(getPosition().x + x, getPosition().y, getPosition().z + z);
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
		if (Input::PeekRight() > 0.1 || Input::PeekRight() < -0.1)
		{

		}
		else
		{
			if (m_peekRotate > 0.05f || m_peekRotate < -0.05f)
			{
				if (m_peekRotate > 0)
				{
					p_camera->Rotate(0.0f, -0.05f, 0.0f);
					m_peekRotate -= 0.05;
				}
				else
				{
					p_camera->Rotate(0.0f, +0.05f, 0.0f);
					m_peekRotate += 0.05;
				}

			}
			else
			{
				m_peekRotate = 0;
			}
			//m_peekRotate = 0;
		}

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

void Enemy::setSoundLocation(const SoundLocation & sl)
{
	m_sl = sl;
}

const Enemy::SoundLocation & Enemy::getSoundLocation() const
{
	return m_sl;
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
		float offsetY = p_viewBobbing(deltaTime, Input::MoveForward(), m_moveSpeed, p_moveState);

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
		p_camera->setPosition(pos);
	}
	else
	{
		DirectX::XMFLOAT4A pos = getPosition();
		pos.y += m_standHeight;
		p_camera->setPosition(pos);
		//pos = p_CameraTilting(deltaTime, Input::PeekRight());
		float offsetY = p_viewBobbing(deltaTime, Input::MoveForward(), m_moveSpeed, p_moveState);

		pos.y += offsetY;

		//pos.y += p_Crouching(deltaTime, m_crouchAnimStartPos, p_camera->getPosition());
		p_camera->setPosition(pos);
	}
}

bool Enemy::_MoveTo(Node* nextNode, double deltaTime)
{
	_playFootsteps(deltaTime);
	if (abs(nextNode->worldPos.x - getPosition().x) <= 1 && abs(nextNode->worldPos.y - getPosition().z) <= 1)
	{
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
		float x = nextNode->worldPos.x - getPosition().x;
		float y = nextNode->worldPos.y - getPosition().z;

		float angle = atan2(y, x);

		float dx = cos(angle) * m_guardSpeed * deltaTime;
		float dy = sin(angle) * m_guardSpeed * deltaTime;
		
		//Update current movespeed
		{
			auto deltaVector = DirectX::XMVectorSet(dx, dy, 0.0, 0.0);
			m_currentMoveSpeed = DirectX::XMVectorGetX(DirectX::XMVector2LengthEst(deltaVector));
		}

		_RotateGuard(x, y, angle, deltaTime);

		setPosition(getPosition().x + dx, getPosition().y, getPosition().z + dy);
	//DirectX::XMFLOAT4A a = DirectX::XMFLOAT4A(nextNode->worldPos.x, 0, nextNode->worldPos.y, 1.0f);
	//DirectX::XMFLOAT4A b = DirectX::XMFLOAT4A(m_path.at(m_currentPathNode)->worldPos.x, 0, m_path.at(m_currentPathNode)->worldPos.y,  1.0f);
//FREDRIK FIXAR PÅ MÅNDAG	//DirectX::XMVECTOR direction = DirectX::XMLoadFloat4A(&a);
	//DirectX::XMVECTOR current = DirectX::XMLoadFloat4A(&b);
	//DirectX::XMVECTOR moveVector = DirectX::XMVectorSubtract(current, direction);
	//this->setLiniearVelocity(DirectX::XMVectorGetX(moveVector), this->getLiniearVelocity().y, DirectX::XMVectorGetZ(moveVector));
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
	}
	else
	{
		float x = nextNode->worldPos.x - getPosition().x;
		float y = nextNode->worldPos.y - getPosition().z;

		float angle = atan2(y, x);

		float dx = cos(angle) * m_guardSpeed * deltaTime;
		float dy = sin(angle) * m_guardSpeed * deltaTime;

		_RotateGuard(x, y, angle, deltaTime);

		setPosition(getPosition().x + dx, getPosition().y, getPosition().z + dy);
	}
	return false;
}

/*void Enemy::_MoveBackToPatrolRoute(Node * nextNode, double deltaTime)
{
	_playFootsteps(deltaTime);
	if (abs(nextNode->worldPos.x - getPosition().x) <= 1 && abs(nextNode->worldPos.y - getPosition().z) <= 1)
	{
		delete nextNode;
		m_alertPath.erase(m_alertPath.begin());
	}
	else
	{
		float x = nextNode->worldPos.x - getPosition().x;
		float y = nextNode->worldPos.y - getPosition().z;
		
		float angle = atan2(y, x);

		float dx = cos(angle) * m_guardSpeed * deltaTime;
		float dy = sin(angle) * m_guardSpeed * deltaTime;

		_RotateGuard(x, y, angle, deltaTime);

		setPosition(getPosition().x + dx, getPosition().y, getPosition().z + dy);
	}
}*/

void Enemy::_RotateGuard(float x, float y, float angle, float deltaTime)
{
	p_camera->setDirection(x, p_camera->getDirection().y, y);
	DirectX::XMFLOAT4A cameraRotationY = p_camera->getYRotationEuler();
	p_camera->Rotate(0, angle * deltaTime, 0);
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