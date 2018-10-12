#include "Player.h"
#include "../../../../InputManager/InputHandler.h"
#include "../../../../InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"

Player::Player() : Actor(), CameraHolder(), PhysicsComponent()
{
	p_initCamera(new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 0.1f, 50.0f));
	p_camera->setPosition(0, 0, 0);
	this->m_rayListener = new RayCastListener();
}

Player::~Player()
{	
	delete this->m_rayListener;
}

void Player::BeginPlay()
{

}

void Player::Update(double deltaTime)
{
	_handleInput(deltaTime);

	p_camera->setPosition(getPosition());
}

void Player::PhysicsUpdate(double deltaTime)
{
	p_updatePhysics(this);
}

void Player::setPosition(const float& x, const float& y, const float& z, const float& w)
{
	Transform::setPosition(x, y, z);
	PhysicsComponent::p_setPosition(x, y, z);
}

void Player::Phase()
{
	this->m_rayListener->shotRay(this->getBody(), p_camera->getDirection(), 2);
	b3Vec3 pos = b3Vec3(0, 0, 0);
	if (this->m_rayListener->shape != nullptr)
	{
		pos.x = this->m_rayListener->contactPoint.x + (0.5*(-this->m_rayListener->normal.x));
		pos.y = this->m_rayListener->contactPoint.y + (0.5*(-this->m_rayListener->normal.y));
		pos.z = this->m_rayListener->contactPoint.z + (0.5*(-this->m_rayListener->normal.z));
	}
	this->m_rayListener->clear();
	if (!(pos.x == 0 && pos.z == 0))
		p_setPosition(pos.x, this->getPosition().y, pos.z);
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
	XMVECTOR vUP= XMLoadFloat4(&UP);
	XMVECTOR vRight;

	vRight = XMVector3Normalize(XMVector3Cross(vUP, vForward));
	vForward = XMVector3Normalize(XMVector3Cross(vRight, vUP));



	XMStoreFloat4A(&forward, vForward);
	XMStoreFloat4(&RIGHT, vRight);


	float x = Input::MoveRight() * m_moveSpeed * deltaTime * RIGHT.x;
	x += Input::MoveForward() * m_moveSpeed * deltaTime * forward.x;


	float z = Input::MoveForward() * m_moveSpeed * deltaTime * forward.z;
	z += Input::MoveRight() * m_moveSpeed * deltaTime * RIGHT.z;


	p_camera->Rotate((Input::TurnUp()*-1) * 5 * deltaTime, 0.0f, 0.0f);
	
	p_camera->Rotate(0.0f, Input::TurnRight() * 5 * deltaTime, 0.0f);

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


	if (!InputHandler::isKeyPressed('Q')) //Phase acts like short range teleport through objects
	{
		isQPressed = true;
	}
	else if (isQPressed)
	{
		this->Phase();
		isQPressed = false;
	}

	setLiniearVelocity(x, getLiniearVelocity().y, z);


	if (InputHandler::isKeyPressed('Y'))
	{
		if (isPressed2 == false)
		{
			CreateBox(2, 2, 2);
			isPressed2 = true;
		}
	}
	else
	{
		isPressed2 = false;
	}
	//addForceToCenter(x, getLiniearVelocity().y, z);

	//std::cout << "Y: " << getLiniearVelocity().y << std::endl;
	//p_setPosition(getPosition().x + x, getPosition().y, getPosition().z + z);
	//setPosition(p_camera->getPosition());
}
