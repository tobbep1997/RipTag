#include "Player.h"
#include "InputManager/InputHandler.h"
#include "InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"
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
	ShowCursor(FALSE);
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

	float cameraOffset = 1.0f;
	DirectX::XMFLOAT4A pos = getPosition();
	pos.y += cameraOffset;
	p_camera->setPosition(pos);
	pos = p_CameraTilting(deltaTime, Input::PeekRight(), getPosition());
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

void Player::Phase(float searchLength)
{
	this->m_rayListener->shotRay(this->getBody(), p_camera->getDirection(), searchLength);
	if (this->m_rayListener->shape != nullptr)
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
	Drawable::Draw();
	if (Input::CheckVisability())
	{
		visSphear->Draw();
	}
	
}

void Player::SetCurrentVisability(const float & guard)
{
	this->m_visability = guard;
}

void Player::LockPlayerInput()
{
	m_lockPlayerInput = true;
}

void Player::UnlockPlayerInput()
{
	m_lockPlayerInput = false;
}


void Player::_handleInput(double deltaTime)
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


	/*DirectX::XMFLOAT4A camPos = p_camera->getPosition();
		if (crouchAnim.getSteps() != 1)
		{
			DirectX::XMFLOAT4A standPos = p_camera->getPosition();
			standPos.y = p_camera->getPosition().y + (this->standHeight - p_camera->getPosition().y);
			collectedPos = XMVectorAdd(collectedPos, XMLoadFloat4A(&crouchAnim.lerp(&this->standPos, &camPos, float(deltaTime * 5))));
			collectedPos = XMVectorSubtract(collectedPos, XMLoadFloat4A(&camPos));
		}*/
	float x = Input::MoveRight() * m_moveSpeed  * RIGHT.x;
	x += Input::MoveForward() * m_moveSpeed * forward.x;

	float z = Input::MoveForward() * m_moveSpeed * forward.z;
	z += Input::MoveRight() * m_moveSpeed * RIGHT.z;

	if (!unlockMouse)
	{
		int midX = InputHandler::getviewportPos().x + (InputHandler::getWindowSize().x / 2);
		int midY = InputHandler::getviewportPos().y + (InputHandler::getWindowSize().y / 2);

		DirectX::XMFLOAT2 poss = InputHandler::getMousePosition();

		float deltaX = ((InputHandler::getWindowSize().x / 2)) - poss.x;
		float deltaY = ((InputHandler::getWindowSize().y / 2)) - poss.y;
		
		SetCursorPos(midX, midY);

		if (deltaY)
			p_camera->Rotate((deltaY*-1 / 10.0f) * 1 * deltaTime, 0.0f, 0.0f);
		
		if (deltaX && !Input::PeekRight())
			p_camera->Rotate(0.0f, (deltaX*-1 / 10.0f) * 1 * deltaTime, 0.0f);
	}
	

	

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

	if (!InputHandler::isMLeftPressed(false)) //Phase acts like short range teleport through objects
	{
		isCPressed = true;
	}
	else if (isCPressed)
	{
		this->Phase(10);
		isCPressed = false;
	}
	//std::cout << x << "\n";

	setLiniearVelocity(x, getLiniearVelocity().y, z);

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

