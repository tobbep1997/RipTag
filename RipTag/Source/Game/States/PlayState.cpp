#include "PlayState.h"

PlayState::PlayState(RenderingManager * rm) : State(rm)
{
	CameraHandler::Instance();
	m_tempCam = new Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);
	CameraHandler::setActiveCamera(m_tempCam);
}

PlayState::~PlayState()
{
	delete m_tempCam;
}

void PlayState::Update()
{
	_tempCameraControls();

	m_objectHandler.Update();
	m_levelHandler.Update();
}

void PlayState::Draw()
{
	/*Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);
	camera.setPosition(0, 0, -6);*/
	//TODO::FiX
	
	//m_dynamicObjectContainer.Draw();
	//m_level.Draw();
	// Cam camera = getCam();
	//p_renderingManager->Flush(camera);
	m_objectHandler.Draw();
	m_levelHandler.Draw();

	p_renderingManager->Flush(*CameraHandler::getActiveCamera());
}

void PlayState::_tempCameraControls()
{
	if (InputHandler::isKeyPressed('W'))
		m_tempCam->Translate(0.0f, 0.0f, 0.1f);
	else if (InputHandler::isKeyPressed('S'))
		m_tempCam->Translate(0.0f, 0.0f, -0.1f);
	if (InputHandler::isKeyPressed('A'))
		m_tempCam->Translate(-0.1f, 0.0f, 0.0f);
	else if (InputHandler::isKeyPressed('D'))
		m_tempCam->Translate(0.1f, 0.0f, 0.0f);

	if (InputHandler::isKeyPressed(InputHandler::UpArrow))
		m_tempCam->Rotate(-0.05f, 0.0f, 0.0f);
	else if (InputHandler::isKeyPressed(InputHandler::DownArrow))
		m_tempCam->Rotate(0.05f, 0.0f, 0.0f);
	if (InputHandler::isKeyPressed(InputHandler::LeftArrow))
		m_tempCam->Rotate(0.0f, -0.05f, 0.0f);
	else if (InputHandler::isKeyPressed(InputHandler::RightArrow))
		m_tempCam->Rotate(0.0f, 0.05f, 0.0f);
}
