#include "RipTagPCH.h"
#include "LobbyState.h"


LobbyState::LobbyState(RenderingManager * rm) : State(rm)
{
	Manager::g_textureManager.loadTextures("KOMBIN");
	Manager::g_textureManager.loadTextures("SPHERE");
	Manager::g_textureManager.loadTextures("PIRASRUM");
	Manager::g_textureManager.loadTextures("DAB");

	_initButtons();
	m_currentButton = (unsigned int)ButtonOrderLobby::Host;
}


LobbyState::~LobbyState()
{
	for (auto &button : this->m_lobbyButtons)
		delete button;
	this->m_lobbyButtons.clear();
	for (auto & button : this->m_charSelectButtons)
		delete button;
	this->m_charSelectButtons.clear();
}

void LobbyState::Update(double deltaTime)
{
	if (!InputHandler::getShowCursor())
		InputHandler::setShowCursor(TRUE);

	_handleMouseInput();
	_handleGamePadInput();
	_handleKeyboardInput();

	if (!isHosting && !hasJoined)
	{
		if (m_lobbyButtons[m_currentButton]->getState() == (unsigned int)ButtonStates::Pressed)
		{
			switch ((ButtonOrderLobby)m_currentButton)
			{
			case ButtonOrderLobby::Host:
				isHosting = true;
				break;
			case ButtonOrderLobby::Join:
				hasJoined = true;
				break;
			case ButtonOrderLobby::Return:
				this->setKillState(true);
				break;
			}
		}
	}
	else
	{
		if (m_charSelectButtons[m_currentButton]->getState() == (unsigned int)ButtonStates::Pressed)
		{
			switch ((CharacterSelection)m_currentButton)
			{
			case CharacterSelection::CharOne:
				if (hasCharSelected)
				{
					hasCharSelected = false;
					selectedChar = 0;
				}
				else
				{
					hasCharSelected = true;
					selectedChar = 1;
				}
				break;
			case CharacterSelection::CharTwo:
				if (hasCharSelected)
				{
					hasCharSelected = false;
					selectedChar = 0;
				}
				else
				{
					hasCharSelected = true;
					selectedChar = 2;
				}
				break;
			case CharacterSelection::Ready:
				//ready
				break;
			case CharacterSelection::Back:
				isHosting = false;
				hasJoined = false;
				m_currentButton = (unsigned int)ButtonOrderLobby::Host;
				break;
			}
		}
	}
}

void LobbyState::Draw()
{
	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);
	camera.setPosition(0, 0, -10);

	if (!this->isHosting && !this->hasJoined)
	{
		for (auto &button : this->m_lobbyButtons)
			button->Draw();
		for (auto & listElement : this->m_hostList)
			listElement->Draw();
	}
	else
	{
		for (auto &button : this->m_charSelectButtons)
			button->Draw();
	}

	p_renderingManager->Flush(camera);
}

void LobbyState::_initButtons()
{
	//Lobby buttons
	{
		//Host button
		this->m_lobbyButtons.push_back(Quad::CreateButton("Host", 0.2f, 0.55f, 0.3f, 0.25f));
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
		//Join button
		this->m_lobbyButtons.push_back(Quad::CreateButton("Join", 0.2f, 0.35f, 0.3f, 0.25f));
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
		//Return button
		this->m_lobbyButtons.push_back(Quad::CreateButton("Return", 0.2f, 0.10f, 0.3f, 0.25f));
		this->m_lobbyButtons[ButtonOrderLobby::Return]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
		this->m_lobbyButtons[ButtonOrderLobby::Return]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
		this->m_lobbyButtons[ButtonOrderLobby::Return]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
		this->m_lobbyButtons[ButtonOrderLobby::Return]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_lobbyButtons[ButtonOrderLobby::Return]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
	}
	//Character selection buttons
	{
		//Character One
		this->m_charSelectButtons.push_back(Quad::CreateButton("Lejf", 0.3f, 0.55f, 0.3f, 0.25f));
		this->m_charSelectButtons[CharacterSelection::CharOne]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
		this->m_charSelectButtons[CharacterSelection::CharOne]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
		this->m_charSelectButtons[CharacterSelection::CharOne]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
		this->m_charSelectButtons[CharacterSelection::CharOne]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_charSelectButtons[CharacterSelection::CharOne]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
		//Character Two
		this->m_charSelectButtons.push_back(Quad::CreateButton("Billy", 0.7f, 0.55f, 0.3f, 0.25f));
		this->m_charSelectButtons[CharacterSelection::CharTwo]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
		this->m_charSelectButtons[CharacterSelection::CharTwo]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
		this->m_charSelectButtons[CharacterSelection::CharTwo]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
		this->m_charSelectButtons[CharacterSelection::CharTwo]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_charSelectButtons[CharacterSelection::CharTwo]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
		//Ready
		this->m_charSelectButtons.push_back(Quad::CreateButton("Ready", 0.5f, 0.25f, 0.3f, 0.25f));
		this->m_charSelectButtons[CharacterSelection::Ready]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
		this->m_charSelectButtons[CharacterSelection::Ready]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
		this->m_charSelectButtons[CharacterSelection::Ready]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
		this->m_charSelectButtons[CharacterSelection::Ready]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_charSelectButtons[CharacterSelection::Ready]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
		//Return
		this->m_charSelectButtons.push_back(Quad::CreateButton("Return", 0.85f, 0.25f, 0.3f, 0.25f));
		this->m_charSelectButtons[CharacterSelection::Back]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
		this->m_charSelectButtons[CharacterSelection::Back]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
		this->m_charSelectButtons[CharacterSelection::Back]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
		this->m_charSelectButtons[CharacterSelection::Back]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_charSelectButtons[CharacterSelection::Back]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
	}

}

void LobbyState::_handleGamePadInput()
{
	if (Input::isUsingGamepad())
	{
		if (GamePadHandler::IsUpDpadPressed())
		{
			if (!isHosting && !hasJoined)
			{
				if (m_currentButton == 0)
					m_currentButton = (unsigned int)ButtonOrderLobby::Return;
				else
					m_currentButton--;
			}
			else
			{
				if (m_currentButton == 0)
					m_currentButton = (unsigned int)CharacterSelection::Back;
				else
					m_currentButton--;
			}
		}
		else if (GamePadHandler::IsDownDpadPressed())
		{
			m_currentButton++;
			if (!isHosting && !hasJoined)
				m_currentButton = m_currentButton % ((unsigned int)ButtonOrderLobby::Return + 1);
			else
				m_currentButton = m_currentButton % ((unsigned int)CharacterSelection::Back + 1);
		}

		_updateSelectionStates();

		//Check for action input
		if (GamePadHandler::IsAPressed())
		{
			if (!isHosting && !hasJoined)
			{
				if (m_lobbyButtons[m_currentButton]->isSelected())
					this->m_lobbyButtons[m_currentButton]->setState(ButtonStates::Pressed);
			}
			else
			{
				if (m_charSelectButtons[m_currentButton]->isSelected())
					this->m_charSelectButtons[m_currentButton]->setState(ButtonStates::Pressed);
			}
		}

	}

	
}

void LobbyState::_handleKeyboardInput()
{
	if (InputHandler::isKeyReleased(InputHandler::Up))
	{
		if (!isHosting && !hasJoined)
		{
			if (m_currentButton == 0)
				m_currentButton = (unsigned int)ButtonOrderLobby::Return;
			else
				m_currentButton--;
		}
		else
		{
			if (m_currentButton == 0)
				m_currentButton = (unsigned int)CharacterSelection::Back;
			else
				m_currentButton--;
		}
	}
	else if (InputHandler::isKeyReleased(InputHandler::Down))
	{
		m_currentButton++;
		if (!isHosting && !hasJoined)
			m_currentButton = m_currentButton % ((unsigned int)ButtonOrderLobby::Return + 1);
		else
			m_currentButton = m_currentButton % ((unsigned int)CharacterSelection::Back + 1);
	}

	_updateSelectionStates();

	if (InputHandler::isKeyReleased(InputHandler::Enter))
	{
		if (!isHosting && !hasJoined)
		{
			if (m_lobbyButtons[m_currentButton]->isSelected())
				this->m_lobbyButtons[m_currentButton]->setState(ButtonStates::Pressed);
		}
		else
		{
			if (m_charSelectButtons[m_currentButton]->isSelected())
				this->m_charSelectButtons[m_currentButton]->setState(ButtonStates::Pressed);
		}
	}
}

void LobbyState::_handleMouseInput()
{
	DirectX::XMFLOAT2 mousePos = InputHandler::getMousePosition();
	DirectX::XMINT2 windowSize = InputHandler::getWindowSize();

	mousePos.x /= windowSize.x;
	mousePos.y /= windowSize.y;

	if (!isHosting && !hasJoined)
	{
		for (size_t i = 0; i < m_lobbyButtons.size(); i++)
		{
			if (m_lobbyButtons[i]->Inside(mousePos))
			{
				//set this button to current and on hover state
				m_currentButton = i;
				m_lobbyButtons[i]->Select(true);
				m_lobbyButtons[i]->setState(ButtonStates::Hover);
				//check if we released this button
				if (m_lobbyButtons[i]->isReleased(mousePos))
					m_lobbyButtons[i]->setState(ButtonStates::Pressed);
				//set all the other buttons to
				for (size_t j = 0; j < m_lobbyButtons.size(); j++)
				{
					if (i != j)
					{
						m_lobbyButtons[j]->Select(false);
						m_lobbyButtons[j]->setState(ButtonStates::Normal);
					}
				}
				break;
			}
		}
	}
	else
	{
		for (size_t i = 0; i < m_charSelectButtons.size(); i++)
		{
			if (m_charSelectButtons[i]->Inside(mousePos))
			{
				//set this button to current and on hover state
				m_currentButton = i;
				m_charSelectButtons[i]->Select(true);
				m_charSelectButtons[i]->setState(ButtonStates::Hover);
				//check if we released this button
				if (m_charSelectButtons[i]->isReleased(mousePos))
					m_charSelectButtons[i]->setState(ButtonStates::Pressed);
				//set all the other buttons to
				for (size_t j = 0; j < m_charSelectButtons.size(); j++)
				{
					if (i != j)
					{
						m_charSelectButtons[j]->Select(false);
						m_charSelectButtons[j]->setState(ButtonStates::Normal);
					}
				}
				break;
			}
		}
	}
}

void LobbyState::_updateSelectionStates()
{
	//update the selection states
	if (!isHosting && !hasJoined)
	{
		for (size_t i = 0; i < m_lobbyButtons.size(); i++)
		{
			if (i != m_currentButton)
			{
				m_lobbyButtons[i]->Select(false);
				m_lobbyButtons[i]->setState(ButtonStates::Normal);
			}
			else
			{
				if (!m_lobbyButtons[i]->isSelected()
					&& (m_lobbyButtons[i]->getState() != (unsigned int)ButtonStates::Pressed)
					)
				{
					m_lobbyButtons[i]->Select(true);
					m_lobbyButtons[i]->setState(ButtonStates::Hover);
				}
			}
		}

	}
	else
	{
		for (size_t i = 0; i < m_charSelectButtons.size(); i++)
		{
			if (i != m_currentButton)
			{
				m_charSelectButtons[i]->Select(false);
				m_charSelectButtons[i]->setState(ButtonStates::Normal);
			}
			else
			{
				if (!m_charSelectButtons[i]->isSelected()
					&& (m_charSelectButtons[i]->getState() != (unsigned int)ButtonStates::Pressed)
					)
				{
					m_charSelectButtons[i]->Select(true);
					m_charSelectButtons[i]->setState(ButtonStates::Hover);
				}
			}
		}
	}
}
