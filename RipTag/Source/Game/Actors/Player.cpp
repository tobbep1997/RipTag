#include "Player.h"
#include "InputManager/InputHandler.h"
#include "InputManager/XboxInput/GamePadHandler.h"
#include "EngineSource/3D Engine/RenderingManager.h"
#include <algorithm>
Player::Player() : Actor(), CameraHolder(), PhysicsComponent()
{
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
	ShowCursor(FALSE);
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



	float cameraOffset = 1.0f;
	DirectX::XMFLOAT4A pos = getPosition();
	pos.y += cameraOffset;
	p_camera->setPosition(pos);
	
	_onTilt(deltaTime, pos);

	pos.y += p_viewBobbing(deltaTime, Input::MoveForward(), m_moveSpeed);
	pos.y += p_Crouching(deltaTime, this->m_standHeight, p_camera->getPosition());
	p_camera->setPosition(pos);

	if (Input::CheckVisability())
	{
		DirectX::XMFLOAT4A po = Transform::getPosition();
		DirectX::XMVECTOR ve = DirectX::XMLoadFloat4A(&po);
		DirectX::XMVECTOR cm = DirectX::XMLoadFloat4A(&p_camera->getDirection());
		DirectX::XMStoreFloat4A(&po, DirectX::XMVectorAdd(ve, cm));

		visSphear->setPosition(po);
		visSphear->setColor(1.0f * m_visability, 1.0f * m_visability, 1.0f * m_visability, 1);
	}
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
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::ENTITY_EVENT), PacketPriority::MEDIUM_PRIORITY);
}

void Player::SendOnMovementMessage()
{
	DirectX::XMFLOAT4A pos = this->getPosition();
	Network::ENTITY_MOVE packet(Network::ID_PLAYER_MOVE, Network::Multiplayer::GetInstance()->GetNetworkID(), pos.x, pos.y, pos.z);
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::ENTITY_MOVE), PacketPriority::MEDIUM_PRIORITY);
}


void Player::RegisterThisInstanceToInput()
{
	
	InputMapping::LoadKeyMapFromFile("..\\Configuration\\KeyMapping.ini");
	using namespace std::placeholders;
	
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
	_onSprint();

	_onMovement();

	_onCrouch();

	_onRotate(deltaTime);

	_onJump();
	
	_onTeleport(deltaTime);
	
}

void Player::_onMovement()
{
	using namespace DirectX;
	XMFLOAT4A forward = p_camera->getDirection();
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

	if (Input::isUsingGamepad())
	{
		x = Input::MoveRight() * m_moveSpeed  * RIGHT.x;
		x += Input::MoveForward() * m_moveSpeed * forward.x;
		z = Input::MoveForward() * m_moveSpeed * forward.z;
		z += Input::MoveRight() * m_moveSpeed * RIGHT.z;
	}
	else
	{
		std::map<char, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "MoveRight")
				{
					x += m_moveSpeed  * RIGHT.x;
					z += m_moveSpeed * RIGHT.z;
				}
				else if (keyIterator->second == "MoveLeft")
				{
					x +=  -1.0f * m_moveSpeed * RIGHT.x;
					z += -1.0f * m_moveSpeed * RIGHT.z;
				}
				else if (keyIterator->second == "MoveForward")
				{
					x += m_moveSpeed * forward.x;
					z += m_moveSpeed * forward.z;
				}
				else if (keyIterator->second == "MoveBackward")
				{
					x += -1.0f * m_moveSpeed * forward.x;
					z += -1.0f * m_moveSpeed * forward.z;
				}
			}
		}
	}
	setLiniearVelocity(x, getLiniearVelocity().y, z);
}

void Player::_onSprint()
{
	if (GamePadHandler::IsLeftStickPressed())
	{
		m_moveSpeed = 1.0f;
	}
	else
	{
		m_moveSpeed = 1.0f;
	}

	if (crouching)
	{
		m_moveSpeed = 0.5;
	}
}

void Player::_onCrouch()
{
	if (Input::Crouch())
	{
		if (crouching == false)
		{
			m_standHeight = this->p_camera->getPosition().y;
			this->CreateBox(0.5, 0.10, 0.5);
			this->setPosition(this->getPosition().x, this->getPosition().y - 0.4, this->getPosition().z, 1);
			crouching = true;
		}
	}
	else
	{
		if (crouching)
		{
			m_standHeight = this->p_camera->getPosition().y;
			this->CreateBox(0.5, 0.5, 0.5);
			this->setPosition(this->getPosition().x, this->getPosition().y + 0.4, this->getPosition().z, 1);
			crouching = false;
		}
	}
}

void Player::_onRotate(double deltaTime)
{
	if (!unlockMouse)
	{
		int midX = InputHandler::getviewportPos().x + (InputHandler::getWindowSize().x / 2);
		int midY = InputHandler::getviewportPos().y + (InputHandler::getWindowSize().y / 2);

		DirectX::XMFLOAT2 poss = InputHandler::getMousePosition();

		float deltaX = ((InputHandler::getWindowSize().x / 2)) - poss.x;
		float deltaY = ((InputHandler::getWindowSize().y / 2)) - poss.y;

		SetCursorPos(midX, midY);

		if (deltaX && !Input::PeekRight())
			p_camera->Rotate(0.0f, (deltaX*-1 / 10.0f) * 1 * deltaTime, 0.0f);
		if (deltaY)
			p_camera->Rotate((deltaY*-1 / 10.0f) * 1 * deltaTime, 0.0f, 0.0f);

	}
}

void Player::_onJump()
{
	if (Input::Jump())
	{
		if (isPressed == false)
		{
			addForceToCenter(0, 500, 0);
			isPressed = true;
		}
	}
	else
	{
		isPressed = false;
	}
}

void Player::_onTeleport(double deltaTime)
{
	if (InputHandler::isKeyPressed('T'))
	{
		if (!m_teleport.getActiveSphere())
		{
			m_teleport.ChargeSphere(deltaTime);
		}
	}
	else if (m_teleport.getCharging())
	{
		m_teleport.ThrowSphere(getPosition(), p_camera->getDirection());
		m_teleport.setCharging(false);
	}
	if (InputHandler::isKeyPressed('G'))
	{
		if (m_teleport.getActiveSphere())
		{
			DirectX::XMFLOAT4A newPos = m_teleport.TeleportToSphere();
			setPosition(newPos.x, newPos.y + 0.6f, newPos.z, newPos.w);
			//If we want skill... remove
			setLiniearVelocity(0, 0, 0);
			setAwakeState(true);
		}
	}
}

void Player::_onTilt(double deltaTime, DirectX::XMFLOAT4A & referencePos)
{
	if (Input::isUsingGamepad())
		referencePos = p_CameraTilting(deltaTime, Input::PeekRight(), getPosition());
	else
	{
		std::map<char, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "TiltRight")
				{
					referencePos = p_CameraTilting(deltaTime, -1, getPosition());
				}
				else if (keyIterator->second == "TiltLeft")
				{
					referencePos = p_CameraTilting(deltaTime, 1, getPosition());
				}
			}
		}
	}
}

