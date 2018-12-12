#include "RipTagPCH.h"
#include "PauseMenu.h"

PauseMenu::PauseMenu()
{
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
	for (auto & b : m_buttonsMain)
	{
		b->Release();
		delete b;
	}
	m_buttonsMain.clear();
	for (auto & b : m_buttonsOptions)
	{
		b->Release();
		delete b;
	}
	m_buttonsOptions.clear();
	if (m_fullscreenOverlay)
	{
		m_fullscreenOverlay->Release();
		delete m_fullscreenOverlay;
		m_fullscreenOverlay = nullptr;
	}
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

	if (Input::isUsingGamepad())
	{
		if (GamePadHandler::IsBReleased())
		{
			m_buttonPressed = true;
			if (m_inOptions)
				m_currentButton = ButtonOrderOptions::Back;
			else
				m_currentButton = ButtonOrderMain::ReturnButton;
		}
		if (GamePadHandler::IsStartReleased())
		{
			if (!m_inOptions)
			{
				m_buttonPressed = true;
				m_currentButton = ButtonOrderMain::ReturnButton;
			}
		}
	}
	if (InputHandler::wasKeyPressed(InputHandler::Esc))
	{
		m_buttonPressed = true;
		if (m_inOptions)
			m_currentButton = ButtonOrderOptions::Back;
		else
			m_currentButton = ButtonOrderMain::ReturnButton;
	}

	if (m_currentButton != -1)
	{
		if (m_inOptions)
		{
			_updateOptionsText();
			_updateOptions(deltaTime, camera);
		}
		else
			_updateMain();
	}
}

void PauseMenu::Draw()
{
	if (m_fullscreenOverlay)
		m_fullscreenOverlay->Draw();
	if (m_inOptions)
	{
		for (auto & t : m_text)
			t->Draw();
		for (auto & b : m_buttonsOptions)
			b->Draw();
	}
	else
	{
		for (auto & b : m_buttonsMain)
			b->Draw();
	}
}

void PauseMenu::_slideSound()
{
	DirectX::XMFLOAT2 mp = InputHandler::getMousePosition();

	mp.x /= InputHandler::getWindowSize().x;
	mp.x *= InputHandler::getViewportSize().x;

	DirectX::XMFLOAT2A pos = m_buttonsOptions[m_currentButton]->getPosition();
	m_buttonsOptions[m_currentButton]->setPosition(mp.x / InputHandler::getViewportSize().x, pos.y);

	float xPos = m_buttonsOptions[m_currentButton]->getPosition().x;

	if (xPos < MIN_MAX_SLIDE_SOUND.x)
		m_buttonsOptions[m_currentButton]->setPosition(MIN_MAX_SLIDE_SOUND.x, pos.y);
	else if (xPos > MIN_MAX_SLIDE_SOUND.y)
		m_buttonsOptions[m_currentButton]->setPosition(MIN_MAX_SLIDE_SOUND.y, pos.y);
}

void PauseMenu::_slideGeneral()
{
	DirectX::XMFLOAT2 mp = InputHandler::getMousePosition();

	mp.x /= InputHandler::getWindowSize().x;
	mp.x *= InputHandler::getViewportSize().x;

	DirectX::XMFLOAT2A pos = m_buttonsOptions[m_currentButton]->getPosition();
	m_buttonsOptions[m_currentButton]->setPosition(mp.x / InputHandler::getViewportSize().x, pos.y);

	float xPos = m_buttonsOptions[m_currentButton]->getPosition().x;

	if (xPos < MIN_MAX_SLIDE_GENERAL.x)
		m_buttonsOptions[m_currentButton]->setPosition(MIN_MAX_SLIDE_GENERAL.x, pos.y);
	else if (xPos > MIN_MAX_SLIDE_GENERAL.y)
		m_buttonsOptions[m_currentButton]->setPosition(MIN_MAX_SLIDE_GENERAL.y, pos.y);
}

void PauseMenu::_initButtons()
{
	//Option buttons/info text
	{
		m_text.push_back(Quad::CreateButton("", 0.2f, 0.79f, 0.65f, 0.1f));
		m_text[TextOrder::SlideBarFov]->setUnpressedTexture("gui_slider_slide");
		m_text[TextOrder::SlideBarFov]->setPressedTexture("gui_slider_slide");
		m_text[TextOrder::SlideBarFov]->setHoverTexture("gui_slider_slide");
		m_text[TextOrder::SlideBarFov]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		m_text[TextOrder::SlideBarFov]->setFont(FontHandler::getFont("consolas16")); 

		m_text.push_back(Quad::CreateButton("", 0.2f, 0.64f, 0.65f, 0.1f));
		m_text[TextOrder::SliderBarSensX]->setUnpressedTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarSensX]->setPressedTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarSensX]->setHoverTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarSensX]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		m_text[TextOrder::SliderBarSensX]->setFont(FontHandler::getFont("consolas16"));

		m_text.push_back(Quad::CreateButton("", 0.2f, 0.49f, 0.65f, 0.1f));
		m_text[TextOrder::SliderBarSensY]->setUnpressedTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarSensY]->setPressedTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarSensY]->setHoverTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarSensY]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		m_text[TextOrder::SliderBarSensY]->setFont(FontHandler::getFont("consolas16"));

		m_text.push_back(Quad::CreateButton("", 0.775f, 0.79f, 0.76f, 0.1f));
		m_text[TextOrder::SliderBarMaster]->setUnpressedTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarMaster]->setPressedTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarMaster]->setHoverTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarMaster]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		m_text[TextOrder::SliderBarMaster]->setFont(FontHandler::getFont("consolas16"));

		m_text.push_back(Quad::CreateButton("", 0.775f, 0.64f, 0.76f, 0.1f));
		m_text[TextOrder::SliderBarEffects]->setUnpressedTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarEffects]->setPressedTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarEffects]->setHoverTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarEffects]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		m_text[TextOrder::SliderBarEffects]->setFont(FontHandler::getFont("consolas16"));

		m_text.push_back(Quad::CreateButton("", 0.775f, 0.49f, 0.76f, 0.1f));
		m_text[TextOrder::SliderBarAmbient]->setUnpressedTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarAmbient]->setPressedTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarAmbient]->setHoverTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarAmbient]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		m_text[TextOrder::SliderBarAmbient]->setFont(FontHandler::getFont("consolas16"));

		m_text.push_back(Quad::CreateButton("", 0.775f, 0.34f, 0.76f, 0.1f));
		m_text[TextOrder::SliderBarMusic]->setUnpressedTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarMusic]->setPressedTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarMusic]->setHoverTexture("gui_slider_slide");
		m_text[TextOrder::SliderBarMusic]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		m_text[TextOrder::SliderBarMusic]->setFont(FontHandler::getFont("consolas16"));


		m_text.push_back(Quad::CreateButton("Field of View:", 0.2f, 0.86f, 0.73f, 0.12f));
		m_text[TextOrder::SliderFov]->setUnpressedTexture("gui_transparent_pixel");
		m_text[TextOrder::SliderFov]->setPressedTexture("gui_pressed_pixel");
		m_text[TextOrder::SliderFov]->setHoverTexture("gui_hover_pixel");
		m_text[TextOrder::SliderFov]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		m_text[TextOrder::SliderFov]->setFont(FontHandler::getFont("consolas16"));
		m_text[TextOrder::SliderFov]->setString("Field of View: " + std::to_string(m_fov));


		float xPos = ((((float)m_fov - (float)MIN_MAX_FOV.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_FOV.y - (float)MIN_MAX_FOV.x) + MIN_MAX_SLIDE_GENERAL.x);

		m_buttonsOptions.push_back(Quad::CreateButton("", xPos, 0.79f, 0.04f, 0.10f));
		m_buttonsOptions[ButtonOrderOptions::SliderFovButton]->setUnpressedTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderFovButton]->setPressedTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderFovButton]->setHoverTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderFovButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

		m_text.push_back(Quad::CreateButton("X-Axis: ", 0.2f, 0.71f, 0.73f, 0.12f));
		m_text[TextOrder::SliderSensitivityX]->setUnpressedTexture("gui_transparent_pixel");
		m_text[TextOrder::SliderSensitivityX]->setPressedTexture("gui_pressed_pixel");
		m_text[TextOrder::SliderSensitivityX]->setHoverTexture("gui_hover_pixel");
		m_text[TextOrder::SliderSensitivityX]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		m_text[TextOrder::SliderSensitivityX]->setFont(FontHandler::getFont("consolas16"));
		m_text[TextOrder::SliderSensitivityX]->setString("X-Axis: " + std::to_string(m_sens.x));

		xPos = (((float)m_sens.x - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE_GENERAL.x;
		m_buttonsOptions.push_back(Quad::CreateButton("", xPos, 0.64f, 0.04f, 0.10f));
		m_buttonsOptions[ButtonOrderOptions::SliderSensXButton]->setUnpressedTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderSensXButton]->setPressedTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderSensXButton]->setHoverTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderSensXButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

		m_text.push_back(Quad::CreateButton("Y-Axis: ", 0.2f, 0.56f, 0.73f, 0.12f));
		m_text[TextOrder::SliderSensitivityY]->setUnpressedTexture("gui_transparent_pixel");
		m_text[TextOrder::SliderSensitivityY]->setPressedTexture("gui_pressed_pixel");
		m_text[TextOrder::SliderSensitivityY]->setHoverTexture("gui_hover_pixel");
		m_text[TextOrder::SliderSensitivityY]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		m_text[TextOrder::SliderSensitivityY]->setFont(FontHandler::getFont("consolas16"));
		m_text[TextOrder::SliderSensitivityY]->setString("Y-Axis: " + std::to_string(m_sens.y));

		xPos = (((float)m_sens.y - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE_GENERAL.x;
		m_buttonsOptions.push_back(Quad::CreateButton("", xPos, 0.49f, 0.04f, 0.10f));
		m_buttonsOptions[ButtonOrderOptions::SliderSensYButton]->setUnpressedTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderSensYButton]->setPressedTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderSensYButton]->setHoverTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderSensYButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));


		m_text.push_back(Quad::CreateButton("Master Volume", 0.77f, 0.86f, 0.73f, 0.12f));
		m_text[TextOrder::SliderMaster]->setUnpressedTexture("gui_transparent_pixel");
		m_text[TextOrder::SliderMaster]->setPressedTexture("gui_pressed_pixel");
		m_text[TextOrder::SliderMaster]->setHoverTexture("gui_hover_pixel");
		m_text[TextOrder::SliderMaster]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		m_text[TextOrder::SliderMaster]->setFont(FontHandler::getFont("consolas16"));
		m_text[TextOrder::SliderMaster]->setString("Master Volume: " + std::to_string(m_master));

		xPos = ((((float)m_master - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x);

		m_buttonsOptions.push_back(Quad::CreateButton("", xPos, 0.79f, 0.04f, 0.10f));
		m_buttonsOptions[ButtonOrderOptions::SliderMasterButton]->setUnpressedTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderMasterButton]->setPressedTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderMasterButton]->setHoverTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderMasterButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

		m_text.push_back(Quad::CreateButton("Effects Volume", 0.77f, 0.71f, 0.73f, 0.12f));
		m_text[TextOrder::SliderEffects]->setUnpressedTexture("gui_transparent_pixel");
		m_text[TextOrder::SliderEffects]->setPressedTexture("gui_pressed_pixel");
		m_text[TextOrder::SliderEffects]->setHoverTexture("gui_hover_pixel");
		m_text[TextOrder::SliderEffects]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		m_text[TextOrder::SliderEffects]->setFont(FontHandler::getFont("consolas16"));
		m_text[TextOrder::SliderEffects]->setString("Effects Volume: " + std::to_string(m_effects));

		xPos = (((float)m_effects - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
		m_buttonsOptions.push_back(Quad::CreateButton("", xPos, 0.64f, 0.04f, 0.10f));
		m_buttonsOptions[ButtonOrderOptions::SliderEffectsButton]->setUnpressedTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderEffectsButton]->setPressedTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderEffectsButton]->setHoverTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderEffectsButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

		m_text.push_back(Quad::CreateButton("Ambient Volume: ", 0.77f, 0.56f, 0.73f, 0.12f));
		m_text[TextOrder::SliderAmbient]->setUnpressedTexture("gui_transparent_pixel");
		m_text[TextOrder::SliderAmbient]->setPressedTexture("gui_pressed_pixel");
		m_text[TextOrder::SliderAmbient]->setHoverTexture("gui_hover_pixel");
		m_text[TextOrder::SliderAmbient]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		m_text[TextOrder::SliderAmbient]->setFont(FontHandler::getFont("consolas16"));
		m_text[TextOrder::SliderAmbient]->setString("Ambient Volume: " + std::to_string(m_ambient));

		xPos = (((float)m_ambient - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
		m_buttonsOptions.push_back(Quad::CreateButton("", xPos, 0.49f, 0.04f, 0.10f));
		m_buttonsOptions[ButtonOrderOptions::SliderAmbientButton]->setUnpressedTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderAmbientButton]->setPressedTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderAmbientButton]->setHoverTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderAmbientButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

		m_text.push_back(Quad::CreateButton("Music Volume: ", 0.77f, 0.41f, 0.73f, 0.12f));
		m_text[TextOrder::SliderMusic]->setUnpressedTexture("gui_transparent_pixel");
		m_text[TextOrder::SliderMusic]->setPressedTexture("gui_pressed_pixel");
		m_text[TextOrder::SliderMusic]->setHoverTexture("gui_hover_pixel");
		m_text[TextOrder::SliderMusic]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		m_text[TextOrder::SliderMusic]->setFont(FontHandler::getFont("consolas16"));
		m_text[TextOrder::SliderMusic]->setString("Music Volume: " + std::to_string(m_music));

		xPos = (((float)m_music - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
		m_buttonsOptions.push_back(Quad::CreateButton("", xPos, 0.34f, 0.04f, 0.10f));
		m_buttonsOptions[ButtonOrderOptions::SliderMusicButton]->setUnpressedTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderMusicButton]->setPressedTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderMusicButton]->setHoverTexture("gui_slider_button");
		m_buttonsOptions[ButtonOrderOptions::SliderMusicButton]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));

		m_buttonsOptions.push_back(Quad::CreateButton("Back", 0.5f, 0.15f, 0.4f, 0.15f));
		m_buttonsOptions[ButtonOrderOptions::Back]->setUnpressedTexture("gui_transparent_pixel");
		m_buttonsOptions[ButtonOrderOptions::Back]->setPressedTexture("gui_pressed_pixel");
		m_buttonsOptions[ButtonOrderOptions::Back]->setHoverTexture("gui_hover_pixel");
		m_buttonsOptions[ButtonOrderOptions::Back]->setFont(FontHandler::getFont("consolas32"));
		m_buttonsOptions[ButtonOrderOptions::Back]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	}
	//Main buttons
	{
		m_buttonsMain.push_back(Quad::CreateButton("Back to Game", 0.5f, 0.5f, 0.5f, 0.2f));
		m_buttonsMain[ButtonOrderMain::ReturnButton]->setUnpressedTexture("gui_transparent_pixel");
		m_buttonsMain[ButtonOrderMain::ReturnButton]->setPressedTexture("gui_pressed_pixel");
		m_buttonsMain[ButtonOrderMain::ReturnButton]->setHoverTexture("gui_hover_pixel");
		m_buttonsMain[ButtonOrderMain::ReturnButton]->setTextColor(Colors::White);
		m_buttonsMain[ButtonOrderMain::ReturnButton]->setFont(FontHandler::getFont("consolas32"));

		m_buttonsMain.push_back(Quad::CreateButton("Options", 0.5f, 0.4f, 0.5f, 0.2f));
		m_buttonsMain[ButtonOrderMain::Options]->setUnpressedTexture("gui_transparent_pixel");
		m_buttonsMain[ButtonOrderMain::Options]->setPressedTexture("gui_pressed_pixel");
		m_buttonsMain[ButtonOrderMain::Options]->setHoverTexture("gui_hover_pixel");
		m_buttonsMain[ButtonOrderMain::Options]->setTextColor(Colors::White);
		m_buttonsMain[ButtonOrderMain::Options]->setFont(FontHandler::getFont("consolas32"));

		m_buttonsMain.push_back(Quad::CreateButton("Quit to Menu", 0.5f, 0.25f, 0.5f, 0.2f));
		m_buttonsMain[ButtonOrderMain::MainMenuButton]->setUnpressedTexture("gui_transparent_pixel");
		m_buttonsMain[ButtonOrderMain::MainMenuButton]->setPressedTexture("gui_pressed_pixel");
		m_buttonsMain[ButtonOrderMain::MainMenuButton]->setHoverTexture("gui_hover_pixel");
		m_buttonsMain[ButtonOrderMain::MainMenuButton]->setTextColor(Colors::White);
		m_buttonsMain[ButtonOrderMain::MainMenuButton]->setFont(FontHandler::getFont("consolas32"));
	}
	//Fullscreen semi-transparent overlay
	{
		this->m_fullscreenOverlay = new Quad();
		this->m_fullscreenOverlay->init();
		this->m_fullscreenOverlay->setPivotPoint(Quad::PivotPoint::center);
		this->m_fullscreenOverlay->setPosition(0.5f, 0.5f);
		this->m_fullscreenOverlay->setScale(2.0f, 2.0f);
		this->m_fullscreenOverlay->setUnpressedTexture("pause_menu_background");
		this->m_fullscreenOverlay->setPressedTexture("pause_menu_background");
		this->m_fullscreenOverlay->setHoverTexture("pause_menu_background");
	}
}

void PauseMenu::_handleGamePadInput(double dt)
{
	static const double MAX_TIME = 0.15;
	static double timer = 0.0f;
	timer += dt;

	int dir = Direction::None;

	if (Input::isUsingGamepad())
	{
		bool dpadUp		= GamePadHandler::IsUpDpadPressed();
		bool dpadDown	= GamePadHandler::IsDownDpadPressed();
		bool dpadRight	= GamePadHandler::IsRightDpadPressed();
		bool dpadLeft		= GamePadHandler::IsLeftDpadPressed();

		bool lStickUp		= GamePadHandler::GetLeftStickYPosition() > 0.0f;
		bool lStickDown	= GamePadHandler::GetLeftStickYPosition() < 0.0f;
		bool lStickRight	= GamePadHandler::GetLeftStickXPosition() > 0.0f;
		bool lStickLeft		= GamePadHandler::GetLeftStickXPosition() < 0.0f;

		bool ready = timer > MAX_TIME;

		if (ready && (dpadUp || lStickUp))
		{
			m_liu = Gamepad;
			dir = Direction::Up;
			timer = 0.0;
			m_sliderPressed = false;
		}
		else if (ready && (dpadDown || lStickDown))
		{
			m_liu = Gamepad;
			dir = Direction::Down;
			timer = 0.0;
			m_sliderPressed = false;
		}
		else if (ready && (dpadRight || lStickRight))
		{
			m_liu = Gamepad;
			dir = Direction::Right;
			timer = 0.0;
			m_sliderPressed = false;
		}
		else if (ready && (dpadLeft || lStickLeft))
		{
			m_liu = Gamepad;
			dir = Direction::Left;
			timer = 0.0;
			m_sliderPressed = false;
		}
		
		if (m_inOptions)
			_handleInputInOptions(dir);
		else
			_handleInputInMain(dir);

		if (GamePadHandler::IsAReleased())
		{
			_onButtonUsed();
		}
	}
}

void PauseMenu::_handleInputInOptions(int dir)
{
	if (dir == None)
		return;

	switch (dir)
	{
	case Up:
		_inOptionsOnUp();
		break;
	case Down:
		_inOptionsOnDown();
		break;
	case Right:
		_inOptionsOnRight();
		break;
	case Left:
		_inOptionsOnLeft();
		break;
	}
}

void PauseMenu::_inOptionsOnUp()
{
	//this button currently is locked in
	if (m_buttonsOptions[m_currentButton]->getState() == ButtonStates::Pressed)
		return;
	switch (m_currentButton)
	{
	case SliderFovButton:
		_onButtonSwap(Back);
		break;
	case SliderSensXButton:
		_onButtonSwap(SliderFovButton);
		break;
	case SliderSensYButton:
		_onButtonSwap(SliderSensXButton);
		break;
	case SliderMasterButton:
		_onButtonSwap(Back);
		break;
	case SliderEffectsButton:
		_onButtonSwap(SliderMasterButton);
		break;
	case SliderAmbientButton:
		_onButtonSwap(SliderEffectsButton);
		break;
	case SliderMusicButton:
		_onButtonSwap(SliderAmbientButton);
		break;
	case Back:
		_onButtonSwap(SliderMusicButton);
		break;
	}
}

void PauseMenu::_inOptionsOnDown()
{
	//this button currently is locked in
	if (m_buttonsOptions[m_currentButton]->getState() == ButtonStates::Pressed)
		return;
	switch (m_currentButton)
	{
	case SliderFovButton:
		_onButtonSwap(SliderSensXButton);
		break;
	case SliderSensXButton:
		_onButtonSwap(SliderSensYButton);
		break;
	case SliderSensYButton:
		_onButtonSwap(Back);
		break;
	case SliderMasterButton:
		_onButtonSwap(SliderEffectsButton);
		break;
	case SliderEffectsButton:
		_onButtonSwap(SliderAmbientButton);
		break;
	case SliderAmbientButton:
		_onButtonSwap(SliderMusicButton);
		break;
	case SliderMusicButton:
		_onButtonSwap(Back);
		break;
	case Back:
		_onButtonSwap(SliderMasterButton);
		break;
	}
}

void PauseMenu::_inOptionsOnRight()
{
	//this button currently is locked in
	if (m_buttonsOptions[m_currentButton]->getState() == ButtonStates::Pressed)
		return;
	switch (m_currentButton)
	{
	case SliderFovButton:
		_onButtonSwap(SliderMasterButton);
		break;
	case SliderSensXButton:
		_onButtonSwap(SliderEffectsButton);
		break;
	case SliderSensYButton:
		_onButtonSwap(SliderAmbientButton);
		break;
	case SliderMasterButton:
		_onButtonSwap(SliderFovButton);
		break;
	case SliderEffectsButton:
		_onButtonSwap(SliderSensXButton);
		break;
	case SliderAmbientButton:
		_onButtonSwap(SliderSensYButton);
		break;
	case SliderMusicButton:
		
		break;
	case Back:
		_onButtonSwap(SliderMusicButton);
		break;
	}
}

void PauseMenu::_inOptionsOnLeft()
{
	//this button currently is locked in
	if (m_buttonsOptions[m_currentButton]->getState() == ButtonStates::Pressed)
		return;
	switch (m_currentButton)
	{
	case SliderFovButton:
		_onButtonSwap(SliderMasterButton);
		break;
	case SliderSensXButton:
		_onButtonSwap(SliderEffectsButton);
		break;
	case SliderSensYButton:
		_onButtonSwap(SliderAmbientButton);
		break;
	case SliderMasterButton:
		_onButtonSwap(SliderFovButton);
		break;
	case SliderEffectsButton:
		_onButtonSwap(SliderSensXButton);
		break;
	case SliderAmbientButton:
		_onButtonSwap(SliderSensYButton);
		break;
	case SliderMusicButton:

		break;
	case Back:
		_onButtonSwap(SliderSensYButton);
		break;
	}
}

void PauseMenu::_handleInputInMain(int dir)
{
	if (dir == Left || dir == Right || dir == None)
		return;
	//nested switches are great Kappa
	switch (dir)
	{
	case Up:
		_inMainOnUp();
		break;
	case Down:
		_inMainOnDown();
		break;
	}
}

void PauseMenu::_inMainOnUp()
{
	switch (m_currentButton)
	{
	case ReturnButton:
		_onButtonSwap(MainMenuButton);
		break;
	case Options:
		_onButtonSwap(ReturnButton);
		break;
	case MainMenuButton:
		_onButtonSwap(Options);
		break;
	}
}

void PauseMenu::_inMainOnDown()
{
	switch (m_currentButton)
	{
	case ReturnButton:
		_onButtonSwap(Options);
		break;
	case Options:
		_onButtonSwap(MainMenuButton);
		break;
	case MainMenuButton:
		_onButtonSwap(ReturnButton);
		break;
	}
}

void PauseMenu::_onButtonUsed()
{
	if (m_inOptions)
	{
		if (m_buttonsOptions[m_currentButton]->getState() == ButtonStates::Pressed)
		{
			m_buttonsOptions[m_currentButton]->setState(ButtonStates::Hover);
			m_buttonPressed = false;
		}
		else if (m_buttonsOptions[m_currentButton]->isSelected())
		{
			m_buttonsOptions[m_currentButton]->setState(ButtonStates::Pressed);
			m_buttonPressed = true;
		}
	}
	else
	{
		if (m_buttonsMain[m_currentButton]->isSelected())
		{
			m_buttonsMain[m_currentButton]->setState(ButtonStates::Pressed);
			m_buttonPressed = true;
		}
	}
}

void PauseMenu::_onButtonSwap(int newButton)
{
	if (m_inOptions)
	{
		m_buttonsOptions[m_currentButton]->setState(ButtonStates::Normal);
		m_buttonsOptions[m_currentButton]->Select(false);
		m_currentButton = newButton;
		m_buttonsOptions[m_currentButton]->setState(ButtonStates::Hover);
		m_buttonsOptions[m_currentButton]->Select(true);
	}
	else
	{
		m_buttonsMain[m_currentButton]->setState(ButtonStates::Normal);
		m_buttonsMain[m_currentButton]->Select(false);
		m_currentButton = newButton;
		m_buttonsMain[m_currentButton]->setState(ButtonStates::Hover);
		m_buttonsMain[m_currentButton]->Select(true);
	}
}

void PauseMenu::_updateOptionsText()
{
	m_text[SliderFov]->setState(m_buttonsOptions[SliderFovButton]->getState());
	m_text[SliderSensitivityX]->setState(m_buttonsOptions[SliderSensXButton]->getState());
	m_text[SliderSensitivityY]->setState(m_buttonsOptions[SliderSensYButton]->getState());
	m_text[SliderMaster]->setState(m_buttonsOptions[SliderMasterButton]->getState());
	m_text[SliderEffects]->setState(m_buttonsOptions[SliderEffectsButton]->getState());
	m_text[SliderAmbient]->setState(m_buttonsOptions[SliderAmbientButton]->getState());
	m_text[SliderMusic]->setState(m_buttonsOptions[SliderMusicButton]->getState());
}

void PauseMenu::_handleKeyboardInput(double dt)
{
	static const double MAX_TIME = 0.15;
	static double timer = 0.0f;
	timer += dt;

	int dir = Direction::None;

	bool up		= InputHandler::isKeyPressed(InputHandler::Up);
	bool down	= InputHandler::isKeyPressed(InputHandler::Down);
	bool right		= InputHandler::isKeyPressed(InputHandler::Right);
	bool left		= InputHandler::isKeyPressed(InputHandler::Left);

	bool ready = timer > MAX_TIME;

	if (ready && up)
	{
		m_liu = Keyboard;
		dir = Direction::Up;
		timer = 0.0;
		m_sliderPressed = false;
	}
	else if (ready && down)
	{
		m_liu = Keyboard;
		dir = Direction::Down;
		timer = 0.0;
		m_sliderPressed = false;
	}
	else if (ready && right)
	{
		m_liu = Keyboard;
		dir = Direction::Right;
		timer = 0.0;
		m_sliderPressed = false;
	}
	else if (ready && left)
	{
		m_liu = Keyboard;
		dir = Direction::Left;
		timer = 0.0;
		m_sliderPressed = false;
	}

	if (m_inOptions)
		_handleInputInOptions(dir);
	else
		_handleInputInMain(dir);

	if (InputHandler::wasKeyPressed(InputHandler::Return))
	{
		_onButtonUsed();
	}
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

		if (m_inOptions)
			_handleMouseInOptions(mousePos);
		else
			_handleMouseInMain(mousePos);
	}
	return m_mouseMoved;
}

void PauseMenu::_handleMouseInOptions(DirectX::XMFLOAT2 & mousePos)
{
	for (size_t i = 0; i < m_buttonsOptions.size() && !m_sliderPressed; i++)
	{
		//set this button to current and on hover state
		if (m_buttonsOptions[i]->isReleased(mousePos))
		{
			m_buttonPressed = true;
			break;
		}
		else if (m_buttonsOptions[i]->isPressed(mousePos))
		{
			m_currentButton = i;
			ButtonOrderOptions type = (ButtonOrderOptions)i;
			if (type == SliderFovButton || type == SliderSensXButton || type == SliderSensYButton || type == SliderAmbientButton
				|| type == SliderEffectsButton || type == SliderMasterButton || type == SliderMusicButton)
			{
				m_sliderPressed = true;
				m_buttonsOptions[i]->Select(true);
			}
			break;
		}
		else if (m_buttonsOptions[i]->Inside(mousePos))
		{
			if (i != m_currentButton)
			{
				m_buttonsOptions[m_currentButton]->setState(ButtonStates::Normal);
				m_buttonsOptions[m_currentButton]->Select(false);
				m_currentButton = i;
				m_buttonsOptions[m_currentButton]->setState(ButtonStates::Hover);
				m_buttonsOptions[m_currentButton]->Select(true);
			}
		}
		else
			m_buttonsOptions[i]->Select(false);
	}
}

void PauseMenu::_handleMouseInMain(DirectX::XMFLOAT2 & mousePos)
{
	for (size_t i = 0; i < m_buttonsMain.size(); i++)
	{
		if (m_buttonsMain[i]->isReleased(mousePos))
		{
			m_currentButton = i;
			m_buttonPressed = true;
			break;
		}
		else if (m_buttonsMain[i]->Inside(mousePos))
		{
			if (i != m_currentButton)
			{
				m_buttonsMain[m_currentButton]->setState(ButtonStates::Normal);
				m_buttonsMain[m_currentButton]->Select(false);
				m_currentButton = i;
				m_buttonsMain[m_currentButton]->setState(ButtonStates::Hover);
				m_buttonsMain[m_currentButton]->Select(true);
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

void PauseMenu::_updateMain()
{
	if (m_buttonPressed)
	{
		switch ((ButtonOrderMain)m_currentButton)
		{
		case ReturnButton:
			m_exitPause = true;
			m_buttonPressed = false;
			break;
		case Options:
			m_inOptions = true;
			_onButtonSwap(Back);
			m_buttonPressed = false;
			m_sliderPressed = false;
			break;
		case MainMenuButton:
			m_mainMenuPressed = true;
			m_buttonPressed = false;
			break;
		}
	}
}

void PauseMenu::_updateOptions(float deltaTime, Camera * camera)
{
	if (m_sliderPressed || m_buttonPressed)
	{
		switch ((ButtonOrderOptions)m_currentButton)
		{
		case SliderFovButton:
			_updateFOV(deltaTime, camera);
			break;
		case SliderSensXButton:
			_updateSenseX(deltaTime);
			break;
		case SliderSensYButton:
			_updateSenseY(deltaTime);
			break;
		case SliderMasterButton:
			_updateMasterAudio();
			break;
		case SliderEffectsButton:
			_updateEffectAudio();
			break;
		case SliderAmbientButton:
			_updateAmbientAudio();
			break;
		case SliderMusicButton:
			_updateMusicAudio();
			break;
		case Back:
			_updateBackToMain();
			break;
		}
	}
}

void PauseMenu::_updateFOV(float deltaTime, Camera * camera)
{
	switch (m_liu)
	{
	case Mouse:
		_slideGeneral();
		m_fov = (((m_buttonsOptions[m_currentButton]->getPosition().x - MIN_MAX_SLIDE_GENERAL.x) * ((float)MIN_MAX_FOV.y - (float)MIN_MAX_FOV.x)) / (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) + (float)MIN_MAX_FOV.x;
		break;
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

		{
			float pos = (((float)m_fov - (float)MIN_MAX_FOV.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_FOV.y - (float)MIN_MAX_FOV.x) + MIN_MAX_SLIDE_GENERAL.x;
			m_buttonsOptions[ButtonOrderOptions::SliderFovButton]->setPosition(pos, m_buttonsOptions[ButtonOrderOptions::SliderFovButton]->getPosition().y);
		}
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
		{
			float pos = (((float)m_fov - (float)MIN_MAX_FOV.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_FOV.y - (float)MIN_MAX_FOV.x) + MIN_MAX_SLIDE_GENERAL.x;
			m_buttonsOptions[ButtonOrderOptions::SliderFovButton]->setPosition(pos, m_buttonsOptions[ButtonOrderOptions::SliderFovButton]->getPosition().y);
		}
		break;
	}

	camera->setFOV(DirectX::XMConvertToRadians(m_fov));
	m_text[TextOrder::SliderFov]->setString("Field of View: " + std::to_string(m_fov));
}

void PauseMenu::_updateSenseX(float deltaTime)
{
	switch (m_liu)
	{
	case Mouse:
		_slideGeneral();
		m_sens.x = (((m_buttonsOptions[m_currentButton]->getPosition().x - MIN_MAX_SLIDE_GENERAL.x) * ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x)) / (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) + (float)MIN_MAX_SENSITIVITY.x;
		break;
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
		{
			float pos = (((float)m_sens.x - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE_GENERAL.x;
			m_buttonsOptions[ButtonOrderOptions::SliderSensXButton]->setPosition(pos, m_buttonsOptions[ButtonOrderOptions::SliderSensXButton]->getPosition().y);
		}
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

		{
			float pos = (((float)m_sens.x - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE_GENERAL.x;
			m_buttonsOptions[ButtonOrderOptions::SliderSensXButton]->setPosition(pos, m_buttonsOptions[ButtonOrderOptions::SliderSensXButton]->getPosition().y);
		}
		break;
	}
	
	m_text[TextOrder::SliderSensitivityX]->setString("X-Axis: " + std::to_string(m_sens.x));
}

void PauseMenu::_updateSenseY(float deltaTime)
{
	switch (m_liu)
	{
	case Mouse:
		_slideGeneral();
		m_sens.y = (((m_buttonsOptions[m_currentButton]->getPosition().x - MIN_MAX_SLIDE_GENERAL.x) * ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x)) / (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) + (float)MIN_MAX_SENSITIVITY.x;
		break;
	case Gamepad:
		m_stickTimerY += deltaTime;
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

		{
			float pos = (((float)m_sens.y - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE_GENERAL.x;
			m_buttonsOptions[ButtonOrderOptions::SliderSensYButton]->setPosition(pos, m_buttonsOptions[ButtonOrderOptions::SliderSensYButton]->getPosition().y);
		}
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

		{
			float pos = (((float)m_sens.y - (float)MIN_MAX_SENSITIVITY.x) * (MIN_MAX_SLIDE_GENERAL.y - MIN_MAX_SLIDE_GENERAL.x)) / ((float)MIN_MAX_SENSITIVITY.y - (float)MIN_MAX_SENSITIVITY.x) + MIN_MAX_SLIDE_GENERAL.x;
			m_buttonsOptions[ButtonOrderOptions::SliderSensYButton]->setPosition(pos, m_buttonsOptions[ButtonOrderOptions::SliderSensYButton]->getPosition().y);
		}
		break;

	}
	m_text[TextOrder::SliderSensitivityY]->setString("Y-Axis: " + std::to_string(m_sens.y));
}

void PauseMenu::_updateMasterAudio()
{
	switch (m_liu)
	{
	case Mouse:
		_slideSound();
		m_master = (((m_buttonsOptions[m_currentButton]->getPosition().x - MIN_MAX_SLIDE_SOUND.x) * ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x)) / (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) + (float)MIN_MAX_SOUND.x;
		break;
	case Gamepad:
		if (GamePadHandler::IsRightDpadPressed() || GamePadHandler::GetLeftStickXPosition() > 0)
			m_master++;
		if (GamePadHandler::IsLeftDpadPressed() || GamePadHandler::GetLeftStickXPosition() < 0)
			m_master--;

		if (m_master < MIN_MAX_SOUND.x)
			m_master = MIN_MAX_SOUND.x;
		if (m_master > MIN_MAX_SOUND.y)
			m_master = MIN_MAX_SOUND.y;

		{
			float pos = (((float)m_master - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
			m_buttonsOptions[ButtonOrderOptions::SliderMasterButton]->setPosition(pos, m_buttonsOptions[ButtonOrderOptions::SliderMasterButton]->getPosition().y);
		}
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

		{
			float pos = (((float)m_master - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
			m_buttonsOptions[ButtonOrderOptions::SliderMasterButton]->setPosition(pos, m_buttonsOptions[ButtonOrderOptions::SliderMasterButton]->getPosition().y);
		}
		break;
	}

	m_text[TextOrder::SliderMaster]->setString("Master Volume: " + std::to_string(m_master));
	AudioEngine::SetMasterVolume((float)m_master / 100.0f);
}

void PauseMenu::_updateEffectAudio()
{
	switch (m_liu)
	{
	case Mouse:
		_slideSound();
		m_effects = (((m_buttonsOptions[m_currentButton]->getPosition().x - MIN_MAX_SLIDE_SOUND.x) * ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x)) / (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) + (float)MIN_MAX_SOUND.x;
		break;
	case Gamepad:
		if (GamePadHandler::IsRightDpadPressed() || GamePadHandler::GetLeftStickXPosition() > 0)
			m_effects++;
		if (GamePadHandler::IsLeftDpadPressed() || GamePadHandler::GetLeftStickXPosition() < 0)
			m_effects--;

		if (m_effects < MIN_MAX_SOUND.x)
			m_effects = MIN_MAX_SOUND.x;
		if (m_effects > MIN_MAX_SOUND.y)
			m_effects = MIN_MAX_SOUND.y;

		{
			float pos = (((float)m_effects - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
			m_buttonsOptions[ButtonOrderOptions::SliderEffectsButton]->setPosition(pos, m_buttonsOptions[ButtonOrderOptions::SliderEffectsButton]->getPosition().y);
		}
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

	{
		float pos = (((float)m_effects - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
		m_buttonsOptions[ButtonOrderOptions::SliderEffectsButton]->setPosition(pos, m_buttonsOptions[ButtonOrderOptions::SliderEffectsButton]->getPosition().y);
	}
		break;
	}

	m_text[TextOrder::SliderEffects]->setString("Effects Volume: " + std::to_string(m_effects));
	AudioEngine::SetEffectVolume((float)m_effects / 100.0f);
}

void PauseMenu::_updateAmbientAudio()
{
	switch (m_liu)
	{
	case Mouse:
		_slideSound();
		m_ambient = (((m_buttonsOptions[m_currentButton]->getPosition().x - MIN_MAX_SLIDE_SOUND.x) * ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x)) / (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) + (float)MIN_MAX_SOUND.x;
		break;
	case Gamepad:
		if (GamePadHandler::IsRightDpadPressed() || GamePadHandler::GetLeftStickXPosition() > 0)
			m_ambient++;
		if (GamePadHandler::IsLeftDpadPressed() || GamePadHandler::GetLeftStickXPosition() < 0)
			m_ambient--;

		if (m_ambient < MIN_MAX_SOUND.x)
			m_ambient = MIN_MAX_SOUND.x;
		if (m_ambient > MIN_MAX_SOUND.y)
			m_ambient = MIN_MAX_SOUND.y;

		{
			float pos = (((float)m_ambient - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
			m_buttonsOptions[ButtonOrderOptions::SliderAmbientButton]->setPosition(pos, m_buttonsOptions[ButtonOrderOptions::SliderAmbientButton]->getPosition().y);
		}
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

		{
			float pos = (((float)m_ambient - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
			m_buttonsOptions[ButtonOrderOptions::SliderAmbientButton]->setPosition(pos, m_buttonsOptions[ButtonOrderOptions::SliderAmbientButton]->getPosition().y);
		}
		break;
	}
	m_text[TextOrder::SliderAmbient]->setString("Ambient Volume: " + std::to_string(m_ambient));
	AudioEngine::SetAmbientVolume((float)m_ambient / 100.0f);
}

void PauseMenu::_updateMusicAudio()
{
	switch (m_liu)
	{
	case Mouse:
		_slideSound();
		m_music = (((m_buttonsOptions[m_currentButton]->getPosition().x - MIN_MAX_SLIDE_SOUND.x) * ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x)) / (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) + (float)MIN_MAX_SOUND.x;
		break;
	case Gamepad:
		if (GamePadHandler::IsRightDpadPressed() || GamePadHandler::GetLeftStickXPosition() > 0)
			m_music++;
		if (GamePadHandler::IsLeftDpadPressed() || GamePadHandler::GetLeftStickXPosition() < 0)
			m_music--;

		if (m_music < MIN_MAX_SOUND.x)
			m_music = MIN_MAX_SOUND.x;
		if (m_music > MIN_MAX_SOUND.y)
			m_music = MIN_MAX_SOUND.y;

		{
			float pos = (((float)m_music - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
			m_buttonsOptions[ButtonOrderOptions::SliderMusicButton]->setPosition(pos, m_buttonsOptions[ButtonOrderOptions::SliderMusicButton]->getPosition().y);
		}
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

		{
			float pos = (((float)m_music - (float)MIN_MAX_SOUND.x) * (MIN_MAX_SLIDE_SOUND.y - MIN_MAX_SLIDE_SOUND.x)) / ((float)MIN_MAX_SOUND.y - (float)MIN_MAX_SOUND.x) + MIN_MAX_SLIDE_SOUND.x;
			m_buttonsOptions[ButtonOrderOptions::SliderMusicButton]->setPosition(pos, m_buttonsOptions[ButtonOrderOptions::SliderMusicButton]->getPosition().y);
		}
		break;
	}

	m_text[TextOrder::SliderMusic]->setString("Music Volume: " + std::to_string(m_music));
	AudioEngine::SetMusicVolume((float)m_music / 100.0f);
}

void PauseMenu::_updateBackToMain()
{
	_WriteSettingsToFile();
	Input::ReadSettingsFile();
	m_currentButton = ButtonOrderMain::Options;
	m_inOptions = false;
	_onButtonSwap(Options);
	m_buttonPressed = false;
}

const bool & PauseMenu::getExitPause() const
{
	return m_exitPause; 
}

const bool & PauseMenu::getMainMenuPressed() const
{
	return m_mainMenuPressed; 
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
				if (extension == L".DDS")
					Manager::g_textureManager.loadDDSTexture(stem, file.generic_wstring(), L"_256");
			}
		}
	}
}

void PauseMenu::unLoad()
{
	Manager::g_textureManager.UnloadGUITextures();
}
