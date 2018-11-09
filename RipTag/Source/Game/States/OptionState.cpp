#include "RipTagPCH.h"
#include "OptionState.h"

OptionState::OptionState(RenderingManager * rm) : State(rm)
{
	m_fov = 90;
	m_sens.x = 5;
	m_sens.y = 5;
	m_resSelection = 0;
	m_graphicsSelection = 0;
	m_sliderPressed = false;
	m_fullscreen = false;
	m_buttonPressed = false;
	m_drawMustRestart = false;
	m_currentButton = -1;
	_ReadSettingsFromFile();
	_initButtons();
}

OptionState::~OptionState()
{
	for (auto & t : m_text)
	{
		t->Release();
		delete t;
	}
	m_text.clear();
	for (auto & b : m_buttons)
	{
		b->Release();
		delete b;
	}
	m_buttons.clear();
	m_restart->Release();
	delete m_restart;
}

void OptionState::Update(double deltaTime)
{
	if (!InputHandler::getShowCursor())
		InputHandler::setShowCursor(TRUE);

	_handleMouseInput();
	_handleGamePadInput();
	_handleKeyboardInput();

	if (m_currentButton != -1)
		if (m_sliderPressed || m_buttonPressed)
		{
			switch ((ButtonOrder)m_currentButton)
			{
			case SliderFov:
				_slide();
				m_fov = (((m_buttons[m_currentButton]->getPosition().x - 0.3) * ((float)MIN_MAX_FOV.y - (float)MIN_MAX_FOV.x)) / (0.7f - 0.3f)) + (float)MIN_MAX_FOV.x;
				m_text[ButtonOrder::SliderFov]->setString("Field of View: " + std::to_string(m_fov));
				break;
			case SliderSensitivityX:
				_slide();
				m_sens.x = (((m_buttons[m_currentButton]->getPosition().x - 0.3) * ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x)) / (0.7f - 0.3f)) + (float)MIN_MAX_SENSITIVITY.x;
				m_text[ButtonOrder::SliderSensitivityX]->setString("X-Axis: " + std::to_string(m_sens.x));
				break;
			case SliderSensitivityY:
				_slide();
				m_sens.y = (((m_buttons[m_currentButton]->getPosition().x - 0.3) * ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x)) / (0.7f - 0.3f)) + (float)MIN_MAX_SENSITIVITY.x;
				m_text[ButtonOrder::SliderSensitivityY]->setString("Y-Axis: " + std::to_string(m_sens.y));
				break;
				case ToggleResolution:
					if (m_buttonPressed)
					{
						m_resSelection++;
						if (m_resSelection > 2)
							m_resSelection = 0;
						m_buttons[m_currentButton]->setString(SWAP_RESOLUTION[m_resSelection]);
						m_drawMustRestart = true;
					}
					break;
				case ToggleGraphics:
					if (m_buttonPressed)
					{
						m_graphicsSelection++;
						if (m_graphicsSelection > 2)
							m_graphicsSelection = 0;
						m_buttons[m_currentButton]->setString(SWAP_GRAPHICS[m_graphicsSelection]);
						m_drawMustRestart = true;
					}
					break;
				case ToggleFullscreen:
				{
					if (m_buttonPressed)
					{
						m_fullscreen = !m_fullscreen;
						std::string f = "on";
						if (!m_fullscreen)
							f = "off";
						m_buttons[m_currentButton]->setString("Fullscreen: " + f);
						m_drawMustRestart = true;
					}
					break;
				}
				case Return:
					_WriteSettingsToFile();
					p_renderingManager->Reset();
					Input::ReadSettingsFile();
					this->setKillState(true);
					break;
			}
		}
}

void OptionState::Draw()
{
	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);
	camera.setPosition(0, 0, -10);

	for (auto & t : m_text)
		t->Draw();
	for (auto & b : m_buttons)
		b->Draw();
	if (m_drawMustRestart)
		m_restart->Draw();
	p_renderingManager->Flush(camera);
}

void OptionState::_slide()
{
	DirectX::XMFLOAT2 mp = InputHandler::getMousePosition();
	mp.x /= InputHandler::getWindowSize().x;
	mp.x *= InputHandler::getViewportSize().x;

	DirectX::XMFLOAT2A pos = m_buttons[m_currentButton]->getPosition();
	m_buttons[m_currentButton]->setPosition(mp.x / InputHandler::getViewportSize().x, pos.y);
	if (m_buttons[m_currentButton]->getPosition().x < 0.3f)
		m_buttons[m_currentButton]->setPosition(0.3f, pos.y);
	else if(m_buttons[m_currentButton]->getPosition().x > 0.7f)
		m_buttons[m_currentButton]->setPosition(0.7f, pos.y);
}

void OptionState::_initButtons()
{
	m_text.push_back(Quad::CreateButton("Field of View", 0.5f, 0.95f, 0.70f, 0.17f));
	m_text[ButtonOrder::SliderFov]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_text[ButtonOrder::SliderFov]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	m_text[ButtonOrder::SliderFov]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	m_text[ButtonOrder::SliderFov]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderFov]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas16.spritefont"));
	m_text[ButtonOrder::SliderFov]->setString("Field of View: " + std::to_string(m_fov));

	float xPos = ((((float)m_fov - (float)MIN_MAX_FOV.x) * (0.7 - 0.3)) / ((float)MIN_MAX_FOV.y - (float)MIN_MAX_FOV.x) + 0.3);
	 
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.88f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderFov]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_buttons[ButtonOrder::SliderFov]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	m_buttons[ButtonOrder::SliderFov]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	m_buttons[ButtonOrder::SliderFov]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("X-Axis: ", 0.5f, 0.81f, 0.70f, 0.17f));
	m_text[ButtonOrder::SliderSensitivityX]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_text[ButtonOrder::SliderSensitivityX]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	m_text[ButtonOrder::SliderSensitivityX]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	m_text[ButtonOrder::SliderSensitivityX]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderSensitivityX]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas16.spritefont"));
	m_text[ButtonOrder::SliderSensitivityX]->setString("X-Axis: " + std::to_string(m_sens.x));

	xPos = (((float)m_sens.x - (float)MIN_MAX_SENSITIVITY.x) * (0.7 - 0.3)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + 0.3;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.74f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderSensitivityX]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_buttons[ButtonOrder::SliderSensitivityX]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	m_buttons[ButtonOrder::SliderSensitivityX]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	m_buttons[ButtonOrder::SliderSensitivityX]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("Y-Axis: ", 0.5f, 0.67f, 0.70f, 0.17f));
	m_text[ButtonOrder::SliderSensitivityY]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_text[ButtonOrder::SliderSensitivityY]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	m_text[ButtonOrder::SliderSensitivityY]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	m_text[ButtonOrder::SliderSensitivityY]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderSensitivityY]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas16.spritefont"));
	m_text[ButtonOrder::SliderSensitivityY]->setString("Y-Axis: " + std::to_string(m_sens.y));

	xPos = (((float)m_sens.y - (float)MIN_MAX_SENSITIVITY.x) * (0.7 - 0.3)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + 0.3;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.60f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderSensitivityY]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_buttons[ButtonOrder::SliderSensitivityY]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	m_buttons[ButtonOrder::SliderSensitivityY]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	m_buttons[ButtonOrder::SliderSensitivityY]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_buttons.push_back(Quad::CreateButton(SWAP_RESOLUTION[m_resSelection], 0.5f, 0.50f, 0.70f, 0.17f));
	m_buttons[ButtonOrder::ToggleResolution]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_buttons[ButtonOrder::ToggleResolution]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	m_buttons[ButtonOrder::ToggleResolution]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	m_buttons[ButtonOrder::ToggleResolution]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_buttons[ButtonOrder::ToggleResolution]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas16.spritefont"));

	m_buttons.push_back(Quad::CreateButton(SWAP_GRAPHICS[m_graphicsSelection], 0.5f, 0.40f, 0.70f, 0.17f));
	m_buttons[ButtonOrder::ToggleGraphics]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_buttons[ButtonOrder::ToggleGraphics]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	m_buttons[ButtonOrder::ToggleGraphics]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	m_buttons[ButtonOrder::ToggleGraphics]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_buttons[ButtonOrder::ToggleGraphics]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas16.spritefont"));

	std::string f = "on";
	if (!m_fullscreen)
		f = "off";

	m_buttons.push_back(Quad::CreateButton("Fullscreen: " + f, 0.5f, 0.30f, 0.70f, 0.17f));
	m_buttons[ButtonOrder::ToggleFullscreen]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_buttons[ButtonOrder::ToggleFullscreen]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	m_buttons[ButtonOrder::ToggleFullscreen]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	m_buttons[ButtonOrder::ToggleFullscreen]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_buttons[ButtonOrder::ToggleFullscreen]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas16.spritefont"));

	m_buttons.push_back(Quad::CreateButton("Save and return", 0.5f, 0.20f, 0.70f, 0.17f));
	m_buttons[ButtonOrder::Return]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_buttons[ButtonOrder::Return]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	m_buttons[ButtonOrder::Return]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	m_buttons[ButtonOrder::Return]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_buttons[ButtonOrder::Return]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas16.spritefont"));

	m_restart = Quad::CreateButton("You must restart the game to apply some of the changes made", 0.5f, 0.1f, 1.2f, 0.17f);
	m_restart->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_restart->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	m_restart->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	m_restart->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_restart->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas16.spritefont"));
}

void OptionState::_handleGamePadInput()
{
}

void OptionState::_handleKeyboardInput()
{
}

void OptionState::_handleMouseInput()
{
	DirectX::XMFLOAT2 mousePos = InputHandler::getMousePosition();
	DirectX::XMINT2 windowSize = InputHandler::getWindowSize();

	mousePos.x /= windowSize.x;
	mousePos.y /= windowSize.y;
	m_buttonPressed = false;

	if (!InputHandler::isMLeftPressed(true))
		m_sliderPressed = false;

	for (size_t i = 0; i < m_buttons.size() && !m_sliderPressed; i++)
	{
		//set this button to current and on hover state
		if (m_buttons[i]->isReleased(mousePos))
		{
			m_buttonPressed = true;
			break;
		}
		else if (m_buttons[i]->isPressed(mousePos))
		{
			m_currentButton = i;
			OptionState::ButtonOrder type = (OptionState::ButtonOrder)i;
			if (type == SliderFov || type == SliderSensitivityX || type == SliderSensitivityY)
			{
				m_sliderPressed = true;
				m_buttons[i]->Select(true);
			}
			break;
		}
		else
			m_buttons[i]->Select(false);
	}
}

void OptionState::_updateSelectionStates()
{
}

void OptionState::_WriteSettingsToFile()
{
	// Player settings
	std::string file = "../Configuration/PlayerMapping.ini";
	WritePrivateProfileStringA("Player", "XAXIS", std::to_string(m_sens.x).c_str(), file.c_str());
	WritePrivateProfileStringA("Player", "YAXIS", std::to_string(m_sens.y).c_str(), file.c_str());
	WritePrivateProfileStringA("Player", "PlayerFOV", std::to_string(m_fov).c_str(), file.c_str());

	file = "../Configuration/defultEngineSettings.ini";
	WritePrivateProfileStringA("Engine", "fullscreen", std::to_string(m_fullscreen).c_str(), file.c_str());
	WritePrivateProfileStringA("Engine", "width", std::to_string(RES[m_resSelection].x).c_str(), file.c_str());
	WritePrivateProfileStringA("Engine", "height", std::to_string(RES[m_resSelection].y).c_str(), file.c_str());
	
}

void OptionState::_ReadSettingsFromFile()
{
	std::string file[2] = { "../Configuration/PlayerMapping.ini", "../Configuration/defultEngineSettings.ini"};
	std::string names[2] = { "Player", "Engine" };
	for (int k = 0; k < 2; k++)
	{
		const int bufferSize = 1024;
		char buffer[bufferSize];

		//Load in Keyboard section
		if (GetPrivateProfileStringA(names[k].c_str(), NULL, NULL, buffer, bufferSize, file[k].c_str()))
		{
			std::vector<std::string> nameList;
			std::istringstream nameStream;
			nameStream.str(std::string(buffer, bufferSize));

			std::string name = "";
			while (std::getline(nameStream, name, '\0'))
			{
				if (name == "")
					break;
				nameList.push_back(name);
			}

			for (size_t i = 0; i < nameList.size(); i++)
			{
				int key = -1;
				key = GetPrivateProfileIntA(names[k].c_str(), nameList[i].c_str(), -1, file[k].c_str());
				if (key != -1)
				{
					_ParseFileInputInt(nameList[i], key);
				}

			}
			//Clear buffer for reuse
			ZeroMemory(buffer, bufferSize);

		}
		else
			std::cout << GetLastError() << std::endl;
	}
	
}

void OptionState::_ParseFileInputInt(const std::string & name, int key)
{
	if (name == "XAXIS")
	{
		m_sens.x = key;
	}
	else if (name == "YAXIS")
	{
		m_sens.y = key;
	}
	else if (name == "PlayerFOV")
	{
		m_fov = key;
	}
	else if (name == "fullscreen")
	{
		m_fullscreen = key;
	}
	else if (name == "height")
	{
		if (key == 720)
			m_resSelection = 0;
		else if (key == 1080)
			m_resSelection = 1;
		else
			m_resSelection = 2;
	}
}
