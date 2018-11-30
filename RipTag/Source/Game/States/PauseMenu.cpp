#include "RipTagPCH.h"
#include "PauseMenu.h"

PauseMenu::PauseMenu()
{
	m_liu = Mouse;
	m_buttonPressed = false;
	m_currentButton = -1;
	m_mouseMoved = false;
	m_sliderPressed = false;
	Load(); 
	_ReadSettingsFromFile(); 
	_initButtons(); 
}

PauseMenu::~PauseMenu()
{
	unLoad(); 

}

void PauseMenu::Update(double deltaTime, Camera* camera)
{
	if (!InputHandler::getShowCursor())
		InputHandler::setShowCursor(TRUE);

	_handleMouseInput();

	if (m_currentButton != -1)
		if (m_sliderPressed || m_buttonPressed)
		{
			switch ((ButtonOrder)m_currentButton)
			{
			case SliderFov:
				if (m_liu == Mouse)
				{
					_slideGeneral();
					m_fov = (((m_buttons[m_currentButton]->getPosition().x - MIN_MAX_SLIDE_GENERAL.x) * ((float)MIN_MAX_FOV.y - (float)MIN_MAX_FOV.x)) / (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) + (float)MIN_MAX_FOV.x;
				}
				else
				{
					switch (m_liu)
					{
					case Gamepad:

						m_stickTimerFOV += deltaTime;

						if (GamePadHandler::IsRightDpadPressed())
							m_fov++;
						if (GamePadHandler::IsLeftDpadPressed())
							m_fov--;

						if (GamePadHandler::GetLeftStickXPosition() > 0 && m_stickTimerFOV >= 0.06f)
						{
							m_fov++;
							m_stickTimerFOV = 0;
						}
						else if (GamePadHandler::GetLeftStickXPosition() < 0 && m_stickTimerFOV >= 0.06f)
						{
							m_fov--;
							m_stickTimerFOV = 0;
						}

						if (m_fov < MIN_MAX_FOV.x)
							m_fov = MIN_MAX_FOV.x;
						if (m_fov > MIN_MAX_FOV.y)
							m_fov = MIN_MAX_FOV.y;
						break;
					case Keyboard:
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

					float pos = (((float)m_fov - (float)MIN_MAX_FOV.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_FOV.y - (float)MIN_MAX_FOV.x) + MIN_MAX_SLIDE_GENERAL.x;
					m_buttons[ButtonOrder::SliderFov]->setPosition(pos, m_buttons[ButtonOrder::SliderFov]->getPosition().y);
				}
				camera->setFOV(DirectX::XMConvertToRadians(m_fov));
				m_text[ButtonOrder::SliderFov]->setString("Field of View: " + std::to_string(m_fov));
				break;
			case SliderSensitivityX:
				if (m_liu == Mouse)
				{
					_slideGeneral();
					m_sens.x = (((m_buttons[m_currentButton]->getPosition().x - MIN_MAX_SLIDE_GENERAL.x) * ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x)) / (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) + (float)MIN_MAX_SENSITIVITY.x;
				}
				else
				{
					switch (m_liu)
					{
					case Gamepad:

						m_stickTimerX += deltaTime;

						if (GamePadHandler::IsRightDpadPressed())
							m_sens.x++;
						if (GamePadHandler::IsLeftDpadPressed())
							m_sens.x--;

						if (GamePadHandler::GetLeftStickXPosition() > 0 && m_stickTimerX >= 0.1f)
						{
							m_sens.x++;
							m_stickTimerX = 0;
						}
						else if (GamePadHandler::GetLeftStickXPosition() < 0 && m_stickTimerX >= 0.1f)
						{
							m_sens.x--;
							m_stickTimerX = 0;
						}

						if (m_sens.x < MIN_MAX_SENSITIVITY.x)
							m_sens.x = MIN_MAX_SENSITIVITY.x;
						if (m_sens.x > MIN_MAX_SENSITIVITY.y)
							m_sens.x = MIN_MAX_SENSITIVITY.y;
						break;
					case Keyboard:
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
					float pos = (((float)m_sens.x - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE_GENERAL.x;
					m_buttons[ButtonOrder::SliderSensitivityX]->setPosition(pos, m_buttons[ButtonOrder::SliderSensitivityX]->getPosition().y);
				}
				m_text[ButtonOrder::SliderSensitivityX]->setString("X-Axis: " + std::to_string(m_sens.x));
				break;
			case SliderSensitivityY:
				if (m_liu == Mouse)
				{
					_slideGeneral();
					m_sens.y = (((m_buttons[m_currentButton]->getPosition().x - MIN_MAX_SLIDE_GENERAL.x) * ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x)) / (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) + (float)MIN_MAX_SENSITIVITY.x;
				}
				else
				{

					m_stickTimerY += deltaTime;

					switch (m_liu)
					{
					case Gamepad:
						if (GamePadHandler::IsRightDpadPressed())
							m_sens.y++;
						if (GamePadHandler::IsLeftDpadPressed())
							m_sens.y--;

						if (GamePadHandler::GetLeftStickXPosition() > 0 && m_stickTimerY > 0.1f)
						{
							m_sens.y++;
							m_stickTimerY = 0;
						}
						else if (GamePadHandler::GetLeftStickXPosition() < 0 && m_stickTimerY > 0.1f)
						{
							m_sens.y--;
							m_stickTimerY = 0;
						}

						if (m_sens.y < MIN_MAX_SENSITIVITY.x)
							m_sens.y = MIN_MAX_SENSITIVITY.x;
						if (m_sens.y > MIN_MAX_SENSITIVITY.y)
							m_sens.y = MIN_MAX_SENSITIVITY.y;

						break;
					case Keyboard:
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


					float pos = (((float)m_sens.y - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE_GENERAL.x;
					m_buttons[ButtonOrder::SliderSensitivityY]->setPosition(pos, m_buttons[ButtonOrder::SliderSensitivityY]->getPosition().y);
				}
				m_text[ButtonOrder::SliderSensitivityY]->setString("Y-Axis: " + std::to_string(m_sens.y));
				break;

			case SliderMaster:
				if (m_liu == Mouse)
				{
					_slideSound();
					m_master = (((m_buttons[m_currentButton]->getPosition().x - MIN_MAX_SLIDE_SOUND.x) * ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x)) / (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) + (float)MIN_MAX_SOUND.x;
				}
				else
				{
					switch (m_liu)
					{
					case Gamepad:
						if (GamePadHandler::IsRightDpadPressed() || GamePadHandler::GetLeftStickXPosition() > 0)
							m_master++;
						if (GamePadHandler::IsLeftDpadPressed() || GamePadHandler::GetLeftStickXPosition() < 0)
							m_master--;

						if (m_master < MIN_MAX_SOUND.x)
							m_master = MIN_MAX_SOUND.x;
						if (m_master > MIN_MAX_SOUND.y)
							m_master = MIN_MAX_SOUND.y;

						break;
					case Keyboard:
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

					float pos = (((float)m_master - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
					m_buttons[ButtonOrder::SliderMaster]->setPosition(pos, m_buttons[ButtonOrder::SliderMaster]->getPosition().y);
				}
				m_text[ButtonOrder::SliderMaster]->setString("Master Volume: " + std::to_string(m_master));
				AudioEngine::SetMasterVolume((float)m_master / 100.0f);
				break;
			case SliderEffects:
				if (m_liu == Mouse)
				{
					_slideSound();
					m_effects = (((m_buttons[m_currentButton]->getPosition().x - MIN_MAX_SLIDE_SOUND.x) * ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x)) / (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) + (float)MIN_MAX_SOUND.x;
				}
				else
				{
					switch (m_liu)
					{
					case Gamepad:
						if (GamePadHandler::IsRightDpadPressed() || GamePadHandler::GetLeftStickXPosition() > 0)
							m_effects++;
						if (GamePadHandler::IsLeftDpadPressed() || GamePadHandler::GetLeftStickXPosition() < 0)
							m_effects--;

						if (m_effects < MIN_MAX_SOUND.x)
							m_effects = MIN_MAX_SOUND.x;
						if (m_effects > MIN_MAX_SOUND.y)
							m_effects = MIN_MAX_SOUND.y;

						break;
					case Keyboard:
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
					float pos = (((float)m_effects - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
					m_buttons[ButtonOrder::SliderEffects]->setPosition(pos, m_buttons[ButtonOrder::SliderEffects]->getPosition().y);
				}
				m_text[ButtonOrder::SliderEffects]->setString("Effects Volume: " + std::to_string(m_effects));
				AudioEngine::SetEffectVolume((float)m_effects / 100.0f);
				break;
			case SliderAmbient:
				if (m_liu == Mouse)
				{
					_slideSound();
					m_ambient = (((m_buttons[m_currentButton]->getPosition().x - MIN_MAX_SLIDE_SOUND.x) * ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x)) / (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) + (float)MIN_MAX_SOUND.x;
				}
				else
				{
					switch (m_liu)
					{
					case Gamepad:
						if (GamePadHandler::IsRightDpadPressed() || GamePadHandler::GetLeftStickXPosition() > 0)
							m_ambient++;
						if (GamePadHandler::IsLeftDpadPressed() || GamePadHandler::GetLeftStickXPosition() < 0)
							m_ambient--;

						if (m_ambient < MIN_MAX_SOUND.x)
							m_ambient = MIN_MAX_SOUND.x;
						if (m_ambient > MIN_MAX_SOUND.y)
							m_ambient = MIN_MAX_SOUND.y;

						break;
					case Keyboard:
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

					float pos = (((float)m_ambient - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
					m_buttons[ButtonOrder::SliderAmbient]->setPosition(pos, m_buttons[ButtonOrder::SliderAmbient]->getPosition().y);
				}
				m_text[ButtonOrder::SliderAmbient]->setString("Ambient Volume: " + std::to_string(m_ambient));
				AudioEngine::SetAmbientVolume((float)m_ambient / 100.0f);
				break;
			case SliderMusic:
				if (m_liu == Mouse)
				{
					_slideSound();
					m_music = (((m_buttons[m_currentButton]->getPosition().x - MIN_MAX_SLIDE_SOUND.x) * ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x)) / (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) + (float)MIN_MAX_SOUND.x;
				}
				else
				{
					switch (m_liu)
					{
					case Gamepad:
						if (GamePadHandler::IsRightDpadPressed() || GamePadHandler::GetLeftStickXPosition() > 0)
							m_music++;
						if (GamePadHandler::IsLeftDpadPressed() || GamePadHandler::GetLeftStickXPosition() < 0)
							m_music--;

						if (m_music < MIN_MAX_SOUND.x)
							m_music = MIN_MAX_SOUND.x;
						if (m_music > MIN_MAX_SOUND.y)
							m_music = MIN_MAX_SOUND.y;

						break;
					case Keyboard:
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

					float pos = (((float)m_music - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
					m_buttons[ButtonOrder::SliderMusic]->setPosition(pos, m_buttons[ButtonOrder::SliderMusic]->getPosition().y);
				}
				m_text[ButtonOrder::SliderMusic]->setString("Music Volume: " + std::to_string(m_music));
				AudioEngine::SetMusicVolume((float)m_music / 100.0f);
				break;

			case Return:
				_WriteSettingsToFile();
				Input::ReadSettingsFile();
				m_exitPause = true; 
				break;
			}
		}
}

void PauseMenu::Draw()
{
	for (auto & t : m_text)
		t->Draw();
	for (auto & b : m_buttons)
		b->Draw();
}

void PauseMenu::_slideSound()
{
	DirectX::XMFLOAT2 mp = InputHandler::getMousePosition();
	mp.x /= InputHandler::getWindowSize().x;
	mp.x *= InputHandler::getViewportSize().x;

	DirectX::XMFLOAT2A pos = m_buttons[m_currentButton]->getPosition();
	m_buttons[m_currentButton]->setPosition(mp.x / InputHandler::getViewportSize().x, pos.y);
	if (m_buttons[m_currentButton]->getPosition().x < MIN_MAX_SLIDE_SOUND.x)
		m_buttons[m_currentButton]->setPosition(MIN_MAX_SLIDE_SOUND.x, pos.y);
	else if (m_buttons[m_currentButton]->getPosition().x > MIN_MAX_SLIDE_SOUND.y)
		m_buttons[m_currentButton]->setPosition(MIN_MAX_SLIDE_SOUND.y, pos.y);
}


void PauseMenu::_slideGeneral()
{
	DirectX::XMFLOAT2 mp = InputHandler::getMousePosition();
	mp.x /= InputHandler::getWindowSize().x;
	mp.x *= InputHandler::getViewportSize().x;

	DirectX::XMFLOAT2A pos = m_buttons[m_currentButton]->getPosition();
	m_buttons[m_currentButton]->setPosition(mp.x / InputHandler::getViewportSize().x, pos.y);
	if (m_buttons[m_currentButton]->getPosition().x < MIN_MAX_SLIDE_GENERAL.x)
		m_buttons[m_currentButton]->setPosition(MIN_MAX_SLIDE_GENERAL.x, pos.y);
	else if (m_buttons[m_currentButton]->getPosition().x > MIN_MAX_SLIDE_GENERAL.y)
		m_buttons[m_currentButton]->setPosition(MIN_MAX_SLIDE_GENERAL.y, pos.y);
}

void PauseMenu::_initButtons()
{
	m_text.push_back(Quad::CreateButton("Field of View", 0.2f, 0.86f, 0.73f, 0.12f));
	m_text[ButtonOrder::SliderFov]->setUnpressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderFov]->setPressedTexture("gui_pressed_pixel");
	m_text[ButtonOrder::SliderFov]->setHoverTexture("gui_hover_pixel");
	m_text[ButtonOrder::SliderFov]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderFov]->setFont(FontHandler::getFont("consolas16"));
	m_text[ButtonOrder::SliderFov]->setString("Field of View: " + std::to_string(m_fov));

	float xPos = ((((float)m_fov - (float)MIN_MAX_FOV.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_FOV.y - (float)MIN_MAX_FOV.x) + MIN_MAX_SLIDE_GENERAL.x);

	m_buttons.push_back(Quad::CreateButton("", 0.2, 0.79f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderFov]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderFov]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderFov]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderFov]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("X-Axis: ", 0.2f, 0.71f, 0.73f, 0.12f));
	m_text[ButtonOrder::SliderSensitivityX]->setUnpressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderSensitivityX]->setPressedTexture("gui_pressed_pixel");
	m_text[ButtonOrder::SliderSensitivityX]->setHoverTexture("gui_hover_pixel");
	m_text[ButtonOrder::SliderSensitivityX]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderSensitivityX]->setFont(FontHandler::getFont("consolas16"));
	m_text[ButtonOrder::SliderSensitivityX]->setString("X-Axis: " + std::to_string(m_sens.x));

	xPos = (((float)m_sens.x - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE_GENERAL.x;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.64f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderSensitivityX]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensitivityX]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensitivityX]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensitivityX]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("Y-Axis: ", 0.2f, 0.56f, 0.73f, 0.12f));
	m_text[ButtonOrder::SliderSensitivityY]->setUnpressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderSensitivityY]->setPressedTexture("gui_pressed_pixel");
	m_text[ButtonOrder::SliderSensitivityY]->setHoverTexture("gui_hover_pixel");
	m_text[ButtonOrder::SliderSensitivityY]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderSensitivityY]->setFont(FontHandler::getFont("consolas16"));
	m_text[ButtonOrder::SliderSensitivityY]->setString("Y-Axis: " + std::to_string(m_sens.y));

	xPos = (((float)m_sens.y - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE_GENERAL.x;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.49f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderSensitivityY]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensitivityY]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensitivityY]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensitivityY]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));


	m_text.push_back(Quad::CreateButton("Master", 0.77f, 0.86f, 0.73f, 0.12f));
	m_text[ButtonOrder::SliderMaster]->setUnpressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderMaster]->setPressedTexture("gui_pressed_pixel");
	m_text[ButtonOrder::SliderMaster]->setHoverTexture("gui_hover_pixel");
	m_text[ButtonOrder::SliderMaster]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderMaster]->setFont(FontHandler::getFont("consolas16"));
	m_text[ButtonOrder::SliderMaster]->setString("Master Volume: " + std::to_string(m_master));

	xPos = ((((float)m_master - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x);

	m_buttons.push_back(Quad::CreateButton("", xPos, 0.80f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderMaster]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderMaster]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderMaster]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderMaster]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("Effects", 0.77f, 0.71f, 0.73f, 0.12f));
	m_text[ButtonOrder::SliderEffects]->setUnpressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderEffects]->setPressedTexture("gui_pressed_pixel");
	m_text[ButtonOrder::SliderEffects]->setHoverTexture("gui_hover_pixel");
	m_text[ButtonOrder::SliderEffects]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderEffects]->setFont(FontHandler::getFont("consolas16"));
	m_text[ButtonOrder::SliderEffects]->setString("Effects Volume: " + std::to_string(m_effects));

	xPos = (((float)m_effects - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.64f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderEffects]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderEffects]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderEffects]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderEffects]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("Ambient: ", 0.77f, 0.56f, 0.73f, 0.12f));
	m_text[ButtonOrder::SliderAmbient]->setUnpressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderAmbient]->setPressedTexture("gui_pressed_pixel");
	m_text[ButtonOrder::SliderAmbient]->setHoverTexture("gui_hover_pixel");
	m_text[ButtonOrder::SliderAmbient]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderAmbient]->setFont(FontHandler::getFont("consolas16"));
	m_text[ButtonOrder::SliderAmbient]->setString("Ambient Volume: " + std::to_string(m_ambient));

	xPos = (((float)m_ambient - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.49f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderAmbient]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderAmbient]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderAmbient]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderAmbient]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("Music: ", 0.77f, 0.41f, 0.73f, 0.12f));
	m_text[ButtonOrder::SliderMusic]->setUnpressedTexture("gui_transparent_pixel");
	m_text[ButtonOrder::SliderMusic]->setPressedTexture("gui_pressed_pixel");
	m_text[ButtonOrder::SliderMusic]->setHoverTexture("gui_hover_pixel");
	m_text[ButtonOrder::SliderMusic]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[ButtonOrder::SliderMusic]->setFont(FontHandler::getFont("consolas16"));
	m_text[ButtonOrder::SliderMusic]->setString("Music Volume: " + std::to_string(m_music));

	xPos = (((float)m_music - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
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
}

void PauseMenu::_handleGamePadInput(double dt)
{
}

void PauseMenu::_handleKeyboardInput(double dt)
{
}

bool PauseMenu::_handleMouseInput()
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
				ButtonOrder type = (ButtonOrder)i;
				if (type == SliderFov || type == SliderSensitivityX || type == SliderSensitivityY || type == SliderAmbient 
					|| type == SliderEffects || type == SliderMaster || type == SliderMusic)
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

void PauseMenu::_updateSelectionStates()
{
	m_buttonPressed = false;
	for (size_t i = 0; i < m_buttons.size(); i++)
	{
		if (i == SliderFov || i == SliderSensitivityX || i == SliderSensitivityY)
		{
			if (i != m_currentButton)
			{
				m_text[i]->Select(false);
				m_text[i]->setState(ButtonStates::Normal);
			}
			else
			{
				if (!m_text[i]->isSelected()
					&& (m_text[i]->getState() != (unsigned int)ButtonStates::Pressed)
					)
				{
					m_text[i]->setState(ButtonStates::Hover);
				}
			}
		}
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

void PauseMenu::_WriteSettingsToFile()
{
	// Player Settings
	std::string filePlayer = "../Configuration/PlayerMapping.ini";
	WritePrivateProfileStringA("Player", "XAXIS", std::to_string(m_sens.x).c_str(), filePlayer.c_str());
	WritePrivateProfileStringA("Player", "YAXIS", std::to_string(m_sens.y).c_str(), filePlayer.c_str());
	WritePrivateProfileStringA("Player", "PlayerFOV", std::to_string(m_fov).c_str(), filePlayer.c_str());

	//Sound Settings
	std::string fileAudio = "../Configuration/AudioSettings.ini";
	WritePrivateProfileStringA("Audio", "Master", std::to_string(m_master).c_str(), fileAudio.c_str());
	WritePrivateProfileStringA("Audio", "Effects", std::to_string(m_effects).c_str(), fileAudio.c_str());
	WritePrivateProfileStringA("Audio", "Ambient", std::to_string(m_ambient).c_str(), fileAudio.c_str());
	WritePrivateProfileStringA("Audio", "Music", std::to_string(m_music).c_str(), fileAudio.c_str());
}

void PauseMenu::_ReadSettingsFromFile()
{
	std::string file[2] = { "../Configuration/PlayerMapping.ini", "../Configuration/AudioSettings.ini" };
	std::string names[2] = { "Player", "Audio" };
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

void PauseMenu::_ParseFileInputInt(const std::string & name, int key)
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
	else if (name == "Master")
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

const bool & PauseMenu::getExitPause() const
{
	return m_exitPause; 
}

void PauseMenu::Load()
{
	
}

void PauseMenu::unLoad()
{
	//Unload textures for pause.
}
