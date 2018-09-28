#include "Player.h"
#include "../../../../InputManager/InputHandler.h"
#include "../../../../InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"

Player::Player() : Actor(), CameraHolder()
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
	
}


void Player::_handleInput(double deltaTime)
{
	
	p_camera->Translate(0.0f, 0.0f, Input::MoveForward() * m_moveSpeed * deltaTime);
	/*if (InputHandler::isKeyPressed('W'))
		p_camera->Translate(0.0f, 0.0f, m_moveSpeed * deltaTime);*/
	/*else if (InputHandler::isKeyPressed('S'))
		p_camera->Translate(0.0f, 0.0f, -m_moveSpeed * deltaTime);*/
	if (InputHandler::isKeyPressed('A'))
		p_camera->Translate(-m_moveSpeed * deltaTime, 0.0f, 0.0f);
	else if (InputHandler::isKeyPressed('D'))
		p_camera->Translate(m_moveSpeed * deltaTime, 0.0f, 0.0f);

	if (InputHandler::isKeyPressed(InputHandler::UpArrow))
		p_camera->Rotate(-m_cameraSpeed * deltaTime, 0.0f, 0.0f);
	else if (InputHandler::isKeyPressed(InputHandler::DownArrow))
		p_camera->Rotate(m_cameraSpeed * deltaTime, 0.0f, 0.0f);
	if (InputHandler::isKeyPressed(InputHandler::LeftArrow))
		p_camera->Rotate(0.0f, -m_cameraSpeed * deltaTime, 0.0f);
	else if (InputHandler::isKeyPressed(InputHandler::RightArrow))
		p_camera->Rotate(0.0f, m_cameraSpeed * deltaTime, 0.0f);

	setPosition(p_camera->getPosition());
}
