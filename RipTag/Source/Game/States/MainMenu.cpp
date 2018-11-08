#include "RipTagPCH.h"
#include "MainMenu.h"


MainMenu::MainMenu(RenderingManager * rm) : State(rm)
{
	Manager::g_textureManager.loadTextures("KOMBIN");
	Manager::g_textureManager.loadTextures("SPHERE");
	Manager::g_textureManager.loadTextures("PIRASRUM");
	Manager::g_textureManager.loadTextures("DAB");

	_initButtons();
	m_currentButton = (unsigned int)ButtonOrder::Play;
}

MainMenu::~MainMenu()
{
	for (size_t i = 0; i < m_buttons.size(); i++)
		delete m_buttons[i];
	m_buttons.clear();
}
#include "InputManager/XboxInput/GamePadHandler.h"
void MainMenu::Update(double deltaTime)
{
	if (!InputHandler::getShowCursor())
		InputHandler::setShowCursor(TRUE);

	_handleMouseInput();
	_handleGamePadInput();
	_handleKeyboardInput();

	//Check if we have the current button pressed, if so determine which one and do the thing
	if (m_buttons[m_currentButton]->getState() == (unsigned int)ButtonStates::Pressed)
	{
		switch ((ButtonOrder)m_currentButton)
		{
		case ButtonOrder::Play:
			this->pushNewState(new PlayState(this->p_renderingManager));
			break;
		case ButtonOrder::Lobby:
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

	for (size_t i = 0; i < m_buttons.size(); i++)
		m_buttons[i]->Draw();
	
	p_renderingManager->Flush(camera);
}

void MainMenu::_initButtons()
{
	//play button
	this->m_buttons.push_back(Quad::CreateButton("Play Game", 0.5f, 0.80f, 0.5f, 0.25f));
	this->m_buttons[ButtonOrder::Play]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	this->m_buttons[ButtonOrder::Play]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	this->m_buttons[ButtonOrder::Play]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	this->m_buttons[ButtonOrder::Play]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	this->m_buttons[ButtonOrder::Play]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));

	//lobby button
	this->m_buttons.push_back(Quad::CreateButton("Lobby", 0.5f, 0.6f, 0.5f, 0.25f));
	this->m_buttons[ButtonOrder::Lobby]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	this->m_buttons[ButtonOrder::Lobby]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	this->m_buttons[ButtonOrder::Lobby]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	this->m_buttons[ButtonOrder::Lobby]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	this->m_buttons[ButtonOrder::Lobby]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));

	this->m_buttons.push_back(Quad::CreateButton("Options", 0.5f, 0.4f, 0.5f, 0.25f));
	this->m_buttons[ButtonOrder::Option]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	this->m_buttons[ButtonOrder::Option]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	this->m_buttons[ButtonOrder::Option]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	this->m_buttons[ButtonOrder::Option]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	this->m_buttons[ButtonOrder::Option]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));

	//Quit button
	this->m_buttons.push_back(Quad::CreateButton("Quit", 0.5f, 0.20f, 0.5f, 0.25f));
	this->m_buttons[ButtonOrder::Quit]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	this->m_buttons[ButtonOrder::Quit]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	this->m_buttons[ButtonOrder::Quit]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	this->m_buttons[ButtonOrder::Quit]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	this->m_buttons[ButtonOrder::Quit]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
}

void MainMenu::_handleGamePadInput()
{
	if (Input::isUsingGamepad())
	{
		if (GamePadHandler::IsUpDpadPressed())
		{
			if (m_currentButton == 0)
				m_currentButton = (unsigned int)ButtonOrder::Quit;
			else
				m_currentButton--;
		}
		else if (GamePadHandler::IsDownDpadPressed())
		{
			m_currentButton++;
			m_currentButton = m_currentButton % ((unsigned int)ButtonOrder::Quit + 1);
		}
	}

	_updateSelectionStates();

	//Check for action input
	if (GamePadHandler::IsAPressed())
	{
		if (m_buttons[m_currentButton]->isSelected())
			this->m_buttons[m_currentButton]->setState(ButtonStates::Pressed);
	}
}

void MainMenu::_handleKeyboardInput()
{
	if (InputHandler::isKeyPressed(InputHandler::Up))
	{
		if (m_currentButton == 0)
			m_currentButton = (unsigned int)ButtonOrder::Quit;
		else
			m_currentButton--;
	}
	else if (InputHandler::isKeyPressed(InputHandler::Down))
	{
		m_currentButton++;
		m_currentButton = m_currentButton % ((unsigned int)ButtonOrder::Quit + 1);
	}

	_updateSelectionStates();

	if (InputHandler::isKeyPressed(InputHandler::Enter))
	{
		if (m_buttons[m_currentButton]->isSelected())
			this->m_buttons[m_currentButton]->setState(ButtonStates::Pressed);
	}
}

void MainMenu::_handleMouseInput()
{
	DirectX::XMFLOAT2 mousePos = InputHandler::getMousePosition();
	DirectX::XMINT2 windowSize = InputHandler::getWindowSize();

	mousePos.x /= windowSize.x;
	mousePos.y /= windowSize.y;

	for (size_t i = 0; i < m_buttons.size(); i++)
	{
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
