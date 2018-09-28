#include "Player.h"
#include "../../../../InputManager/InputHandler.h"
#include "../../../../InputManager/XboxInput/GamePadHandler.h"

Player::Player() : Actor()
{
	m_camera = new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f, 0.1f, 50.0f);
	
	m_camera->setPosition(0, 0, 0);
}

Player::~Player()
{
	delete m_camera;
}

void Player::BeginPlay()
{

}

void Player::Update(double deltaTime)
{
	_handleInput(deltaTime);
	
}


Camera* Player::getCamera()
{
	return m_camera;
}

void Player::_handleInput(double deltaTime)
{
	if (InputHandler::isKeyPressed('W'))
		m_camera->Translate(0.0f, 0.0f, m_moveSpeed * deltaTime);
	else if (InputHandler::isKeyPressed('S'))
		m_camera->Translate(0.0f, 0.0f, -m_moveSpeed * deltaTime);
	if (InputHandler::isKeyPressed('A'))
		m_camera->Translate(-m_moveSpeed * deltaTime, 0.0f, 0.0f);
	else if (InputHandler::isKeyPressed('D'))
		m_camera->Translate(m_moveSpeed * deltaTime, 0.0f, 0.0f);

	if (InputHandler::isKeyPressed(InputHandler::UpArrow))
		m_camera->Rotate(-m_cameraSpeed * deltaTime, 0.0f, 0.0f);
	else if (InputHandler::isKeyPressed(InputHandler::DownArrow))
		m_camera->Rotate(m_cameraSpeed * deltaTime, 0.0f, 0.0f);
	if (InputHandler::isKeyPressed(InputHandler::LeftArrow))
		m_camera->Rotate(0.0f, -m_cameraSpeed * deltaTime, 0.0f);
	else if (InputHandler::isKeyPressed(InputHandler::RightArrow))
		m_camera->Rotate(0.0f, m_cameraSpeed * deltaTime, 0.0f);

	setPosition(m_camera->getPosition());
}
