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
}

void PauseMenu::Update(double deltaTime, Camera* camera)
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
			case SliderFovButton:
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
					m_buttons[ButtonOrder::SliderFovButton]->setPosition(pos, m_buttons[ButtonOrder::SliderFovButton]->getPosition().y);
				}
				camera->setFOV(DirectX::XMConvertToRadians(m_fov));
				m_text[TextOrder::SliderFov]->setString("Field of View: " + std::to_string(m_fov));
				break;
			case SliderSensXButton:
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
					m_buttons[ButtonOrder::SliderSensXButton]->setPosition(pos, m_buttons[ButtonOrder::SliderSensXButton]->getPosition().y);
				}
				m_text[TextOrder::SliderSensitivityX]->setString("X-Axis: " + std::to_string(m_sens.x));
				break;
			case SliderSensYButton:
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
					m_buttons[ButtonOrder::SliderSensYButton]->setPosition(pos, m_buttons[ButtonOrder::SliderSensYButton]->getPosition().y);
				}
				m_text[TextOrder::SliderSensitivityY]->setString("Y-Axis: " + std::to_string(m_sens.y));
				break;

			case SliderMasterButton:
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
					m_buttons[ButtonOrder::SliderMasterButton]->setPosition(pos, m_buttons[ButtonOrder::SliderMasterButton]->getPosition().y);
				}
				m_text[TextOrder::SliderMaster]->setString("Master Volume: " + std::to_string(m_master));
				AudioEngine::SetMasterVolume((float)m_master / 100.0f);
				break;
			case SliderEffectsButton:
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
					m_buttons[ButtonOrder::SliderEffectsButton]->setPosition(pos, m_buttons[ButtonOrder::SliderEffectsButton]->getPosition().y);
				}
				m_text[TextOrder::SliderEffects]->setString("Effects Volume: " + std::to_string(m_effects));
				AudioEngine::SetEffectVolume((float)m_effects / 100.0f);
				break;
			case SliderAmbientButton:
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
					m_buttons[ButtonOrder::SliderAmbientButton]->setPosition(pos, m_buttons[ButtonOrder::SliderAmbientButton]->getPosition().y);
				}
				m_text[TextOrder::SliderAmbient]->setString("Ambient Volume: " + std::to_string(m_ambient));
				AudioEngine::SetAmbientVolume((float)m_ambient / 100.0f);
				break;
			case SliderMusicButton:
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
					m_buttons[ButtonOrder::SliderMusicButton]->setPosition(pos, m_buttons[ButtonOrder::SliderMusicButton]->getPosition().y);
				}
				m_text[TextOrder::SliderMusic]->setString("Music Volume: " + std::to_string(m_music));
				AudioEngine::SetMusicVolume((float)m_music / 100.0f);
				break;

			case ReturnButton:
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
	m_text.push_back(Quad::CreateButton("", 0.2f, 0.79f, 0.65f, 0.1f));
	m_text[TextOrder::SlideBarFov]->setUnpressedTexture("gui_hover_pixel");
	m_text[TextOrder::SlideBarFov]->setPressedTexture("gui_slider_slide");
	m_text[TextOrder::SlideBarFov]->setHoverTexture("gui_slider_slide");
	m_text[TextOrder::SlideBarFov]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[TextOrder::SlideBarFov]->setFont(FontHandler::getFont("consolas16")); 

	m_text.push_back(Quad::CreateButton("", 0.2f, 0.64f, 0.65f, 0.1f));
	m_text[TextOrder::SliderBarSensX]->setUnpressedTexture("gui_hover_pixel");
	m_text[TextOrder::SliderBarSensX]->setPressedTexture("gui_slider_slide");
	m_text[TextOrder::SliderBarSensX]->setHoverTexture("gui_slider_slide");
	m_text[TextOrder::SliderBarSensX]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[TextOrder::SliderBarSensX]->setFont(FontHandler::getFont("consolas16"));

	m_text.push_back(Quad::CreateButton("", 0.2f, 0.49f, 0.65f, 0.1f));
	m_text[TextOrder::SliderBarSensY]->setUnpressedTexture("gui_hover_pixel");
	m_text[TextOrder::SliderBarSensY]->setPressedTexture("gui_slider_slide");
	m_text[TextOrder::SliderBarSensY]->setHoverTexture("gui_slider_slide");
	m_text[TextOrder::SliderBarSensY]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[TextOrder::SliderBarSensY]->setFont(FontHandler::getFont("consolas16"));

	m_text.push_back(Quad::CreateButton("", 0.775f, 0.79f, 0.76f, 0.1f));
	m_text[TextOrder::SliderBarMaster]->setUnpressedTexture("gui_hover_pixel");
	m_text[TextOrder::SliderBarMaster]->setPressedTexture("gui_slider_slide");
	m_text[TextOrder::SliderBarMaster]->setHoverTexture("gui_slider_slide");
	m_text[TextOrder::SliderBarMaster]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[TextOrder::SliderBarMaster]->setFont(FontHandler::getFont("consolas16"));

	m_text.push_back(Quad::CreateButton("", 0.775f, 0.64f, 0.76f, 0.1f));
	m_text[TextOrder::SliderBarEffects]->setUnpressedTexture("gui_hover_pixel");
	m_text[TextOrder::SliderBarEffects]->setPressedTexture("gui_slider_slide");
	m_text[TextOrder::SliderBarEffects]->setHoverTexture("gui_slider_slide");
	m_text[TextOrder::SliderBarEffects]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[TextOrder::SliderBarEffects]->setFont(FontHandler::getFont("consolas16"));

	m_text.push_back(Quad::CreateButton("", 0.775f, 0.49f, 0.76f, 0.1f));
	m_text[TextOrder::SliderBarAmbient]->setUnpressedTexture("gui_hover_pixel");
	m_text[TextOrder::SliderBarAmbient]->setPressedTexture("gui_slider_slide");
	m_text[TextOrder::SliderBarAmbient]->setHoverTexture("gui_slider_slide");
	m_text[TextOrder::SliderBarAmbient]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[TextOrder::SliderBarAmbient]->setFont(FontHandler::getFont("consolas16"));

	m_text.push_back(Quad::CreateButton("", 0.775f, 0.34f, 0.76f, 0.1f));
	m_text[TextOrder::SliderBarMusic]->setUnpressedTexture("gui_hover_pixel");
	m_text[TextOrder::SliderBarMusic]->setPressedTexture("gui_slider_slide");
	m_text[TextOrder::SliderBarMusic]->setHoverTexture("gui_slider_slide");
	m_text[TextOrder::SliderBarMusic]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[TextOrder::SliderBarMusic]->setFont(FontHandler::getFont("consolas16"));

	m_text.push_back(Quad::CreateButton("Pause", 0.5f, 0.9f, 1.0f, 1.0f));
	m_text[TextOrder::Title]->setUnpressedTexture("gui_transparent_pixel");
	m_text[TextOrder::Title]->setPressedTexture("gui_pressed_pixel");
	m_text[TextOrder::Title]->setHoverTexture("gui_hover_pixel");
	m_text[TextOrder::Title]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[TextOrder::Title]->setFont(FontHandler::getFont("consolas32"));

	m_text.push_back(Quad::CreateButton("Field of View:", 0.2f, 0.86f, 0.73f, 0.12f));
	m_text[TextOrder::SliderFov]->setUnpressedTexture("gui_transparent_pixel");
	m_text[TextOrder::SliderFov]->setPressedTexture("gui_pressed_pixel");
	m_text[TextOrder::SliderFov]->setHoverTexture("gui_hover_pixel");
	m_text[TextOrder::SliderFov]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[TextOrder::SliderFov]->setFont(FontHandler::getFont("consolas16"));
	m_text[TextOrder::SliderFov]->setString("Field of View: " + std::to_string(m_fov));


	float xPos = ((((float)m_fov - (float)MIN_MAX_FOV.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_FOV.y - (float)MIN_MAX_FOV.x) + MIN_MAX_SLIDE_GENERAL.x);

	m_buttons.push_back(Quad::CreateButton("", 0.2, 0.79f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderFovButton]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderFovButton]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderFovButton]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderFovButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("X-Axis: ", 0.2f, 0.71f, 0.73f, 0.12f));
	m_text[TextOrder::SliderSensitivityX]->setUnpressedTexture("gui_transparent_pixel");
	m_text[TextOrder::SliderSensitivityX]->setPressedTexture("gui_pressed_pixel");
	m_text[TextOrder::SliderSensitivityX]->setHoverTexture("gui_hover_pixel");
	m_text[TextOrder::SliderSensitivityX]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[TextOrder::SliderSensitivityX]->setFont(FontHandler::getFont("consolas16"));
	m_text[TextOrder::SliderSensitivityX]->setString("X-Axis: " + std::to_string(m_sens.x));

	xPos = (((float)m_sens.x - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE_GENERAL.x;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.64f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderSensXButton]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensXButton]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensXButton]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensXButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("Y-Axis: ", 0.2f, 0.56f, 0.73f, 0.12f));
	m_text[TextOrder::SliderSensitivityY]->setUnpressedTexture("gui_transparent_pixel");
	m_text[TextOrder::SliderSensitivityY]->setPressedTexture("gui_pressed_pixel");
	m_text[TextOrder::SliderSensitivityY]->setHoverTexture("gui_hover_pixel");
	m_text[TextOrder::SliderSensitivityY]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[TextOrder::SliderSensitivityY]->setFont(FontHandler::getFont("consolas16"));
	m_text[TextOrder::SliderSensitivityY]->setString("Y-Axis: " + std::to_string(m_sens.y));

	xPos = (((float)m_sens.y - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE_GENERAL.x;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.49f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderSensYButton]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensYButton]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensYButton]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderSensYButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));


	m_text.push_back(Quad::CreateButton("Master", 0.77f, 0.86f, 0.73f, 0.12f));
	m_text[TextOrder::SliderMaster]->setUnpressedTexture("gui_transparent_pixel");
	m_text[TextOrder::SliderMaster]->setPressedTexture("gui_pressed_pixel");
	m_text[TextOrder::SliderMaster]->setHoverTexture("gui_hover_pixel");
	m_text[TextOrder::SliderMaster]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[TextOrder::SliderMaster]->setFont(FontHandler::getFont("consolas16"));
	m_text[TextOrder::SliderMaster]->setString("Master Volume: " + std::to_string(m_master));

	xPos = ((((float)m_master - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x);

	m_buttons.push_back(Quad::CreateButton("", xPos, 0.79f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderMasterButton]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderMasterButton]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderMasterButton]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderMasterButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("Effects", 0.77f, 0.71f, 0.73f, 0.12f));
	m_text[TextOrder::SliderEffects]->setUnpressedTexture("gui_transparent_pixel");
	m_text[TextOrder::SliderEffects]->setPressedTexture("gui_pressed_pixel");
	m_text[TextOrder::SliderEffects]->setHoverTexture("gui_hover_pixel");
	m_text[TextOrder::SliderEffects]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[TextOrder::SliderEffects]->setFont(FontHandler::getFont("consolas16"));
	m_text[TextOrder::SliderEffects]->setString("Effects Volume: " + std::to_string(m_effects));

	xPos = (((float)m_effects - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.64f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderEffectsButton]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderEffectsButton]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderEffectsButton]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderEffectsButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("Ambient: ", 0.77f, 0.56f, 0.73f, 0.12f));
	m_text[TextOrder::SliderAmbient]->setUnpressedTexture("gui_transparent_pixel");
	m_text[TextOrder::SliderAmbient]->setPressedTexture("gui_pressed_pixel");
	m_text[TextOrder::SliderAmbient]->setHoverTexture("gui_hover_pixel");
	m_text[TextOrder::SliderAmbient]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[TextOrder::SliderAmbient]->setFont(FontHandler::getFont("consolas16"));
	m_text[TextOrder::SliderAmbient]->setString("Ambient Volume: " + std::to_string(m_ambient));

	xPos = (((float)m_ambient - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.49f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderAmbientButton]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderAmbientButton]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderAmbientButton]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderAmbientButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_text.push_back(Quad::CreateButton("Music: ", 0.77f, 0.41f, 0.73f, 0.12f));
	m_text[TextOrder::SliderMusic]->setUnpressedTexture("gui_transparent_pixel");
	m_text[TextOrder::SliderMusic]->setPressedTexture("gui_pressed_pixel");
	m_text[TextOrder::SliderMusic]->setHoverTexture("gui_hover_pixel");
	m_text[TextOrder::SliderMusic]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_text[TextOrder::SliderMusic]->setFont(FontHandler::getFont("consolas16"));
	m_text[TextOrder::SliderMusic]->setString("Music Volume: " + std::to_string(m_music));

	xPos = (((float)m_music - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
	m_buttons.push_back(Quad::CreateButton("", xPos, 0.34f, 0.04f, 0.10f));
	m_buttons[ButtonOrder::SliderMusicButton]->setUnpressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderMusicButton]->setPressedTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderMusicButton]->setHoverTexture("gui_slider_button");
	m_buttons[ButtonOrder::SliderMusicButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

	m_buttons.push_back(Quad::CreateButton("Save and return", 0.2f, 0.13f, 0.73f, 0.12f));
	m_buttons[ButtonOrder::ReturnButton]->setUnpressedTexture("gui_transparent_pixel");
	m_buttons[ButtonOrder::ReturnButton]->setPressedTexture("gui_pressed_pixel");
	m_buttons[ButtonOrder::ReturnButton]->setHoverTexture("gui_hover_pixel");
	m_buttons[ButtonOrder::ReturnButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_buttons[ButtonOrder::ReturnButton]->setFont(FontHandler::getFont("consolas16"));

	m_buttons.push_back(Quad::CreateButton("Main Menu", 0.7f, 0.13f, 0.73f, 0.12f));
	m_buttons[ButtonOrder::MainMenuButton]->setUnpressedTexture("gui_transparent_pixel");
	m_buttons[ButtonOrder::MainMenuButton]->setPressedTexture("gui_pressed_pixel");
	m_buttons[ButtonOrder::MainMenuButton]->setHoverTexture("gui_hover_pixel");
	m_buttons[ButtonOrder::MainMenuButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_buttons[ButtonOrder::MainMenuButton]->setFont(FontHandler::getFont("consolas16"));
}

void PauseMenu::_handleGamePadInput(double dt)
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
			m_sliderPressed = false;
		}
		else if ((!pressedLastFrame || timer > 0.5) && (GamePadHandler::IsDownDpadPressed() || GamePadHandler::GetLeftStickYPosition() < 0.0f))
		{
			m_liu = Gamepad;
			dir = 1;
			timer = 0.0;
			m_sliderPressed = false;
		}

		m_currentButton += dir;

		if (m_currentButton < 0)
			m_currentButton = (short)MainMenuButton;
		else if (m_currentButton > (short)MainMenuButton)
			m_currentButton = 0;

		if (m_liu == Gamepad)
			_updateSelectionStates();

		if (GamePadHandler::IsAPressed())
		{
			m_liu = Gamepad;
			if (m_buttons[m_currentButton]->isSelected())
			{
				this->m_buttons[m_currentButton]->setState(ButtonStates::Pressed);
				m_buttonPressed = true;
				if (m_currentButton != MainMenuButton)
				{
					this->m_text[m_currentButton]->setState(ButtonStates::Pressed);
					m_sliderPressed = true;
				}
			}
		}

		if (m_currentButton == MainMenuButton)
		{
			m_mainManuPressed = true;
		}

		pressedLastFrame = GamePadHandler::IsUpDpadPressed() || GamePadHandler::GetLeftStickYPosition() > 0.0f || GamePadHandler::IsDownDpadPressed() || GamePadHandler::GetLeftStickYPosition() < 0.0f;
	}
}

void PauseMenu::_handleKeyboardInput(double dt)
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
		m_currentButton = (short)MainMenuButton;
	else if (m_currentButton > (short)MainMenuButton)
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
			if (m_currentButton == SliderFovButton || m_currentButton == SliderSensXButton || m_currentButton == SliderSensYButton
				|| m_currentButton == SliderMasterButton || m_currentButton == SliderEffectsButton || m_currentButton == SliderAmbientButton || m_currentButton == SliderMusicButton)
			{
				this->m_text[m_currentButton]->setState(ButtonStates::Pressed);
				m_sliderPressed = true;
			}
		}
	}

	if (m_currentButton == MainMenuButton)
	{
		m_mainManuPressed = true;
	}
	pressedLastFrame = InputHandler::isKeyPressed(InputHandler::Up) || InputHandler::isKeyPressed(InputHandler::Down);
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
				if (i == MainMenuButton)
				{
					m_mainManuPressed = true; 
				}
				break;
			}
			else if (m_buttons[i]->isPressed(mousePos))
			{
				m_currentButton = i;
				ButtonOrder type = (ButtonOrder)i;
				if (type == SliderFovButton || type == SliderSensXButton || type == SliderSensYButton || type == SliderAmbientButton
					|| type == SliderEffectsButton || type == SliderMasterButton || type == SliderMusicButton)
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
		if (i == SliderFovButton || i == SliderSensXButton || i == SliderSensYButton || i == SliderMasterButton || i == SliderEffectsButton ||
			i == SliderAmbientButton || i == SliderMusicButton)
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

const bool & PauseMenu::getMainMenuPressed() const
{
	return m_mainManuPressed; 
}

void PauseMenu::Load()
{
	std::string path = "../Assets/PAUSEFOLDER";
	for (auto & p : std::filesystem::directory_iterator(path))
	{
		if (p.is_regular_file())
		{
			auto file = p.path();
			if (file.has_filename() && file.has_extension())
			{
				std::wstring stem = file.stem().generic_wstring();
				std::wstring extension = file.extension().generic_wstring();
				std::cout << "Attempting to load: " << file.stem().generic_string() << "\n";
				if (extension == L".png" || extension == L".jpg")
					Manager::g_textureManager.loadGUITexture(stem, file.generic_wstring());
			}
		}
	}
}

void PauseMenu::unLoad()
{
	Manager::g_textureManager.UnloadGUITextures();
}
