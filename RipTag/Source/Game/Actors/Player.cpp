#include "Player.h"
#include "../../../../InputManager/InputHandler.h"
#include "../../../../InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"

Player::Player() : Actor(), CameraHolder(), PhysicsComponent()
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

	//p_position.x += pos.x * right.x;
	//p_position.y += pos.x * right.y;
	//p_position.z += pos.x * right.z;

	//p_position.x += pos.y * m_UP.x;
	//p_position.y += pos.y * m_UP.y;
	//p_position.z += pos.y * m_UP.z;

	//p_position.x += pos.z * m_direction.x;
	//p_position.y += pos.z * m_direction.y;
	//p_position.z += pos.z * m_direction.z;

	//p_camera->Translate(0.0f, 0.0f, Input::MoveForward() * m_moveSpeed * deltaTime);
	/*if (InputHandler::isKeyPressed('W'))
		p_camera->Translate(0.0f, 0.0f, m_moveSpeed * deltaTime);*/
	/*else if (InputHandler::isKeyPressed('S'))
		p_camera->Translate(0.0f, 0.0f, -m_moveSpeed * deltaTime);*/
	//p_camera->Translate(Input::MoveRight() * m_moveSpeed * deltaTime, 0.0f, 0.0f);
	float x = Input::MoveRight() * m_moveSpeed * deltaTime * RIGHT.x;
	x += Input::MoveForward() * m_moveSpeed * deltaTime * forward.x;


	float z = Input::MoveForward() * m_moveSpeed * deltaTime * forward.z;
	z += Input::MoveRight() * m_moveSpeed * deltaTime * RIGHT.z;


	p_camera->Rotate(Input::TurnUp() * (m_moveSpeed-95) * deltaTime, 0.0f, 0.0f);
	
	p_camera->Rotate(0.0f, Input::TurnRight() * (m_moveSpeed-95) * deltaTime, 0.0f);

	if (InputHandler::isKeyPressed(InputHandler::Key::SPACEBAR))
	{
		if (isPressed == false)
		{
			addForceToCenter(0, 1000, 0);
			isPressed = true;
		}
	}
	else
	{
		isPressed = false;
	}

	setLiniearVelocity(x, getLiniearVelocity().y, z);
	//addForceToCenter(x, getLiniearVelocity().y, z);

	//std::cout << "Y: " << getLiniearVelocity().y << std::endl;
	//p_setPosition(getPosition().x + x, getPosition().y, getPosition().z + z);
	//setPosition(p_camera->getPosition());
}
