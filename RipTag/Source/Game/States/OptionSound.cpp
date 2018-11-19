#include "RipTagPCH.h"
#include "OptionSound.h"

OptionSound::OptionSound(RenderingManager * rm) : State(rm)
{
	m_liu = Mouse;
	_ReadSettingsFromFile();
	_initButtons();
}

OptionSound::~OptionSound()
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

	//m_restart->Release();
	//delete m_restart;
}

void OptionSound::Update(double deltaTime)
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
			case SliderMaster:
				if (m_liu == Mouse)
				{
					_slide();
					m_master = (((m_buttons[m_currentButton]->getPosition().x - MIN_MAX_SLIDE.x) * ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x)) / (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) + (float)MIN_MAX_SOUND.x;
				}
				else
				{
					switch (m_liu)
					{
					case OptionSound::Gamepad:

						break;
					case OptionSound::Keyboard:
						if (InputHandler::wasKeyPressed(InputHandler::Right))
							m_master++;
						if (InputHandler::wasKeyPressed(InputHandler::Left))
							m_master--;

						if (m_master < MIN_MAX_SOUND.x)
							m_master = MIN_MAX_SOUND.x;
						if (m_master > MIN_MAX_SOUND.y)
							m_master = MIN_MAX_SOUND.y;
						break;
					}

					float pos = (((float)m_master - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE.x;
					m_buttons[ButtonOrder::SliderMaster]->setPosition(pos, m_buttons[ButtonOrder::SliderMaster]->getPosition().y);
				}
				m_text[ButtonOrder::SliderMaster]->setString("Master Volume: " + std::to_string(m_master));
				AudioEngine::SetMasterVolume((float)m_master / 100.0f);
				break;
			case SliderEffects:
				if (m_liu == Mouse)
				{
					_slide();
					m_effects = (((m_buttons[m_currentButton]->getPosition().x - MIN_MAX_SLIDE.x) * ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x)) / (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) + (float)MIN_MAX_SOUND.x;
				}
				else
				{
					switch (m_liu)
					{
					case OptionSound::Gamepad:

						break;
					case OptionSound::Keyboard:
						if (InputHandler::wasKeyPressed(InputHandler::Right))
							m_effects++;
						if (InputHandler::wasKeyPressed(InputHandler::Left))
							m_effects--;

						if (m_effects < MIN_MAX_SOUND.x)
							m_effects = MIN_MAX_SOUND.x;
						if (m_effects > MIN_MAX_SOUND.y)
							m_effects = MIN_MAX_SOUND.y;
						break;
					}
					float pos = (((float)m_effects - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE.x;
					m_buttons[ButtonOrder::SliderEffects]->setPosition(pos, m_buttons[ButtonOrder::SliderEffects]->getPosition().y);
				}
				m_text[ButtonOrder::SliderEffects]->setString("Effects Volume: " + std::to_string(m_effects));
				AudioEngine::SetEffectVolume((float)m_effects / 100.0f);
				break;
			case SliderAmbient:
				if (m_liu == Mouse)
				{
					_slide();
					m_ambient = (((m_buttons[m_currentButton]->getPosition().x - MIN_MAX_SLIDE.x) * ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x)) / (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) + (float)MIN_MAX_SOUND.x;
				}
				else
				{
					switch (m_liu)
					{
					case OptionSound::Gamepad:

						break;
					case OptionSound::Keyboard:
						if (InputHandler::wasKeyPressed(InputHandler::Right))
							m_ambient++;
						if (InputHandler::wasKeyPressed(InputHandler::Left))
							m_ambient--;

						if (m_ambient < MIN_MAX_SOUND.x)
							m_ambient = MIN_MAX_SOUND.x;
						if (m_ambient > MIN_MAX_SOUND.y)
							m_ambient = MIN_MAX_SOUND.y;
						break;
					}

					float pos = (((float)m_ambient - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE.x;
					m_buttons[ButtonOrder::SliderAmbient]->setPosition(pos, m_buttons[ButtonOrder::SliderAmbient]->getPosition().y);
				}
				m_text[ButtonOrder::SliderAmbient]->setString("Ambient Volume: " + std::to_string(m_ambient));
				AudioEngine::SetAmbientVolume((float)m_ambient / 100.0f);
				break;
			case SliderMusic:
				if (m_liu == Mouse)
				{
					_slide();
					m_music = (((m_buttons[m_currentButton]->getPosition().x - MIN_MAX_SLIDE.x) * ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x)) / (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) + (float)MIN_MAX_SOUND.x;
				}
				else
				{
					switch (m_liu)
					{
					case OptionSound::Gamepad:

						break;
					case OptionSound::Keyboard:
						if (InputHandler::wasKeyPressed(InputHandler::Right))
							m_music++;
						if (InputHandler::wasKeyPressed(InputHandler::Left))
							m_music--;

						if (m_music < MIN_MAX_SOUND.x)
							m_music = MIN_MAX_SOUND.x;
						if (m_music > MIN_MAX_SOUND.y)
							m_music = MIN_MAX_SOUND.y;
						break;
					}

					float pos = (((float)m_music - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE.x;
					m_buttons[ButtonOrder::SliderMusic]->setPosition(pos, m_buttons[ButtonOrder::SliderMusic]->getPosition().y);
				}
				m_text[ButtonOrder::SliderMusic]->setString("Music Volume: " + std::to_string(m_music));
				AudioEngine::SetMusicVolume((float)m_music / 100.0f);
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

void OptionSound::Draw()
{
	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);
	camera.setPosition(0, 0, -10);
	if (this->m_background)
		this->m_background->Draw();

	for (auto & t : m_text)
		t->Draw();
	for (auto & b : m_buttons)
		b->Draw();
	p_renderingManager->Flush(camera);
}

void OptionSound::_slide()
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

void OptionSound::_initButtons()
{
	m_text.push_back(Quad::CreateButton("Master", 0.5f, 0.86f, 0.70f, 0.17f));
	m_text[ButtonOrder::SliderMaster]->setUnpressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderMaster]->setPressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderMaster]->setHoverTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderMaster]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderMaster]->setFont(FontHandler::getFont("consolas16"));
	m_text[ButtonOrder::SliderMaster]->setString("Master Volume: " + std::to_string(m_master));

	float xPos = ((((float)m_master - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE.x);

	m_buttons.push_back(Quad::CreateButton("", xPos, 0.79f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderMaster]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderMaster]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderMaster]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderMaster]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("Effects", 0.5f, 0.71f, 0.70f, 0.17f));
	m_text[ButtonOrder::SliderEffects]->setUnpressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderEffects]->setPressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderEffects]->setHoverTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderEffects]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderEffects]->setFont(FontHandler::getFont("consolas16"));
	m_text[ButtonOrder::SliderEffects]->setString("Effects Volume: " + std::to_string(m_effects));

	xPos = (((float)m_effects - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE.x;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.64f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderEffects]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderEffects]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderEffects]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderEffects]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("Ambient: ", 0.5f, 0.56f, 0.70f, 0.17f));
	m_text[ButtonOrder::SliderAmbient]->setUnpressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderAmbient]->setPressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderAmbient]->setHoverTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderAmbient]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderAmbient]->setFont(FontHandler::getFont("consolas16"));
	m_text[ButtonOrder::SliderAmbient]->setString("Ambient Volume: " + std::to_string(m_ambient));

	xPos = (((float)m_ambient - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE.x;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.49f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderAmbient]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderAmbient]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderAmbient]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderAmbient]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("Music: ", 0.5f, 0.41f, 0.70f, 0.17f));
	m_text[ButtonOrder::SliderMusic]->setUnpressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderMusic]->setPressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderMusic]->setHoverTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderMusic]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderMusic]->setFont(FontHandler::getFont("consolas16"));
	m_text[ButtonOrder::SliderMusic]->setString("Music Volume: " + std::to_string(m_music));

	xPos = (((float)m_music - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE.y - MIN_MAX_SLIDE.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE.x;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.34f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderMusic]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderMusic]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderMusic]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderMusic]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_buttons.push_back(Quad::CreateButton("Save and return", 0.5f, 0.13f, 0.73f, 0.12f));
	m_buttons[ButtonOrder::Return]->setUnpressedTexture("gui_transparent_pixel");
	m_buttons[ButtonOrder::Return]->setPressedTexture("gui_pressed_pixel");
	m_buttons[ButtonOrder::Return]->setHoverTexture("gui_hover_pixel");
	m_buttons[ButtonOrder::Return]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_buttons[ButtonOrder::Return]->setFont(FontHandler::getFont("consolas16"));

	//Background Window
	{
		this->m_background = new Quad();
		this->m_background->init();
		this->m_background->setPivotPoint(Quad::PivotPoint::center);
		this->m_background->setPosition(0.5f, 0.5f);
		this->m_background->setScale(2.0f, 2.0f);
		this->m_background->setUnpressedTexture("gui_options_menu_sound");
		this->m_background->setPressedTexture("gui_options_menu_sound");
		this->m_background->setHoverTexture("gui_options_menu_sound");
	}
}

void OptionSound::_handleGamePadInput(double dt)
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

void OptionSound::_handleKeyboardInput(double dt)
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
			if (m_currentButton =! Return)
			{
				m_sliderPressed = true;
			}
		}
	}
	pressedLastFrame = InputHandler::isKeyPressed(InputHandler::Up) || InputHandler::isKeyPressed(InputHandler::Down);
}

bool OptionSound::_handleMouseInput()
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
				OptionSound::ButtonOrder type = (OptionSound::ButtonOrder)i;
				if (type != Return)
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

void OptionSound::_updateSelectionStates()
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

void OptionSound::_WriteSettingsToFile()
{
	// Player settings
	std::string file = "../Configuration/AudioSettings.ini";
	WritePrivateProfileStringA("Audio", "Master", std::to_string(m_master).c_str(), file.c_str());
	WritePrivateProfileStringA("Audio", "Effects", std::to_string(m_effects).c_str(), file.c_str());
	WritePrivateProfileStringA("Audio", "Ambient", std::to_string(m_ambient).c_str(), file.c_str());
	WritePrivateProfileStringA("Audio", "Music", std::to_string(m_music).c_str(), file.c_str());
}

void OptionSound::_ReadSettingsFromFile()
{
	std::string file[1] = { "../Configuration/AudioSettings.ini" };
	std::string names[1] = { "Audio" };
	for (int k = 0; k < 1; k++)
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

void OptionSound::_ParseFileInputInt(const std::string & name, int key)
{
	if (name == "Master")
	{
		m_master = key;
	}
	else if (name == "Effects")
	{
		m_effects = key;
	}
	else if (name == "Ambient")
	{
		m_ambient = key;
	}
	else if (name == "Music")
	{
		m_music = key;
	}
}

void OptionSound::Load()
{
	std::cout << "OptionSound Load" << std::endl;
}

void OptionSound::unLoad()
{
	std::cout << "OptionSound unLoad" << std::endl;
}