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
	m_mouseMoved = false;
	m_currentButton = -1;
	m_liu = Mouse;
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

	if (this->m_background)
	{
		this->m_background->Release();
		delete this->m_background;
	}

	m_restart->Release();
	delete m_restart;
}

void OptionState::Update(double deltaTime)
{
	if (!InputHandler::getShowCursor())
		InputHandler::setShowCursor(TRUE);

	if (!_handleMouseInput())
	{
		_handleGamePadInput(deltaTime);
		_handleKeyboardInput(deltaTime);
	}

	if (m_currentButton != -1)
		if (m_sliderPressed || m_buttonPressed)
		{
			switch ((ButtonOrder)m_currentButton)
			{
			case SliderFov:
				if (m_liu == Mouse)
				{
					_slide();
					m_fov = (((m_buttons[m_currentButton]->getPosition().x - MIN_MAX_SLIDE.x) * ((float)MIN_MAX_FOV.y - (float)MIN_MAX_FOV.x)) / (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) + (float)MIN_MAX_FOV.x;
				}
				else
				{
					switch (m_liu)
					{
					case OptionState::Gamepad:

						break;
					case OptionState::Keyboard:
						if (InputHandler::wasKeyPressed(InputHandler::Right))
							m_fov++;
						if (InputHandler::wasKeyPressed(InputHandler::Left))
							m_fov--;

						if (m_fov < MIN_MAX_FOV.x)
							m_fov = MIN_MAX_FOV.x;
						if (m_fov > MIN_MAX_FOV.y)
							m_fov = MIN_MAX_FOV.y;
						break;
					}

					float pos = (((float)m_fov - (float)MIN_MAX_FOV.x) * (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) / ((float)MIN_MAX_FOV.y - (float)MIN_MAX_FOV.x) + MIN_MAX_SLIDE.x;
					m_buttons[ButtonOrder::SliderFov]->setPosition(pos, m_buttons[ButtonOrder::SliderFov]->getPosition().y);
				}
					m_text[ButtonOrder::SliderFov]->setString("Field of View: " + std::to_string(m_fov));
				break;
			case SliderSensitivityX:
				if (m_liu == Mouse)
				{
					_slide();
					m_sens.x = (((m_buttons[m_currentButton]->getPosition().x - MIN_MAX_SLIDE.x) * ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x)) / (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) + (float)MIN_MAX_SENSITIVITY.x;
				}
				else
				{
					switch (m_liu)
					{
					case OptionState::Gamepad:

						break;
					case OptionState::Keyboard:
						if (InputHandler::wasKeyPressed(InputHandler::Right))
							m_sens.x++;
						if (InputHandler::wasKeyPressed(InputHandler::Left))
							m_sens.x--;

						if (m_sens.x < MIN_MAX_SENSITIVITY.x)
							m_sens.x = MIN_MAX_SENSITIVITY.x;
						if (m_sens.x > MIN_MAX_SENSITIVITY.y)
							m_sens.x = MIN_MAX_SENSITIVITY.y;
						break;
					}
					float pos = (((float)m_sens.x - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE.x;
					m_buttons[ButtonOrder::SliderSensitivityX]->setPosition(pos, m_buttons[ButtonOrder::SliderSensitivityX]->getPosition().y);
				}
				m_text[ButtonOrder::SliderSensitivityX]->setString("X-Axis: " + std::to_string(m_sens.x));
				break;
			case SliderSensitivityY:
				if (m_liu == Mouse)
				{
					_slide();
					m_sens.y = (((m_buttons[m_currentButton]->getPosition().x - MIN_MAX_SLIDE.x) * ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x)) / (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) + (float)MIN_MAX_SENSITIVITY.x;
				}
				else
				{
					switch (m_liu)
					{
					case OptionState::Gamepad:
						
						break;
					case OptionState::Keyboard:
						if (InputHandler::wasKeyPressed(InputHandler::Right))
							m_sens.y++;
						if (InputHandler::wasKeyPressed(InputHandler::Left))
							m_sens.y--;

						if (m_sens.y < MIN_MAX_SENSITIVITY.x)
							m_sens.y = MIN_MAX_SENSITIVITY.x;
						if (m_sens.y > MIN_MAX_SENSITIVITY.y)
							m_sens.y = MIN_MAX_SENSITIVITY.y;
						break;
					}

					float pos = (((float)m_sens.y - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE.x;
					m_buttons[ButtonOrder::SliderSensitivityY]->setPosition(pos, m_buttons[ButtonOrder::SliderSensitivityY]->getPosition().y);
				}
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
						if (m_graphicsSelection > 3)
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
				case SoundSettings:
					this->pushNewState(new OptionSound(this->p_renderingManager));
					break;
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
	if (this->m_background)
		this->m_background->Draw();

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
	if (m_buttons[m_currentButton]->getPosition().x < MIN_MAX_SLIDE.x)
		m_buttons[m_currentButton]->setPosition(MIN_MAX_SLIDE.x, pos.y);
	else if (m_buttons[m_currentButton]->getPosition().x > MIN_MAX_SLIDE.y)
		m_buttons[m_currentButton]->setPosition(MIN_MAX_SLIDE.y, pos.y);
}

void OptionState::_initButtons()
{
	m_text.push_back(Quad::CreateButton("Field of View", 0.5f, 0.86f, 0.70f, 0.17f));
	m_text[ButtonOrder::SliderFov]->setUnpressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderFov]->setPressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderFov]->setHoverTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderFov]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderFov]->setFont(FontHandler::getFont("consolas16"));
	m_text[ButtonOrder::SliderFov]->setString("Field of View: " + std::to_string(m_fov));

	float xPos = ((((float)m_fov - (float)MIN_MAX_FOV.x) * (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) / ((float)MIN_MAX_FOV.y - (float)MIN_MAX_FOV.x) + MIN_MAX_SLIDE.x);
	 
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.79f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderFov]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderFov]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderFov]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderFov]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("X-Axis: ", 0.5f, 0.71f, 0.70f, 0.17f));
	m_text[ButtonOrder::SliderSensitivityX]->setUnpressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderSensitivityX]->setPressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderSensitivityX]->setHoverTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderSensitivityX]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderSensitivityX]->setFont(FontHandler::getFont("consolas16"));
	m_text[ButtonOrder::SliderSensitivityX]->setString("X-Axis: " + std::to_string(m_sens.x));

	xPos = (((float)m_sens.x - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE.x;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.64f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderSensitivityX]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensitivityX]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensitivityX]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensitivityX]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("Y-Axis: ", 0.5f, 0.56f, 0.70f, 0.17f));
	m_text[ButtonOrder::SliderSensitivityY]->setUnpressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderSensitivityY]->setPressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderSensitivityY]->setHoverTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderSensitivityY]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderSensitivityY]->setFont(FontHandler::getFont("consolas16"));
	m_text[ButtonOrder::SliderSensitivityY]->setString("Y-Axis: " + std::to_string(m_sens.y));

	xPos = (((float)m_sens.y - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE.x;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.49f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderSensitivityY]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensitivityY]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensitivityY]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensitivityY]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_buttons.push_back(Quad::CreateButton(SWAP_RESOLUTION[m_resSelection], 0.5f, 0.40f, 0.73f, 0.12f));
	m_buttons[ButtonOrder::ToggleResolution]->setUnpressedTexture("gui_transparent_pixel");
	m_buttons[ButtonOrder::ToggleResolution]->setPressedTexture("gui_pressed_pixel");
	m_buttons[ButtonOrder::ToggleResolution]->setHoverTexture("gui_hover_pixel");
	m_buttons[ButtonOrder::ToggleResolution]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_buttons[ButtonOrder::ToggleResolution]->setFont(FontHandler::getFont("consolas16"));

	m_buttons.push_back(Quad::CreateButton(SWAP_GRAPHICS[m_graphicsSelection], 0.5f, 0.31f, 0.73f, 0.12f));
	m_buttons[ButtonOrder::ToggleGraphics]->setUnpressedTexture("gui_transparent_pixel");
	m_buttons[ButtonOrder::ToggleGraphics]->setPressedTexture("gui_pressed_pixel");
	m_buttons[ButtonOrder::ToggleGraphics]->setHoverTexture("gui_hover_pixel");
	m_buttons[ButtonOrder::ToggleGraphics]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_buttons[ButtonOrder::ToggleGraphics]->setFont(FontHandler::getFont("consolas16"));

	std::string f = "on";
	if (!m_fullscreen)
		f = "off";

	m_buttons.push_back(Quad::CreateButton("Fullscreen: " + f, 0.5f, 0.22f, 0.73f, 0.12f));
	m_buttons[ButtonOrder::ToggleFullscreen]->setUnpressedTexture("gui_transparent_pixel");
	m_buttons[ButtonOrder::ToggleFullscreen]->setPressedTexture("gui_pressed_pixel");
	m_buttons[ButtonOrder::ToggleFullscreen]->setHoverTexture("gui_hover_pixel");
	m_buttons[ButtonOrder::ToggleFullscreen]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_buttons[ButtonOrder::ToggleFullscreen]->setFont(FontHandler::getFont("consolas16"));

	m_buttons.push_back(Quad::CreateButton("Sound Settings", 0.5075f, 0.13f, 0.365f, 0.12f));
	m_buttons[ButtonOrder::SoundSettings]->setPivotPoint(Quad::PivotPoint::centerLeft);
	m_buttons[ButtonOrder::SoundSettings]->setUnpressedTexture("gui_transparent_pixel");
	m_buttons[ButtonOrder::SoundSettings]->setPressedTexture("gui_pressed_pixel");
	m_buttons[ButtonOrder::SoundSettings]->setHoverTexture("gui_hover_pixel");
	m_buttons[ButtonOrder::SoundSettings]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_buttons[ButtonOrder::SoundSettings]->setFont(FontHandler::getFont("consolas16"));

	m_buttons.push_back(Quad::CreateButton("Save and return", 0.4915f, 0.13f, 0.365f, 0.12f));
	m_buttons[ButtonOrder::Return]->setPivotPoint(Quad::PivotPoint::centerRight);
	m_buttons[ButtonOrder::Return]->setUnpressedTexture("gui_transparent_pixel");
	m_buttons[ButtonOrder::Return]->setPressedTexture("gui_pressed_pixel");
	m_buttons[ButtonOrder::Return]->setHoverTexture("gui_hover_pixel");
	m_buttons[ButtonOrder::Return]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_buttons[ButtonOrder::Return]->setFont(FontHandler::getFont("consolas16"));

	m_restart = Quad::CreateButton("You must restart the game to apply some of the changes made", 0.5f, 0.05f, 1.2f, 0.17f);
	m_restart->setUnpressedTexture("gui_transparent_pixel");
	m_restart->setPressedTexture("gui_button_pressed_small");
	m_restart->setHoverTexture("gui_button_hover_small");
	m_restart->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_restart->setFont(FontHandler::getFont("consolas16"));

	//Background Window
	{
		this->m_background = new Quad();
		this->m_background->init();
		this->m_background->setPivotPoint(Quad::PivotPoint::center);
		this->m_background->setPosition(0.5f, 0.5f);
		this->m_background->setScale(2.0f, 2.0f);
		this->m_background->setUnpressedTexture("gui_options_menu");
		this->m_background->setPressedTexture("gui_options_menu");
		this->m_background->setHoverTexture("gui_options_menu");
	}
}

void OptionState::_handleGamePadInput(double dt)
{
	static bool pressedLastFrame = false;
	static double timer = 0.0f;
	timer += dt;

	int dir = 0;
	if (Input::isUsingGamepad())
	{
		if ((!pressedLastFrame || timer > 0.5) && (GamePadHandler::IsUpDpadPressed() || GamePadHandler::GetLeftStickYPosition() > 0.0f))
		{
			m_liu = Gamepad;
			dir = -1;
			timer = 0.0;
		}
		else if ((!pressedLastFrame || timer > 0.5) && (GamePadHandler::IsDownDpadPressed() || GamePadHandler::GetLeftStickYPosition() < 0.0f))
		{
			m_liu = Gamepad;
			dir = 1;
			timer = 0.0;
		}

		m_currentButton += dir;
		
		if (m_currentButton < 0)
			m_currentButton = (short)Return;
		else if (m_currentButton > (short)Return)
			m_currentButton = 0;

		if (m_liu == Gamepad)
			_updateSelectionStates();

		if (GamePadHandler::IsAPressed())
		{
			m_liu = Gamepad;
			if (m_buttons[m_currentButton]->isSelected())
				this->m_buttons[m_currentButton]->setState(ButtonStates::Pressed);
		}
		pressedLastFrame = GamePadHandler::IsUpDpadPressed() || GamePadHandler::GetLeftStickYPosition() > 0.0f || GamePadHandler::IsDownDpadPressed() || GamePadHandler::GetLeftStickYPosition() < 0.0f;
	}
}

void OptionState::_handleKeyboardInput(double dt)
{
	static bool pressedLastFrame = false;
	static double timer = 0.0f;
	timer += dt;

	int dir = 0;
	if ((!pressedLastFrame || timer > 0.5) && (InputHandler::isKeyPressed(InputHandler::Up)))
	{
		m_liu = Keyboard;
		dir = -1;
		timer = 0.0;
		m_sliderPressed = false;
	}
	else if ((!pressedLastFrame || timer > 0.5) && (InputHandler::isKeyPressed(InputHandler::Down)))
	{
		m_liu = Keyboard;
		dir = 1;
		timer = 0.0;
		m_sliderPressed = false;
	}

	m_currentButton += dir;

	if (m_currentButton < 0)
		m_currentButton = (short)Return;
	else if (m_currentButton > (short)Return)
		m_currentButton = 0;

	if (m_liu == Keyboard)
		_updateSelectionStates();

	if (InputHandler::wasKeyPressed(InputHandler::Return))
	{
		m_liu = Keyboard;
		if (m_buttons[m_currentButton]->isSelected())
		{
			this->m_buttons[m_currentButton]->setState(ButtonStates::Pressed);
			m_buttonPressed = true;
			if (m_currentButton == SliderFov || m_currentButton == SliderSensitivityX || m_currentButton == SliderSensitivityY)
			{
				m_sliderPressed = true;
			}
		}
	}
	pressedLastFrame = InputHandler::isKeyPressed(InputHandler::Up) || InputHandler::isKeyPressed(InputHandler::Down);
}

bool OptionState::_handleMouseInput()
{
	static DirectX::XMFLOAT2 s_mouseLastFrame = { 0,0 };
	DirectX::XMFLOAT2 mousePos = InputHandler::getMousePosition();
	DirectX::XMINT2 windowSize = InputHandler::getWindowSize();
	m_mouseMoved = false;
	if (fabs(s_mouseLastFrame.x - mousePos.x) > 0.9 || fabs(s_mouseLastFrame.y - mousePos.y) > 0.9)
	{
		m_mouseMoved = true;
		m_liu = Mouse;
	}
	s_mouseLastFrame = mousePos;

	if (m_liu == Mouse)
	{
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
	return m_mouseMoved;
}

void OptionState::_updateSelectionStates()
{
	m_buttonPressed = false;
	for (size_t i = 0; i < m_buttons.size(); i++)
	{
		if (i != m_currentButton)
		{
			m_buttons[i]->Select(false);
			m_buttons[i]->setState(ButtonStates::Normal);
		}
		else
		{
			if (!m_buttons[i]->isSelected()
				&& (m_buttons[i]->getState() != (unsigned int)ButtonStates::Pressed)
				)
			{
				m_buttons[i]->Select(true);
				m_buttons[i]->setState(ButtonStates::Hover);
			}
		}
	}
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
	WritePrivateProfileStringA("Engine", "graphics", std::to_string(m_graphicsSelection).c_str(), file.c_str());
	
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
	else if (name == "graphics")
	{
		m_graphicsSelection = key;
	}
}

void OptionState::Load()
{
	std::cout << "OptionState Load" << std::endl;
}

void OptionState::unLoad()
{
	std::cout << "OptionState unLoad" << std::endl;
}
