#include "Player.h"
#include "InputManager/InputHandler.h"
#include "InputManager/XboxInput/GamePadHandler.h"
#include "EngineSource/3D Engine/RenderingManager.h"
#include <algorithm>
#include <iostream>
#include <bits.h>
#include "../Handlers/CameraHandler.h"

Player::Player() : Actor(), CameraHolder(), PhysicsComponent(), HUDComponent()
{
	Manager::g_textureManager.loadTextures("CROSS");
	p_initCamera(new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 0.1f, 50.0f));
	p_camera->setPosition(0, 0, 0);
	m_lockPlayerInput = false;
	
	visSphear = new Drawable();
	visSphear->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	visSphear->setScale(0.2f, 0.2f, 0.2f);
	visSphear->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	visSphear->setPosition(5, 5, 2);
	visSphear->setColor(1, 1, 1, 1.0f);
	visSphear->setEntityType(EntityType::ExcludeType);
	
	m_teleport.setOwner(this);
	m_teleport.Init();

	m_possess.setOwner(this);
	m_possess.Init();
	
	m_blink.setOwner(this);
	m_blink.Init();

	Quad * quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.05f, 0.1f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	quad->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	HUDComponent::AddQuad(quad, 49);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.1f, 0.1f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	quad->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	HUDComponent::AddQuad(quad, 50);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.15f, 0.1f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	quad->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	quad->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	HUDComponent::AddQuad(quad, 51);

	quad = new Quad();
	quad->init(DirectX::XMFLOAT2A(0.5f, 0.5f), DirectX::XMFLOAT2A(5.0f / 16.0f, 5.0 /9.0f));
	quad->setUnpressedTexture(Manager::g_textureManager.getTexture("CROSS"));
	HUDComponent::AddQuad(quad);
}

Player::~Player()
{	
	delete visSphear;
}

void Player::BeginPlay()
{

}

void Player::Update(double deltaTime)
{
	if (m_lockPlayerInput == false)
	{
		if (InputHandler::getWindowFocus())
		{
			_handleInput(deltaTime);
		}
		
	}
	m_teleport.Update(deltaTime);
	m_possess.Update(deltaTime);
	m_blink.Update(deltaTime);
	_cameraPlacement(deltaTime);
	HUDComponent::HUDUpdate(deltaTime);
}

void Player::PhysicsUpdate(double deltaTime)
{
	p_updatePhysics(this);
}

void Player::setPosition(const float& x, const float& y, const float& z, const float& w)
{
	Transform::setPosition(x, y, z, w);
	PhysicsComponent::p_setPosition(x, y, z);
}

int Player::getPossessState()
{
	return m_possess.getPossessState();
}
	
void Player::Draw()
{
	m_teleport.Draw();
	Drawable::Draw();
	if (Input::CheckVisability())
	{
		visSphear->Draw();
	}
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

void Player::SendOnJumpMessage()
{
	Network::ENTITY_EVENT packet(Network::ID_PLAYER_JUMP, Network::Multiplayer::GetInstance()->GetNetworkID());
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::ENTITY_EVENT), PacketPriority::MEDIUM_PRIORITY);
}

void Player::SendOnMovementMessage()
{
	DirectX::XMFLOAT4A pos = this->getPosition();
	Network::ENTITY_MOVE packet(Network::ID_PLAYER_MOVE, Network::Multiplayer::GetInstance()->GetNetworkID(), pos.x, pos.y, pos.z);
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::ENTITY_MOVE), PacketPriority::MEDIUM_PRIORITY);
}


void Player::RegisterThisInstanceToNetwork()
{
	Network::Multiplayer::addToOnSendFuncMap("Jump", std::bind(&Player::SendOnJumpMessage, this));
	Network::Multiplayer::addToOnSendFuncMap("MoveRight", std::bind(&Player::SendOnMovementMessage, this));
	Network::Multiplayer::addToOnSendFuncMap("MoveLeft", std::bind(&Player::SendOnMovementMessage, this));
	Network::Multiplayer::addToOnSendFuncMap("MoveForward", std::bind(&Player::SendOnMovementMessage, this));
	Network::Multiplayer::addToOnSendFuncMap("MoveBackward", std::bind(&Player::SendOnMovementMessage, this));
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
	_onBlink();
	_onPossess();
	_onRotate(deltaTime);
	_onTeleport(deltaTime);
	_onCheckVisibility();
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
	if (Input::Blink()) //Phase acts like short range teleport through objects
	{
		if (m_kp.blink == false)
		{
			m_blink.Use();
			//this->Phase(10);
			m_kp.blink = true;
		}
	}
	else
	{
		m_kp.blink = false;
	}
}

void Player::_onPossess()
{
	if (Input::Possess()) //Phase acts like short range teleport through objects
	{
		
		if (m_kp.possess == false)
		{
			m_possess.Use();
			m_kp.possess = true;
		}
	}
	else
	{
		m_kp.possess = false;
	}
}

void Player::_onRotate(double deltaTime)
{
	if (!unlockMouse)
	{	
		float deltaY = Input::TurnUp();
		float deltaX = Input::TurnRight();
		if (deltaX && !Input::PeekRight())
			p_camera->Rotate(0.0f, deltaX * 5 * deltaTime, 0.0f);
		if (deltaY)
			p_camera->Rotate(deltaY * 5 * deltaTime, 0.0f, 0.0f);
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
		}
	}
	else
	{
		m_kp.jump = false;
	}
}

void Player::_onCheckVisibility()
{
	if (Input::CheckVisability())
	{
		DirectX::XMFLOAT4A po = Transform::getPosition();
		po.y += 1;
		DirectX::XMVECTOR ve = DirectX::XMLoadFloat4A(&po);
		DirectX::XMVECTOR cm = DirectX::XMLoadFloat4A(&p_camera->getDirection());
		DirectX::XMStoreFloat4A(&po, DirectX::XMVectorAdd(ve, cm));

		visSphear->setPosition(po);
		visSphear->setColor(2.0f * m_visability, 2.0f * m_visability, 2.0f * m_visability, 1);

		float temp = (float)m_visability / (float)g_fullVisability;

		#if _DEBUG
			ImGui::Begin("visability");
			ImGui::Text("Vis: %f", temp);
			ImGui::End();
		#endif
	}
}

void Player::_onTeleport(double deltaTime)
{
	if (Input::Teleport())
	{
		m_teleport.Use();
	}
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


