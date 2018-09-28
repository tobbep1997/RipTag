#include "Player.h"
#include "../../../../InputManager/InputHandler.h"
#include "../../../../InputManager/XboxInput/GamePadHandler.h"

Player::Player()
{
	m_camera = new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);
	
	m_camera->setPosition(0, 0, 0);
}

Player::~Player()
{
	delete m_camera;
}

void Player::BeginPlay()
{

}

void Player::Update()
{
	_handleInput();
}

void Player::Draw()
{
	
}

Camera* Player::getCamera()
{
	return m_camera;
}

void Player::_handleInput()
{
	if (InputHandler::isKeyPressed('W'))
		m_camera->Translate(0.0f, 0.0f, 0.1f);
	else if (InputHandler::isKeyPressed('S'))
		m_camera->Translate(0.0f, 0.0f, -0.1f);
	if (InputHandler::isKeyPressed('A'))
		m_camera->Translate(-0.1f, 0.0f, 0.0f);
	else if (InputHandler::isKeyPressed('D'))
		m_camera->Translate(0.1f, 0.0f, 0.0f);

	if (InputHandler::isKeyPressed(InputHandler::UpArrow))
		m_camera->Rotate(-0.05f, 0.0f, 0.0f);
	else if (InputHandler::isKeyPressed(InputHandler::DownArrow))
		m_camera->Rotate(0.05f, 0.0f, 0.0f);
	if (InputHandler::isKeyPressed(InputHandler::LeftArrow))
		m_camera->Rotate(0.0f, -0.05f, 0.0f);
	else if (InputHandler::isKeyPressed(InputHandler::RightArrow))
		m_camera->Rotate(0.0f, 0.05f, 0.0f);
}
