#include "Player.h"
#include "InputManager/InputHandler.h"
#include "InputManager/XboxInput/GamePadHandler.h"
#include "EngineSource/3D Engine/RenderingManager.h"
#include <algorithm>

Player::Player() : Actor(), CameraHolder(), PhysicsComponent()
{
	p_initCamera(new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 0.1f, 50.0f));
	p_camera->setPosition(0, 0, 0);
	this->m_rayListener = new RayCastListener();
	m_lockPlayerInput = false;
	
	visSphear = new Drawable();
	visSphear->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	visSphear->setScale(0.2f, 0.2f, 0.2f);
	visSphear->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	visSphear->setPosition(5, 5, 2);
	visSphear->setColor(1, 1, 1, 1.0f);
	visSphear->setEntityType(EntityType::ExcludeType);
}

Player::Player(RakNet::NetworkID nID, float x, float y, float z) : Actor(), CameraHolder(), PhysicsComponent()
{
	p_initCamera(new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 0.1f, 50.0f));
	p_camera->setPosition(x, y, z);
	this->m_rayListener = new RayCastListener();
	m_lockPlayerInput = false;

	visSphear = new Drawable();
	visSphear->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	visSphear->setScale(0.2f, 0.2f, 0.2f);
	visSphear->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	visSphear->setPosition(5, 5, 2);
	visSphear->setColor(1, 1, 1, 1.0f);
	visSphear->setEntityType(EntityType::PlayerType);
}

Player::~Player()
{	
	delete this->m_rayListener;
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
	m_teleport.UpdateLight();
	_cameraPlacement(deltaTime);
}

void Player::PhysicsUpdate()
{
	p_updatePhysics(this);

	//temporary
	std::string output;
	output += "X: " + std::to_string(this->getLiniearVelocity().x);
	output += "\nY: " + std::to_string(this->getLiniearVelocity().y);
	output += "\nZ: " + std::to_string(this->getLiniearVelocity().z);

	ImGui::Begin("Player Velocity");
	ImGui::Text(output.c_str());
	ImGui::End();
}

void Player::setPosition(const float& x, const float& y, const float& z, const float& w)
{
	Transform::setPosition(x, y, z, w);
	PhysicsComponent::p_setPosition(x, y, z);
}

void Player::Phase(float searchLength)
{
	this->m_rayListener->shotRay(this->getBody(), p_camera->getDirection(), searchLength);
	if (this->m_rayListener->type == 1)
	{
		p_setPosition(
			this->m_rayListener->contactPoint.x + (
				(abs(this->m_rayListener->contactPoint.x - this->m_rayListener->shape->GetBody()->GetTransform().translation.x) * 2) *
				(-this->m_rayListener->normal.x)), 
			this->getPosition().y,
			this->m_rayListener->contactPoint.z + (
				(abs(this->m_rayListener->contactPoint.z - this->m_rayListener->shape->GetBody()->GetTransform().translation.z) * 2) *
				(-this->m_rayListener->normal.z))
			);
		if (this->m_rayListener->normal.y != 0)
		{
			p_setPosition(
				this->getPosition().x,
				this->m_rayListener->contactPoint.y + (
				(abs(this->m_rayListener->contactPoint.y - this->m_rayListener->shape->GetBody()->GetTransform().translation.y) * 2) *
					(-this->m_rayListener->normal.y)),
				this->getPosition().z
			);
		}
	}
	this->m_rayListener->clear();
}

void Player::InitTeleport(b3World & world)
{
	m_teleport.Init(world, e_dynamicBody, 0.1f, 0.1f, 0.1f);
	m_teleport.setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	m_teleport.setScale(0.1f, 0.1f, 0.1f);
	m_teleport.setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_teleport.setGravityScale(0.001f);
	m_teleport.setPosition(-100.0f, -100.0f, -100.0f);
}

void Player::ReleaseTeleport(b3World & world)
{
	this->m_teleport.Release(world);
}

void Player::Draw()
{
	m_teleport.Draw();
	m_teleport.QueueLight();
	Drawable::Draw();
	if (Input::CheckVisability())
	{
		visSphear->Draw();
	}
}

void Player::LockPlayerInput()
{
	m_lockPlayerInput = true;
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
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::ENTITY_EVENT), PacketPriority::LOW_PRIORITY);
}

void Player::SendOnMovementMessage()
{
	DirectX::XMFLOAT4A pos = this->getPosition();
	Network::ENTITY_MOVE packet(Network::ID_PLAYER_MOVE, Network::Multiplayer::GetInstance()->GetNetworkID(), pos.x, pos.y, pos.z, this->m_currentState);
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::ENTITY_MOVE), PacketPriority::LOW_PRIORITY);
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
	_onBlink();
	_onRotate(deltaTime);
	_onJump();
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
		m_moveSpeed = 2.0f;
	}
	else
	{
		m_moveSpeed = 1.0f;
	}

	if (m_kp.crouching)
	{
		m_moveSpeed = 0.5;
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
			this->Phase(10);
			m_kp.blink = true;
		}
	}
	else
	{
		m_kp.blink = false;
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
			addForceToCenter(0, 200, 0);
			m_kp.jump = true;
		}
	}


	float epsilon = 0.0002;
	if (this->getLiniearVelocity().y < epsilon && this->getLiniearVelocity().y > -epsilon)
		m_kp.jump = false;
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
	}
}

void Player::_onTeleport(double deltaTime)
{
	if (Input::Teleport())
	{
		if (!m_teleport.getActiveSphere() && m_kp.teleport == false)
		{
			m_teleport.ChargeSphere(deltaTime);
		}
		else if (m_teleport.getActiveSphere())
		{
			DirectX::XMFLOAT4A newPos = m_teleport.TeleportToSphere();
			setPosition(newPos.x, newPos.y + 0.6f, newPos.z, newPos.w);
			//If we want skill... remove
			setLiniearVelocity(0, 0, 0);
			setAwakeState(true);
		}
	}
	else if (m_teleport.getCharging())
	{
		m_teleport.ThrowSphere(getPosition(), p_camera->getDirection());
		m_teleport.setCharging(false);
		m_kp.teleport = true;
	}
	else if (!m_teleport.getActiveSphere())
		m_kp.teleport = false;
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


