#include "RipTagPCH.h"
#include "LobbyState.h"
#include <time.h>
#include <cstdlib>


LobbyState::LobbyState(RenderingManager * rm) : State(rm)
{
	Manager::g_textureManager.loadTextures("KOMBIN");
	Manager::g_textureManager.loadTextures("SPHERE");
	Manager::g_textureManager.loadTextures("PIRASRUM");
	Manager::g_textureManager.loadTextures("DAB");

	_initButtons();
	m_currentButton = (unsigned int)ButtonOrderLobby::Host;

	this->pNetwork = Network::Multiplayer::GetInstance();
	this->pNetwork->StartUpPeer();
	this->m_MySysAdress = pNetwork->GetMySysAdress();
	//INITIAL RANDOM HOST NAME
	srand(time(0));
	this->m_MyHostName = "Host:" + std::to_string(rand());
	this->m_adPacket = Network::LOBBYEVENTPACKET(Network::ID_SERVER_ADVERTISE, this->m_MyHostName);

	this->_registerThisInstanceToNetwork();
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

	for (auto & button : this->m_hostListButtons)
	{
		button->Release();
		delete button;
	}
	this->m_hostListButtons.clear();

	if (this->m_infoWindow)
	{
		this->m_infoWindow->Release();
		delete this->m_infoWindow;
	}

	this->pNetwork->ShutdownPeer();
}

void LobbyState::Update(double deltaTime)
{
	//update the content for the info window
	this->_updateInfoString();
	//Network updates first
	if (isHosting && !hasClient)
		pNetwork->AdvertiseHost((const char*)&this->m_adPacket, sizeof(Network::LOBBYEVENTPACKET) + 1);

	if (!InputHandler::getShowCursor())
		InputHandler::setShowCursor(TRUE);

	//in case after a flush no more servers are found and we are in the server list we have to reset to the default button so we dont get out of range errors
	if (inServerList && m_hostListButtons.size() == 0)
	{
		inServerList = false;
		m_currentButton = (unsigned int)ButtonOrderLobby::Host;
	}

	_handleMouseInput();
	_handleGamePadInput();
	_handleKeyboardInput();

	if (!isHosting && !hasJoined && !inServerList)
	{
		if (m_lobbyButtons[m_currentButton]->getState() == (unsigned int)ButtonStates::Pressed)
		{
			switch ((ButtonOrderLobby)m_currentButton)
			{
			case ButtonOrderLobby::Host:
				pNetwork->SetupServer();
				isHosting = true;
				_resetCharSelectButtonStates();
				break;
			case ButtonOrderLobby::Join:
				if (strcmp(this->selectedHost.ToString(false), "UNASSIGNED_SYSTEM_ADDRESS") == 0)
				{
					m_lobbyButtons[(unsigned int)ButtonOrderLobby::Join]->setState(ButtonStates::Hover);
					break;
				}
				else if (!pNetwork->ConnectTo(this->selectedHost))
					m_lobbyButtons[(unsigned int)ButtonOrderLobby::Join]->setState(ButtonStates::Hover);
				break;
			case ButtonOrderLobby::Refresh:
				_flushServerList();
				m_lobbyButtons[m_currentButton]->setState(ButtonStates::Hover);
				break;
			case ButtonOrderLobby::Return:
				this->setKillState(true);
				break;
			}
		}
	}
	else if (!isHosting && !hasJoined && inServerList)
	{
		if (m_hostListButtons[m_currentButton]->getState() == (unsigned int)ButtonStates::Pressed)
		{
			std::string hostName = m_hostListButtons[m_currentButton]->getString();
			auto it = m_hostAdressMap.find(hostName);
			if (it != m_hostAdressMap.end())
			{
				this->selectedHost = it->second;
				this->selectedHostInfo = "Selected Host: " + hostName;
				this->m_ServerName = hostName;
			}
			else
			{
				this->selectedHost = RakNet::SystemAddress("0.0.0.0");
				this->selectedHostInfo = "Selected Host: None";
			}
			m_hostListButtons[m_currentButton]->setState(ButtonStates::Hover);
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
				if (isHosting)
				{
					m_clientIP = RakNet::SystemAddress("0.0.0.0");
					hasClient = false;
					pNetwork->CloseServer(m_clientIP);
				}
				if (hasJoined)
				{
					this->selectedHost = RakNet::SystemAddress("0.0.0.0");
					pNetwork->Disconnect(selectedHost);
				}
				isRemoteReady = false;
				isReady = false;
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

	if (this->m_infoWindow)
		this->m_infoWindow->Draw();

	p_renderingManager->Flush(camera);
}

void LobbyState::HandlePacket(unsigned char id, RakNet::Packet * packet)
{
	switch (id)
	{
	case DefaultMessageIDTypes::ID_ADVERTISE_SYSTEM:
		_onAdvertisePacket(packet);
		break;
	case DefaultMessageIDTypes::ID_NEW_INCOMING_CONNECTION:
		_onClientJoinPacket(packet);
		break;
	case DefaultMessageIDTypes::ID_CONNECTION_REQUEST_ACCEPTED:
		_onSucceedPacket(packet);
		break;
	case DefaultMessageIDTypes::ID_CONNECTION_ATTEMPT_FAILED:
		_onFailedPacket(packet);
		break;
	case DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION:
		_onDisconnectPacket(packet);
		break;
	case DefaultMessageIDTypes::ID_NO_FREE_INCOMING_CONNECTIONS:
		_onServerDenied(packet);
		break;
	}
}

void LobbyState::_initButtons()
{
	//Lobby buttons
	{
		//Host button
		this->m_lobbyButtons.push_back(Quad::CreateButton("Host", 0.2f, 0.50f, 0.5f, 0.15f));
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
		//Join button
		this->m_lobbyButtons.push_back(Quad::CreateButton("Join", 0.2f, 0.40f, 0.5f, 0.15f));
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
		//Refresh button
		this->m_lobbyButtons.push_back(Quad::CreateButton("Refresh", 0.2f, 0.30f, 0.5f, 0.15f));
		this->m_lobbyButtons[ButtonOrderLobby::Refresh]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
		this->m_lobbyButtons[ButtonOrderLobby::Refresh]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
		this->m_lobbyButtons[ButtonOrderLobby::Refresh]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
		this->m_lobbyButtons[ButtonOrderLobby::Refresh]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_lobbyButtons[ButtonOrderLobby::Refresh]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
		//Return button
		this->m_lobbyButtons.push_back(Quad::CreateButton("Return", 0.2f, 0.10f, 0.5f, 0.15f));
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
	//Info window
	{
		this->m_infoWindow = Quad::CreateButton("", 0.25f, 0.8f, 0.7f, 0.3f);
		this->m_infoWindow->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
		this->m_infoWindow->setPressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
		this->m_infoWindow->setHoverTexture(Manager::g_textureManager.getTexture("SPHERE"));
		this->m_infoWindow->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_infoWindow->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas16.spritefont"));
	}
}

void LobbyState::_handleGamePadInput()
{
	if (Input::isUsingGamepad())
	{
		if (!isHosting && !hasJoined && !inServerList)
			this->_gamePadMainLobby();
		if (!isHosting && !hasJoined && inServerList)
			this->_gamePadServerList();
		if (isHosting || hasJoined)
			this->_gamePadCharSelection();
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
	if (!isHosting && !hasJoined && !inServerList)
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
	else if (!isHosting && !hasJoined && inServerList)
	{
		for (size_t i = 0; i < m_hostListButtons.size(); i++)
		{
			if (i != m_currentButton)
			{
				m_hostListButtons[i]->Select(false);
				m_hostListButtons[i]->setState(ButtonStates::Normal);
			}
			else
			{
				if (!m_hostListButtons[i]->isSelected() && (m_hostListButtons[i]->getState() != (unsigned int)ButtonStates::Pressed))
				{
					m_hostListButtons[i]->Select(true);
					m_hostListButtons[i]->setState(ButtonStates::Hover);
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

void LobbyState::_flushServerList()
{
	this->selectedHostInfo = "Selected Host: None";
	this->m_hostNameMap.clear();
	this->m_hostAdressMap.clear();
	for (auto & button : this->m_hostListButtons)
	{
		button->Release();
		delete button;
	}
	this->m_hostListButtons.clear();

}

void LobbyState::_updateInfoString()
{
	std::string content = "";
	if (!isHosting && !hasJoined)
	{
		content += "Your Host name: " + this->m_MyHostName + "\n";
		content += selectedHostInfo + "\n";
	}
	else if (isHosting)
	{
		content += "Your Host name: " + this->m_MyHostName + "\n";
		if (hasClient)
			content += "Connected to client: " + std::string(m_clientIP.ToString(false)) + "\n";
		else
			content += "No client connected\n";
	}
	else if (hasJoined)
	{
		content += "Your Host name: " + this->m_MyHostName + "\n";
		content += "Connected to server: " + this->m_ServerName + "\n";
	}
	if (this->m_infoWindow)
		this->m_infoWindow->setString(content);
}

void LobbyState::_gamePadMainLobby()
{
	if (GamePadHandler::IsUpDpadPressed())
	{	
		if (m_currentButton == 0)
			m_currentButton = (unsigned int)ButtonOrderLobby::Return;
		else
			m_currentButton--;
	}
	else if (GamePadHandler::IsDownDpadPressed())
	{
		m_currentButton++;
		m_currentButton = m_currentButton % ((unsigned int)ButtonOrderLobby::Return + 1);
	}
	else if (GamePadHandler::IsRightDpadPressed())
	{
		if (m_hostListButtons.size() > 0)
		{
			m_lobbyButtons[m_currentButton]->setState(ButtonStates::Normal);
			m_lobbyButtons[m_currentButton]->Select(false);
			inServerList = true;
			m_currentButton = 0;
		}
	}

	_updateSelectionStates();

	//Check for action input
	if (GamePadHandler::IsAPressed())
	{
		if (m_lobbyButtons[m_currentButton]->isSelected())
			this->m_lobbyButtons[m_currentButton]->setState(ButtonStates::Pressed);
	}
}

void LobbyState::_gamePadCharSelection()
{
	if (GamePadHandler::IsUpDpadPressed())
	{
		if (m_currentButton == 0)
			m_currentButton = (unsigned int)CharacterSelection::Back;
		else
			m_currentButton--;
	}
	else if (GamePadHandler::IsDownDpadPressed())
	{
		m_currentButton++;
		m_currentButton = m_currentButton % ((unsigned int)CharacterSelection::Back + 1);
	}

	_updateSelectionStates();

	//Check for action input
	if (GamePadHandler::IsAPressed())
	{		
		if (m_charSelectButtons[m_currentButton]->isSelected())
			this->m_charSelectButtons[m_currentButton]->setState(ButtonStates::Pressed);
	}
}

void LobbyState::_gamePadServerList()
{
	//this is just in case after a flush we are outside of our range, we simply reset to the first
	

	if (GamePadHandler::IsUpDpadPressed())
	{
		if (m_currentButton == 0)
			m_currentButton = this->m_hostListButtons.size() - 1;
		else
			m_currentButton--;
	}
	else if (GamePadHandler::IsDownDpadPressed())
	{
		m_currentButton++;
		m_currentButton = m_currentButton % this->m_hostListButtons.size();
	}
	else if (GamePadHandler::IsLeftDpadPressed())
	{
		m_hostListButtons[m_currentButton]->setState(ButtonStates::Normal);
		m_hostListButtons[m_currentButton]->Select(false);

		m_currentButton = (unsigned int)ButtonOrderLobby::Host;
		inServerList = false;
	}

	_updateSelectionStates();

	if (GamePadHandler::IsAPressed())
	{
		if (m_hostListButtons[m_currentButton]->isSelected())
		{
			this->m_hostListButtons[m_currentButton]->setState(ButtonStates::Pressed);
		}
	}
}

void LobbyState::_registerThisInstanceToNetwork()
{
	using namespace Network;
	using namespace std::placeholders;

	Multiplayer::addToLobbyOnReceiveMap(DefaultMessageIDTypes::ID_ADVERTISE_SYSTEM, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(DefaultMessageIDTypes::ID_NEW_INCOMING_CONNECTION, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(DefaultMessageIDTypes::ID_CONNECTION_REQUEST_ACCEPTED, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(DefaultMessageIDTypes::ID_CONNECTION_ATTEMPT_FAILED, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(DefaultMessageIDTypes::ID_NO_FREE_INCOMING_CONNECTIONS, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION, std::bind(&LobbyState::HandlePacket, this, _1, _2));
}

void LobbyState::_onAdvertisePacket(RakNet::Packet * packet)
{
	if (packet->guid == pNetwork->GetMyGUID())
		return;

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

void LobbyState::_onClientJoinPacket(RakNet::Packet * data)
{
	hasClient = true;
	m_clientIP = data->systemAddress;
}

void LobbyState::_onFailedPacket(RakNet::Packet * data)
{
	m_lobbyButtons[(unsigned int)ButtonOrderLobby::Join]->setState(ButtonStates::Normal);

	selectedHostInfo = "Failed to connect to host\n";
}

void LobbyState::_onSucceedPacket(RakNet::Packet * data)
{
	this->selectedHost = data->systemAddress;
	hasJoined = true;
	_resetCharSelectButtonStates();
}

void LobbyState::_onDisconnectPacket(RakNet::Packet * data)
{
	//this means we are the client -> the server has shutdown -> go back to main lobby
	if (hasJoined)
	{
		hasJoined = false;
		m_currentButton = (unsigned int)ButtonOrderLobby::Host;
		_flushServerList();
		selectedHostInfo = "Lost connection to host\n";
		this->selectedHost = RakNet::SystemAddress("0.0.0.0");
	}
	else if (isHosting)
	{
		m_clientIP = RakNet::SystemAddress("0.0.0.0");
		hasClient = false;
		isRemoteReady = false;
	}
}

void LobbyState::_onServerDenied(RakNet::Packet * data)
{
	m_lobbyButtons[(unsigned int)ButtonOrderLobby::Join]->setState(ButtonStates::Normal);

	selectedHostInfo = "Server is full\n";
}

void LobbyState::_newHostEntry(std::string & hostName)
{
	static float startX = 0.7f;
	static float startY = 0.8f;
	static float offsetY = 0.05f;
	static float scaleX = 0.4f;
	static float scaleY = 0.1f;

	size_t size = m_hostListButtons.size();

	float px = startX;
	float py = startY - (offsetY * size);

	m_hostListButtons.push_back(Quad::CreateButton(hostName, px, py, scaleX, scaleY));
	m_hostListButtons[size]->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_hostListButtons[size]->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	m_hostListButtons[size]->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	m_hostListButtons[size]->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_hostListButtons[size]->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas16.spritefont"));
}
