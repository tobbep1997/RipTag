#include "RipTagPCH.h"
#include "../2DEngine/2D Engine/Quad/Components/HUDComponent.h"
#include "MainMenu.h"
#include "InputManager/XboxInput/GamePadHandler.h"

std::string RipSounds::g_music1;

MainMenu::MainMenu(RenderingManager * rm) : State(rm)
{

}

MainMenu::~MainMenu()
{
	unLoad(); // This is a special case because the MainMenu is on slot 0 in the stack
}
void MainMenu::Update(double deltaTime)
{
	if (!InputHandler::getShowCursor())
		InputHandler::setShowCursor(TRUE);

	if (!_handleMouseInput())
	{
		_handleGamePadInput(deltaTime);
		_handleKeyboardInput();
	}

	//Check if we have the current button pressed, if so determine which one and do the thing
	if (m_buttons[m_currentButton]->getState() == (unsigned int)ButtonStates::Pressed)
	{
		switch ((ButtonOrder)m_currentButton)
		{
		case ButtonOrder::Play:
#ifndef _DEPLOY
			_resetButtons();
			m_background->Release();
			delete m_background;
			m_background = nullptr;
			m_loadingScreen.removeGUI(m_buttons);
			m_loadingScreen.draw();
			this->pushNewState(new PlayState(this->p_renderingManager, nullptr, 0));
			m_music->stop();
#endif
			break; 
		case ButtonOrder::Lobby:
			_resetButtons();
			this->pushNewState(new LobbyState(this->p_renderingManager));
			//nothing to do here yet
			break;
		case ButtonOrder::Option:
			this->pushNewState(new OptionState(this->p_renderingManager));
			break;
		case ButtonOrder::Quit:
			InputHandler::CloseGame();
			PostQuitMessage(0);
			break;
		}
	}
	
}

void MainMenu::Draw()
{
	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);
	camera.setPosition(0, 0, -10);

	if(m_background)
	m_background->Draw();
	for (size_t i = 0; i < m_buttons.size(); i++)
		m_buttons[i]->Draw();
	p_renderingManager->Flush(camera);
}

void MainMenu::StopMusic()
{
	bool isPlaying = false;
	m_music->isPlaying(&isPlaying);
	if (!isPlaying)
		m_music->stop();
}

void MainMenu::_initButtons()
{
#ifdef _DEPLOY
	//play button
	this->m_buttons.push_back(Quad::CreateButton("Castle\nCaptives", 0.5f, 0.815f, 0.565f, 0.20f));
	this->m_buttons[ButtonOrder::Play]->setUnpressedTexture("gui_transparent_pixel");
	this->m_buttons[ButtonOrder::Play]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	this->m_buttons[ButtonOrder::Play]->setFont(FontHandler::getFont("consolas16"));
#else
	//play button
	this->m_buttons.push_back(Quad::CreateButton("Play Game", 0.5f, 0.815f, 0.565f, 0.20f));
	this->m_buttons[ButtonOrder::Play]->setUnpressedTexture("gui_transparent_pixel");
	this->m_buttons[ButtonOrder::Play]->setPressedTexture("gui_pressed_pixel");
	this->m_buttons[ButtonOrder::Play]->setHoverTexture("gui_hover_pixel");
	this->m_buttons[ButtonOrder::Play]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	this->m_buttons[ButtonOrder::Play]->setFont(FontHandler::getFont("consolas32"));

#endif

	//lobby button
	this->m_buttons.push_back(Quad::CreateButton("Lobby", 0.5f, 0.605f, 0.565f, 0.20f));
	this->m_buttons[ButtonOrder::Lobby]->setUnpressedTexture("gui_transparent_pixel");
	this->m_buttons[ButtonOrder::Lobby]->setPressedTexture("gui_pressed_pixel");
	this->m_buttons[ButtonOrder::Lobby]->setHoverTexture("gui_hover_pixel");
	this->m_buttons[ButtonOrder::Lobby]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	this->m_buttons[ButtonOrder::Lobby]->setFont(FontHandler::getFont("consolas32"));

	this->m_buttons.push_back(Quad::CreateButton("Options", 0.5f, 0.380f, 0.565f, 0.20f));
	this->m_buttons[ButtonOrder::Option]->setUnpressedTexture("gui_transparent_pixel");
	this->m_buttons[ButtonOrder::Option]->setPressedTexture("gui_pressed_pixel");
	this->m_buttons[ButtonOrder::Option]->setHoverTexture("gui_hover_pixel");
	this->m_buttons[ButtonOrder::Option]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	this->m_buttons[ButtonOrder::Option]->setFont(FontHandler::getFont("consolas32"));

	//Quit button
	this->m_buttons.push_back(Quad::CreateButton("Quit", 0.5f, 0.175f, 0.565f, 0.20f));
	this->m_buttons[ButtonOrder::Quit]->setUnpressedTexture("gui_transparent_pixel");
	this->m_buttons[ButtonOrder::Quit]->setPressedTexture("gui_pressed_pixel");
	this->m_buttons[ButtonOrder::Quit]->setHoverTexture("gui_hover_pixel");
	this->m_buttons[ButtonOrder::Quit]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	this->m_buttons[ButtonOrder::Quit]->setFont(FontHandler::getFont("consolas32"));

	this->m_background = new Quad();
	this->m_background->init();
	this->m_background->setPivotPoint(Quad::PivotPoint::center);
	this->m_background->setPosition(0.5f, 0.5f);
	this->m_background->setScale(2.0f, 2.0f);
	this->m_background->setUnpressedTexture("gui_main_menu");
	this->m_background->setPressedTexture("gui_main_menu");
	this->m_background->setHoverTexture("gui_main_menu");

	m_buttons.shrink_to_fit();

}

bool MainMenu::_handleMouseInput()
{
	static DirectX::XMFLOAT2 s_mouseLastFrame = { 0,0 };

	DirectX::XMFLOAT2 mousePos = InputHandler::getMousePosition();
	DirectX::XMINT2 windowSize = InputHandler::getWindowSize();

	bool returnIfTrue = true;
	if (fabs(s_mouseLastFrame.x - mousePos.x) > 0.9 || fabs(s_mouseLastFrame.y - mousePos.y) > 0.9 || InputHandler::isMouseLeftPressed())
	{
		returnIfTrue = false;
	}

	s_mouseLastFrame = mousePos;

	if (returnIfTrue)
		return false;

	mousePos.x /= windowSize.x;
	mousePos.y /= windowSize.y;


	for (size_t i = 0; i < m_buttons.size(); i++)
	{
#ifdef _DEPLOY
		if (i == 0)
			continue;
#endif
		if (m_buttons[i]->Inside(mousePos))
		{
			//set this button to current and on hover state
			m_currentButton = i;
			m_buttons[i]->Select(true);
			m_buttons[i]->setState(ButtonStates::Hover);
			//check if we released this button
			if (m_buttons[i]->isReleased(mousePos))
				m_buttons[i]->setState(ButtonStates::Pressed);
			//set all the other buttons to
			for (size_t j = 0; j < m_buttons.size(); j++)
			{
				if (i != j)
				{
					m_buttons[j]->Select(false);
					m_buttons[j]->setState(ButtonStates::Normal);
				}
			}
			break;
		}
	}
	return true;
}

void MainMenu::_handleGamePadInput(float deltaTime)
{
	if (Input::isUsingGamepad())
	{
		m_stickTimer += deltaTime; 

		if (GamePadHandler::IsUpDpadPressed())
		{
#ifdef _DEPLOY
			if (m_currentButton == 1)
				m_currentButton = ((unsigned int)ButtonOrder::Quit);
#else
			if (m_currentButton == 0)
				m_currentButton = (unsigned int)ButtonOrder::Quit;
#endif
			else
				m_currentButton--;
		}
		else if (GamePadHandler::IsDownDpadPressed())
		{
			m_currentButton++;
			m_currentButton = m_currentButton % ((unsigned int)ButtonOrder::Quit + 1);
#ifdef _DEPLOY
			if (m_currentButton == 0)
				m_currentButton++;
#endif
		}

		if (GamePadHandler::GetLeftStickYPosition() > 0 && m_stickTimer > 0.2f)
		{
			if (m_currentButton == 0)
				m_currentButton = (unsigned int)ButtonOrder::Quit;
			else
				m_currentButton--;

			m_stickTimer = 0; 
		}
		else if (GamePadHandler::GetLeftStickYPosition() < 0 && m_stickTimer > 0.2f)
		{
			m_currentButton++; 
			m_currentButton = m_currentButton % ((unsigned int)ButtonOrder::Quit + 1);
			m_stickTimer = 0; 
		}



		_updateSelectionStates();

		//Check for action input
		if (GamePadHandler::IsAPressed())
		{
			if (m_buttons[m_currentButton]->isSelected())
				this->m_buttons[m_currentButton]->setState(ButtonStates::Pressed);
		}
	}

}

void MainMenu::_handleKeyboardInput()
{
	if (InputHandler::wasKeyPressed(InputHandler::Up))
	{
#ifdef _DEPLOY
		if (m_currentButton == 1)
			m_currentButton = ((unsigned int)ButtonOrder::Quit);
#else
		if (m_currentButton == 0)
			m_currentButton = (unsigned int)ButtonOrder::Quit;
#endif
		else
			m_currentButton--;
	}
	else if (InputHandler::wasKeyPressed(InputHandler::Down))
	{
		m_currentButton++;
		m_currentButton = m_currentButton % ((unsigned int)ButtonOrder::Quit + 1);
#ifdef _DEPLOY
		if (m_currentButton == 0)
			m_currentButton++;
#endif
	}

	_updateSelectionStates();

	if (InputHandler::wasKeyPressed(InputHandler::Enter))
	{
		if (m_buttons[m_currentButton]->isSelected())
			this->m_buttons[m_currentButton]->setState(ButtonStates::Pressed);
	}
}

void MainMenu::_updateSelectionStates()
{
	//update the selection states
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

void MainMenu::_resetButtons()
{
	for (auto & button : m_buttons)
	{
		button->Select(false);
		button->setState(ButtonStates::Normal);
	}
	m_currentButton = (unsigned int)ButtonOrder::Play;
}

void MainMenu::Load()
{
	RipSounds::g_music1 = AudioEngine::LoadMusicSound("../Assets/Audio/Music/MySong2.ogg", true);
	m_music = AudioEngine::PlayMusic(RipSounds::g_music1);
	m_music->setVolume(0.3f);

	bool isPlaying = false;
	m_music->isPlaying(&isPlaying);
	if (!isPlaying)
		m_music = AudioEngine::PlayMusic(RipSounds::g_music1);
	m_music->setVolume(0.3f);


	this->LoadAllGuiElements();
	FontHandler::loadFont("consolas32");
	FontHandler::loadFont("consolas16");
	_initButtons();
	m_loadingScreen.Init();
#ifdef _DEPLOY
	m_currentButton = (unsigned int)ButtonOrder::Lobby;
#else
	m_currentButton = (unsigned int)ButtonOrder::Play;
#endif
	Manager::g_textureManager.loadTextures("LOADING");

	std::cout << "MainMenu Load" << std::endl;
}

void MainMenu::unLoad()
{
	for (size_t i = 0; i < m_buttons.size(); i++)
	{
		m_buttons[i]->Release();
		delete m_buttons[i];
	}
	m_buttons.clear();
	if (m_background)
	{
		m_background->Release();
		delete m_background;
		m_background = nullptr;
	}
	Manager::g_textureManager.UnloadAllTexture();
	Manager::g_textureManager.UnloadGUITextures();
	m_music->stop();

	//HUDComponent::removeHUD();

	AudioEngine::UnloadMusicSound(RipSounds::g_music1);


	std::cout << "MainMenu unLoad" << std::endl;
}

void MainMenu::LoadAllGuiElements()
{
	//Loop through GUI folder and inspect the files extensions
	std::string path = "../Assets/GUIFOLDER";
	for (auto & p : std::filesystem::directory_iterator(path))
	{
		if (p.is_regular_file())
		{
			auto file = p.path();
			if (file.has_filename() && file.has_extension())
			{
				std::wstring stem = file.stem().generic_wstring();
				std::wstring extension = file.extension().generic_wstring();
				if (extension == L".DDS" || extension == L".DDS")
					Manager::g_textureManager.loadGUITexture(stem, file.generic_wstring());
			}
		}
	}

		
}
