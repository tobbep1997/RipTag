#include "Player.h"
#include "../../../../InputManager/InputHandler.h"
#include "../../../../InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"

Player::Player() : Actor(), CameraHolder(), PhysicsComponent(), PhaseAction()
{
	p_initCamera(new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 0.1f, 50.0f));
	p_camera->setPosition(0, 0, 0);
}

Player::~Player()
{	

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
		forward.y = yDir;
		b3Vec3 pos = Phase(this->getBody(), forward, 5);
		if(pos.x != 0 && pos.y != 0 && pos.z != 0)
		p_setPosition(pos.x, getPosition().y + getLiniearVelocity().y, pos.z);
		//std::cout << "Contact Point: " << this->m_rayListener->contactPoint.x << "," << this->m_rayListener->contactPoint.y << "," << this->m_rayListener->contactPoint.z << std::endl;
		//std::cout << "Normal: " << this->m_rayListener->normal.x << "," << this->m_rayListener->normal.y << "," << this->m_rayListener->normal.z << std::endl;
		//std::cout << "Ray End Position: " << x1 << ", " << getPosition().y << ", " << z1 << std::endl;
		std::cout << "foward Vec: " << forward.x << ", " << forward.y << ", " << forward.z << std::endl;
		std::cout << "new Pos: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
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
