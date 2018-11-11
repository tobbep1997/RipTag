#include "RipTagPCH.h"
#include "LobbyState.h"


LobbyState::LobbyState(RenderingManager * rm) : State(rm)
{


	
}


LobbyState::~LobbyState()
{
	for (auto &button : this->m_lobbyButtons)
	{
		button->Release();
		delete button;
	}
	this->m_lobbyButtons.clear();
	for (auto & button : this->m_charSelectButtons)
	{
		button->Release();
		delete button;
	}
	this->m_charSelectButtons.clear();
	this->pNetwork->ShutdownPeer();
}

void LobbyState::Update(double deltaTime)
{
	//Network updates first
	if (isHosting)
		pNetwork->AdvertiseHost((const char*)&this->m_adPacket, sizeof(Network::LOBBYEVENTPACKET));

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
				_resetCharSelectButtonStates();
				break;
			case ButtonOrderLobby::Join:
				hasJoined = true;
				_resetCharSelectButtonStates();
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
				_resetLobbyButtonStates();
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
		for (auto & listElement : this->m_hostListButtons)
			listElement->Draw();
	}
	else
	{
		for (auto &button : this->m_charSelectButtons)
			button->Draw();
	}

	p_renderingManager->Flush(camera);
}

void LobbyState::HandlePacket(unsigned char id, RakNet::Packet * packet)
{
	switch (id)
	{
	case DefaultMessageIDTypes::ID_ADVERTISE_SYSTEM:
		_onAdvertisePacket(packet);
		break;
	}
}

void LobbyState::_initButtons()
{
	//Lobby buttons
	{
		//Host button
		this->m_lobbyButtons.push_back(Quad::CreateButton("Host", 0.2f, 0.55f, 0.3f, 0.25f));
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setUnpressedTexture("SPHERE");
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setPressedTexture("DAB");
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setHoverTexture("PIRASRUM");
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setFont(FontHandler::getFont("consolas32"));
		//Join button
		this->m_lobbyButtons.push_back(Quad::CreateButton("Join", 0.2f, 0.35f, 0.3f, 0.25f));
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setUnpressedTexture("SPHERE");
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setPressedTexture("DAB");
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setHoverTexture("PIRASRUM");
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setFont(FontHandler::getFont("consolas32"));
		//Return button
		this->m_lobbyButtons.push_back(Quad::CreateButton("Return", 0.2f, 0.10f, 0.3f, 0.25f));
		this->m_lobbyButtons[ButtonOrderLobby::Return]->setUnpressedTexture("SPHERE");
		this->m_lobbyButtons[ButtonOrderLobby::Return]->setPressedTexture("DAB");
		this->m_lobbyButtons[ButtonOrderLobby::Return]->setHoverTexture("PIRASRUM");
		this->m_lobbyButtons[ButtonOrderLobby::Return]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_lobbyButtons[ButtonOrderLobby::Return]->setFont(FontHandler::getFont("consolas32"));
	}
	//Character selection buttons
	{
		//Character One
		this->m_charSelectButtons.push_back(Quad::CreateButton("Lejf", 0.3f, 0.55f, 0.3f, 0.25f));
		this->m_charSelectButtons[CharacterSelection::CharOne]->setUnpressedTexture("SPHERE");
		this->m_charSelectButtons[CharacterSelection::CharOne]->setPressedTexture("DAB");
		this->m_charSelectButtons[CharacterSelection::CharOne]->setHoverTexture("PIRASRUM");
		this->m_charSelectButtons[CharacterSelection::CharOne]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_charSelectButtons[CharacterSelection::CharOne]->setFont(FontHandler::getFont("consolas32"));
		//Character Two
		this->m_charSelectButtons.push_back(Quad::CreateButton("Billy", 0.7f, 0.55f, 0.3f, 0.25f));
		this->m_charSelectButtons[CharacterSelection::CharTwo]->setUnpressedTexture("SPHERE");
		this->m_charSelectButtons[CharacterSelection::CharTwo]->setPressedTexture("DAB");
		this->m_charSelectButtons[CharacterSelection::CharTwo]->setHoverTexture("PIRASRUM");
		this->m_charSelectButtons[CharacterSelection::CharTwo]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_charSelectButtons[CharacterSelection::CharTwo]->setFont(FontHandler::getFont("consolas32"));
		//Ready
		this->m_charSelectButtons.push_back(Quad::CreateButton("Ready", 0.5f, 0.25f, 0.3f, 0.25f));
		this->m_charSelectButtons[CharacterSelection::Ready]->setUnpressedTexture("SPHERE");
		this->m_charSelectButtons[CharacterSelection::Ready]->setPressedTexture("DAB");
		this->m_charSelectButtons[CharacterSelection::Ready]->setHoverTexture("PIRASRUM");
		this->m_charSelectButtons[CharacterSelection::Ready]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_charSelectButtons[CharacterSelection::Ready]->setFont(FontHandler::getFont("consolas32"));
		//Return
		this->m_charSelectButtons.push_back(Quad::CreateButton("Return", 0.85f, 0.25f, 0.3f, 0.25f));
		this->m_charSelectButtons[CharacterSelection::Back]->setUnpressedTexture("SPHERE");
		this->m_charSelectButtons[CharacterSelection::Back]->setPressedTexture("DAB");
		this->m_charSelectButtons[CharacterSelection::Back]->setHoverTexture("PIRASRUM");
		this->m_charSelectButtons[CharacterSelection::Back]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_charSelectButtons[CharacterSelection::Back]->setFont(FontHandler::getFont("consolas32"));
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
	if (InputHandler::wasKeyPressed(InputHandler::Up))
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
	else if (InputHandler::wasKeyPressed(InputHandler::Down))
	{
		m_currentButton++;
		if (!isHosting && !hasJoined)
			m_currentButton = m_currentButton % ((unsigned int)ButtonOrderLobby::Return + 1);
		else
			m_currentButton = m_currentButton % ((unsigned int)CharacterSelection::Back + 1);
	}

	_updateSelectionStates();

	if (InputHandler::wasKeyPressed(InputHandler::Enter))
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

void LobbyState::_resetLobbyButtonStates()
{
	for (auto &button : m_lobbyButtons)
	{
		button->Select(false);
		button->setState(ButtonStates::Normal);
	}
	m_currentButton = (unsigned int)ButtonOrderLobby::Host;
}

void LobbyState::_resetCharSelectButtonStates()
{
	for (auto & button : m_charSelectButtons)
	{
		button->Select(false);
		button->setState(ButtonStates::Normal);
	}
	m_currentButton = (unsigned int)CharacterSelection::CharOne;
}

void LobbyState::_registerThisInstanceToNetwork()
{
	using namespace Network;
	using namespace std::placeholders;

	Multiplayer::addToLobbyOnReceiveMap(DefaultMessageIDTypes::ID_ADVERTISE_SYSTEM, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(NETWORKMESSAGES::ID_CLIENT_JOIN, std::bind(&LobbyState::HandlePacket, this, _1, _2));
}

void LobbyState::_onAdvertisePacket(RakNet::Packet * packet)
{
	uint64_t uniqueHostID = packet->guid.g;
	RakNet::SystemAddress hostAdress = packet->systemAddress;
	Network::LOBBYEVENTPACKET * data = (Network::LOBBYEVENTPACKET*)packet->data;
	std::string hostName = std::string(data->string);

	//See if we have this host already added, if not we add it to our list
	auto it = this->m_hostNameMap.find(uniqueHostID);
	if (it == m_hostNameMap.end())
	{
		m_hostNameMap.insert(std::pair<uint64_t, std::string>(uniqueHostID, hostName));
		m_hostAdressMap.insert(std::pair<std::string, RakNet::SystemAddress>(hostName, hostAdress));
		_newHostEntry(hostName);
	}

}

void LobbyState::_newHostEntry(std::string & hostName)
{
	static float startX = 0.7f;
	static float startY = 0.8f;
	static float offsetY = 0.1f;
	static float scaleX = 0.4f;
	static float scaleY = 0.1f;

	size_t size = m_hostListButtons.size();

	float px = startX;
	float py = startY - (offsetY * size);

	m_hostListButtons.push_back(Quad::CreateButton(hostName, px, py, scaleX, scaleY));
	m_hostListButtons[size]->setUnpressedTexture(("SPHERE"));
	m_hostListButtons[size]->setPressedTexture(("DAB"));
	m_hostListButtons[size]->setHoverTexture(("PIRASRUM"));
	m_hostListButtons[size]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_hostListButtons[size]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas16.spritefont"));
}

void LobbyState::Load()
{
	Manager::g_textureManager.loadTextures("KOMBIN");
	Manager::g_textureManager.loadTextures("SPHERE");
	Manager::g_textureManager.loadTextures("PIRASRUM");
	Manager::g_textureManager.loadTextures("DAB");

	_initButtons();
	m_currentButton = (unsigned int)ButtonOrderLobby::Host;

	this->pNetwork = Network::Multiplayer::GetInstance();
	this->pNetwork->StartUpPeer();
	//INITIAL RANDOM HOST NAME
	this->m_MyHostName = "Host" + std::to_string(randomMT());
	this->m_adPacket = Network::LOBBYEVENTPACKET(Network::ID_SERVER_ADVERTISE, this->m_MyHostName);

	this->_registerThisInstanceToNetwork();

	std::cout << "Lobby Load" << std::endl;
}

void LobbyState::unLoad()
{
	Manager::g_textureManager.UnloadTexture("KOMBIN");
	Manager::g_textureManager.UnloadTexture("SPHERE");
	Manager::g_textureManager.UnloadTexture("PIRASRUM");
	Manager::g_textureManager.UnloadTexture("DAB");
	Manager::g_textureManager.UnloadAllTexture();
	std::cout << "Lobby unLoad" << std::endl;
}
