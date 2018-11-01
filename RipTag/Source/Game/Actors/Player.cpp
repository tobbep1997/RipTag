#include "Player.h"
#include "InputManager/InputHandler.h"
#include "InputManager/XboxInput/GamePadHandler.h"
#include "EngineSource/3D Engine/RenderingManager.h"
#include <algorithm>
#include <iostream>
#include <bits.h>
#include "../../../Engine/EngineSource/Helper/HelperFunctions.h"
#include "../Handlers/CameraHandler.h"

Player::Player() : Actor(), CameraHolder(), PhysicsComponent(), HUDComponent()
{
	Manager::g_textureManager.loadTextures("CROSS");
	p_initCamera(new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 0.1f, 110.0f));
	p_camera->setPosition(0, 0, 0);
	m_lockPlayerInput = false;
	
	VisabilityAbility * visAbl = new VisabilityAbility();
	visAbl->setOwner(this);
	visAbl->setIsLocal(true);
	visAbl->Init();
	visAbl->setManaCost(1);

	VisabilityAbility * visAbl2 = new VisabilityAbility();
	visAbl2->setOwner(this);
	visAbl2->setIsLocal(true);
	visAbl2->Init();

	TeleportAbility * m_teleport = new TeleportAbility();
	m_teleport->setOwner(this);
	m_teleport->setIsLocal(true);
	m_teleport->Init();

	DisableAbility * m_dis = new DisableAbility();
	m_dis->setOwner(this);
	m_dis->setIsLocal(true);
	m_dis->Init();

	m_abilityComponents = new AbilityComponent*[m_nrOfAbilitys];
	m_abilityComponents[0] = m_teleport;
	m_abilityComponents[1] = visAbl;
	m_abilityComponents[2] = m_dis;
	m_abilityComponents[3] = visAbl2;

	/*m_possess.setOwner(this);
	m_possess.Init();
	
	m_blink.setOwner(this);
	m_blink.Init();*/

	Quad * quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.1f, 0.15f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	quad->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	HUDComponent::AddQuad(quad, 49);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.15f, 0.1f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	quad->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	HUDComponent::AddQuad(quad, 50);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.1f, 0.05f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	quad->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	HUDComponent::AddQuad(quad, 50);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.05f, 0.1f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	quad->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	HUDComponent::AddQuad(quad, 50);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.5f, 0.5f), DirectX::XMFLOAT2A(5.0f / 16.0f, 5.0f /9.0f));
	quad->setUnpressedTexture(Manager::g_textureManager.getTexture("CROSS"));
	HUDComponent::AddQuad(quad);


	m_maxMana = STANDARD_START_MANA;
	m_currentMana = m_maxMana;

	m_manaBar = new Quad();
	m_manaBar->init(DirectX::XMFLOAT2A(0.2f, 0.2f), DirectX::XMFLOAT2A(5.0f / 16.0f, 5.0f / 9.0f));
	m_manaBar->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_manaBar->setPivotPoint(Quad::PivotPoint::lowerLeft);


	HUDComponent::AddQuad(m_manaBar);

}

Player::Player(RakNet::NetworkID nID, float x, float y, float z) : Actor(), CameraHolder(), PhysicsComponent()
{
	p_initCamera(new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 0.1f, 50.0f));
	p_camera->setPosition(x, y, z);
	this->m_rayListener = new RayCastListener();
	m_lockPlayerInput = false;
}

Player::~Player()
{
	for (unsigned short int i = 0; i < m_nrOfAbilitys; i++)
		delete m_abilityComponents[i];
	delete[] m_abilityComponents;
}


void Player::Init(b3World& world, b3BodyType bodyType, float x, float y, float z)
{
	PhysicsComponent::Init(world, bodyType, x, y, z);
	this->getBody()->SetObjectTag("PLAYER");
	this->getBody()->AddToFilters("TELEPORT");
	setUserDataBody(this);

	setEntityType(EntityType::PlayerType);
	setColor(10, 10, 0, 1);

	setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	setScale(1.0f, 1.0f, 1.0f);
	setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	setTextureTileMult(2, 2);
}

void Player::BeginPlay()
{

}
#include <math.h>
void Player::Update(double deltaTime)
{
	using namespace DirectX;

	//set jumpedThisFrame to false
	m_jumpedThisFrame = false;

	//calculate walk direction (-1, 1, based on camera) and movement speed
	{
		///Speed
		auto physSpeed = this->getLiniearVelocity();
		
		//if y speed is zero, set isInAir to false,
		if (std::abs(physSpeed.y) < 0.001f)
			m_isInAir = false;

		float speed = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSet(physSpeed.x, 0.0, physSpeed.z, 0)));
		m_currentSpeed = std::clamp(std::fabs(speed), 0.0f, 3.0f);

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
		///AssertNotNAN(m_currentDirection);

	}

	

	if (m_lockPlayerInput == false)
	{
		if (InputHandler::getWindowFocus())
		{
			_handleInput(deltaTime);
		}
		
	}

	m_manaBar->setScale((float)m_currentMana / (float)m_maxMana, 0.1f);

	if (InputHandler::isKeyPressed('I'))
	{
		RefillMana(10);
	}
	if (InputHandler::isKeyPressed('J'))
	{
		m_maxMana += 10;
	}

	m_abilityComponents[m_currentAbility]->Update(deltaTime);
	/*m_possess.Update(deltaTime);
	m_blink.Update(deltaTime);*/
	_cameraPlacement(deltaTime);
	//HUDComponent::HUDUpdate(deltaTime);
	
	if (Input::SelectAbility1())	
		m_currentAbility = Ability::TELEPORT;		
	else if (Input::SelectAbility2())	
		m_currentAbility = Ability::VISIBILITY;	
	else if (Input::SelectAbility3())	
		m_currentAbility = Ability::DISABLE;	
	else if (Input::SelectAbility4())
		m_currentAbility = Ability::VIS2;
	
	if (GamePadHandler::IsUpDpadPressed())
		m_currentAbility = Ability::TELEPORT;
	else if (GamePadHandler::IsRightDpadPressed())
		m_currentAbility = Ability::VISIBILITY;
	else if (GamePadHandler::IsDownDpadPressed())
		m_currentAbility = Ability::DISABLE;
	else if (GamePadHandler::IsLeftDpadPressed())
		m_currentAbility = Ability::VIS2;

	HUDComponent::ResetStates();
	HUDComponent::setSelectedQuad(m_currentAbility);
}

void Player::PhysicsUpdate()
{
	p_updatePhysics(this);
}

void Player::setPosition(const float& x, const float& y, const float& z, const float& w)
{
	Transform::setPosition(x, y, z, w);
	PhysicsComponent::p_setPosition(x, y, z);
}
	
const float & Player::getVisability() const
{
	return m_visability;
}

const int & Player::getFullVisability() const
{
	return g_fullVisability;
}

bool Player::CheckManaCost(const int& manaCost)
{
	if (manaCost <= m_currentMana)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Player::DrainMana(const int& manaCost)
{
	if (manaCost <= m_currentMana)
	{
		m_currentMana -= manaCost;
		return true;
	}
	else
	{
		return false;
	}
}

void Player::RefillMana(const int& manaFill)
{
	m_currentMana += manaFill;

	int rest = m_maxMana - m_currentMana;
	if (rest < 0)
	{
		m_currentMana += rest;
	}
}

void Player::Draw()
{
	for (int i = 0; i < m_nrOfAbilitys; i++)
		m_abilityComponents[i]->Draw();
	Drawable::Draw();
	HUDComponent::HUDDraw();
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

void Player::SendOnUpdateMessage()
{
	Network::ENTITYUPDATEPACKET packet = Network::ENTITYUPDATEPACKET(
		Network::ID_PLAYER_UPDATE,
		Network::Multiplayer::GetInstance()->GetNetworkID(),
		PlayerState::Idle,
		this->getPosition(),
		this->getEulerRotation());

	
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

	TeleportAbility * tp_ptr = dynamic_cast<TeleportAbility*>(m_abilityComponents[m_currentAbility]);
	DisableAbility * dis_ptr = dynamic_cast<DisableAbility*>(m_abilityComponents[m_currentAbility]);
	VisabilityAbility * vis_ptr = dynamic_cast<VisabilityAbility*>(m_abilityComponents[m_currentAbility]);
	//unique based on active ability
	switch (this->m_currentAbility)
	{
	case Ability::TELEPORT:
		packet.ability = (unsigned int)TELEPORT;
		packet.start = tp_ptr->getStart();
		packet.velocity = tp_ptr->getVelocity();
		packet.state = tp_ptr->getState();
		break;
	case Ability::DISABLE:
		packet.ability = (unsigned int)DISABLE;
		packet.start = dis_ptr->getStart();
		packet.velocity = dis_ptr->getVelocity();
		packet.state = dis_ptr->getState();
		break;
	case Ability::VIS2:
	case Ability::VISIBILITY:
		packet.ability = (unsigned int)VISIBILITY;
		packet.start = vis_ptr->getStart();
		packet.velocity = vis_ptr->getLastColor();
		packet.state = vis_ptr->getState();
		break;
	}

	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(ENTITYABILITYPACKET), PacketPriority::LOW_PRIORITY);
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

	
	_onSprint();
	_onMovement();
	_onCrouch();
	_onJump();
	_onAbility(deltaTime);
	_onBlink();
	_onPossess();
	_onInteract();
	_onRotate(deltaTime);
}

void Player::_onMovement()
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

	x = Input::MoveRight() * m_moveSpeed  * RIGHT.x;
	x += Input::MoveForward() * m_moveSpeed * forward.x;
	z = Input::MoveForward() * m_moveSpeed * forward.z;
	z += Input::MoveRight() * m_moveSpeed * RIGHT.z;

	setLiniearVelocity(x, getLiniearVelocity().y, z);
}

void Player::_onSprint()
{
	if (Input::Sprinting())
	{
		m_moveSpeed = MOVE_SPEED * SPRINT_MULT;
	}
	else
	{
		m_moveSpeed = MOVE_SPEED;
	}

	if (m_kp.crouching)
	{
		m_moveSpeed = MOVE_SPEED * 0.5f;
	}
}

void Player::_onCrouch()
{
	if (Input::Crouch())
	{
		if (m_kp.crouching == false)
		{
			m_standHeight = this->p_camera->getPosition().y;
			this->CreateBox(0.5f, 0.10f, 0.5f);
			this->setPosition(this->getPosition().x, this->getPosition().y - 0.4, this->getPosition().z, 1);
			m_kp.crouching = true;
		}
	}
	else
	{
		if (m_kp.crouching)
		{
			m_standHeight = this->p_camera->getPosition().y;
			this->CreateBox(0.5, 0.5, 0.5);
			this->setPosition(this->getPosition().x, this->getPosition().y + 0.4, this->getPosition().z, 1);
			m_kp.crouching = false;
		}
	}
}

void Player::_onBlink()
{
	//if (Input::Blink()) //Phase acts like short range teleport through objects
	//{
	//	if (m_kp.blink == false)
	//	{
	//		m_blink.Use();
	//		m_kp.blink = true;
	//	}
	//}
	//else
	//{
	//	m_kp.blink = false;
	//}
}

void Player::_onPossess()
{
	//if (Input::Possess()) //Phase acts like short range teleport through objects
	//{
	//	
	//	if (m_kp.possess == false)
	//	{
	//		m_possess.Use();
	//		m_kp.possess = true;
	//	}
	//}
	//else
	//{
	//	m_kp.possess = false;
	//}
}

void Player::_onRotate(double deltaTime)
{
	if (!unlockMouse)
	{	
		float deltaY = Input::TurnUp();
		float deltaX = Input::TurnRight();
		if (deltaX && !Input::PeekRight())
		{
			p_camera->Rotate(0.0f, deltaX * 5 * deltaTime, 0.0f);
		}
		if (deltaY) 
		{
			if ((p_camera->getDirection().y - deltaY * 5 * deltaTime) < 0.90f)
			{
				p_camera->Rotate(deltaY * 5 * deltaTime, 0.0f, 0.0f);
			}
			else if (p_camera->getDirection().y >= 0.90f)
			{
				p_camera->setDirection(p_camera->getDirection().x, 0.89f, p_camera->getDirection().z);
			}
			if ((p_camera->getDirection().y - deltaY * 5 * deltaTime) > -0.90f)
			{
				p_camera->Rotate(deltaY * 5 * deltaTime, 0.0f, 0.0f);
			}
			else if (p_camera->getDirection().y <= -0.90f)
			{
				p_camera->setDirection(p_camera->getDirection().x, -0.89f, p_camera->getDirection().z);
			}
		
			
		}
		
	}
}

void Player::_onJump()
{
	if (Input::Jump())
	{
		if (m_kp.jump == false)
		{
			addForceToCenter(0, JUMP_POWER, 0);
			m_kp.jump = true;
			m_jumpedThisFrame = true;
			m_isInAir = true;
		}
	}


	float epsilon = 0.002;
	if (this->getLiniearVelocity().y < epsilon && this->getLiniearVelocity().y > -epsilon)
		m_kp.jump = false;
}

void Player::_onInteract()
{
	if (Input::Interact()) //Phase acts like short range teleport through objects
	{
		if (m_kp.interact == false)
		{
			RipExtern::m_rayListener->ShotRay(this->getBody(), this->getCamera()->getDirection(), Player::INTERACT_RANGE);
			for (RayCastListener::RayContact con : RipExtern::m_rayListener->GetContacts())
			{
				if (con.originBody->GetObjectTag() == getBody()->GetObjectTag())
				{
					if (con.contactShape->GetBody()->GetObjectTag() == "ITEM")
					{
						//do the pickups
					}
					else if (con.contactShape->GetBody()->GetObjectTag() == "LEVER")
					{
						//std::cout << "Lever Found!" << std::endl;
						//Pull Levers
					}
					else if (con.contactShape->GetBody()->GetObjectTag() == "TORCH")
					{
						//Snuff out torches (example)
					}
					else if (con.contactShape->GetBody()->GetObjectTag() == "Enemy")
					{
						//Snuff out torches (example)
					}
					else if (con.contactShape->GetBody()->GetObjectTag() == "BLINK_WALL")
					{
						//Snuff out torches (example)
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

void Player::_onAbility(double dt)
{
	this->m_abilityComponents[m_currentAbility]->Update(dt);
}

void Player::_cameraPlacement(double deltaTime)
{
	float cameraOffset = 1.0f;
	DirectX::XMFLOAT4A pos = getPosition();
	pos.y += cameraOffset;
	p_camera->setPosition(pos);
	pos = p_CameraTilting(deltaTime, Input::PeekRight(), getPosition());
	pos.y += p_viewBobbing(deltaTime, Input::MoveForward(), m_moveSpeed);
	pos.y += p_Crouching(deltaTime, this->m_standHeight, p_camera->getPosition());
	p_camera->setPosition(pos);
}


