#include "RipTagPCH.h"
#include "LobbyState.h"
#include <time.h>
#include <cstdlib>


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
	if (this->m_background)
	{
		this->m_background->Release();
		delete this->m_background;
	}
	if (this->m_charSelectionBG)
	{
		this->m_charSelectionBG->Release();
		delete this->m_charSelectionBG;
	}
	if (this->m_charOneInfo)
	{
		this->m_charOneInfo->Release();
		delete this->m_charOneInfo;
	}
	if (this->m_charTwoInfo)
	{
		this->m_charTwoInfo->Release();
		delete this->m_charTwoInfo;
	}
	if (this->m_charSelectInfo)
	{
		this->m_charSelectInfo->Release();
		delete this->m_charSelectInfo;
	}
	if (pCoopData)
	{
		delete pCoopData;
		pCoopData = nullptr;
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
	_handleKeyboardInput();
	_handleGamePadInput();

	if (!isHosting && !hasJoined && !inServerList)
	{
		for (size_t i = 0; i < m_hostListButtons.size(); i++)
		{
			if (m_ServerName != "None")
			{
				if (m_hostListButtons[i]->getString() == m_ServerName)
					m_hostListButtons[i]->setTextColor(ActivatedColor);
				else
					m_hostListButtons[i]->setTextColor(DefaultColor);
			}
			else
				m_hostListButtons[i]->setTextColor(DefaultColor);
		}

		if (m_lobbyButtons[m_currentButton]->getState() == (unsigned int)ButtonStates::Pressed)
		{
			switch ((ButtonOrderLobby)m_currentButton)
			{
			case ButtonOrderLobby::Host:
				pNetwork->SetupServer();
				isHosting = true;
				_resetCharSelectButtonStates();
				this->m_charSelectInfo->setString("You:\n" + this->m_MyHostName + "\nConnected to:\nNone");
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
		for (size_t i = 0; i < m_hostListButtons.size(); i++)
		{
			if (m_ServerName != "None")
			{
				if (m_hostListButtons[i]->getString() == m_ServerName)
					m_hostListButtons[i]->setTextColor(ActivatedColor);
				else
					m_hostListButtons[i]->setTextColor(DefaultColor);
			}
			else
				m_hostListButtons[i]->setTextColor(DefaultColor);
		}

		if (m_hostListButtons[m_currentButtonServerList]->getState() == (unsigned int)ButtonStates::Pressed)
		{
			std::string hostName = m_hostListButtons[m_currentButtonServerList]->getString();
			auto it = m_hostAdressMap.find(hostName);
			if (it != m_hostAdressMap.end())
			{
				if (m_ServerName != hostName)
				{
					this->m_ServerName = hostName;
					this->selectedHost = it->second;
					this->selectedHostInfo = "Selected Host:\n" + hostName;
				}
				else
				{
					this->m_ServerName = "None";
					this->selectedHost = RakNet::SystemAddress("0.0.0.0");
					this->selectedHostInfo = "Selected Host:\nNone\n";
				}
			}
			else
			{
				this->selectedHost = RakNet::SystemAddress("0.0.0.0");
				this->selectedHostInfo = "Selected Host:\nNone\n";
			}
			m_hostListButtons[m_currentButtonServerList]->setState(ButtonStates::Normal);
		}
	}
	else
	{
		if (hasCharSelected && hasRemoteCharSelected && !isReady && hasJoined)
		{
			m_charSelectButtons[Ready]->setTextColor(DefaultColor);
		}
		else if (hasCharSelected && hasRemoteCharSelected && !isReady && isHosting && isRemoteReady)
		{
			m_charSelectButtons[Ready]->setTextColor(DefaultColor);
		}
		else if (hasCharSelected && hasRemoteCharSelected && isReady)
		{
			m_charSelectButtons[Ready]->setTextColor(ActivatedColor);
		}
		else
			m_charSelectButtons[Ready]->setTextColor(InactivatedColor);


		if (m_charSelectButtons[m_currentButton]->getState() == (unsigned int)ButtonStates::Pressed)
		{
			switch ((CharacterSelection)m_currentButton)
			{
			case CharacterSelection::CharOne:
				if (hasCharSelected && selectedChar == 1)
				{
					hasCharSelected = false;
					selectedChar = 0;
					m_charSelectButtons[m_currentButton]->setTextColor(DefaultColor);
				}
				else if (!hasCharSelected && selectedChar == 0 && remoteSelectedChar != 1)
				{
					hasCharSelected = true;
					selectedChar = 1;
					m_charSelectButtons[m_currentButton]->setTextColor(ActivatedColor);
				}
				this->_sendCharacterSelectionPacket();
				break;
			case CharacterSelection::CharTwo:
				if (hasCharSelected && selectedChar == 2)
				{
					hasCharSelected = false;
					selectedChar = 0;	
					m_charSelectButtons[m_currentButton]->setTextColor(DefaultColor);
				}
				else if (!hasCharSelected && selectedChar == 0 && remoteSelectedChar != 2)
				{
					hasCharSelected = true;
					selectedChar = 2;					
					m_charSelectButtons[m_currentButton]->setTextColor(ActivatedColor);
				}
				this->_sendCharacterSelectionPacket();
				break;
			case CharacterSelection::Ready:
				if (hasCharSelected && hasRemoteCharSelected)
				{
					//Client logic
					if (hasJoined)
					{
						if (isReady)
							isReady = false;
						else
							isReady = true;

						_sendReadyPacket();
					}
					//Server logic
					if (isHosting)
					{
						if (isRemoteReady)
							isReady = true;
						else
							isReady = false;
					}

					if (isHosting && isReady && isRemoteReady)
					{
						_sendGameStartedPacket();
						//create the proper struct/tuple containing all necessary info
						//for the PlayState constructor, then push it on the state stack
						if (pCoopData)
						{
							delete pCoopData;
							pCoopData = nullptr;
						}
						pCoopData = new CoopData();
						pCoopData->seed = pNetwork->GetSeed();
						pCoopData->localPlayerCharacter = selectedChar;
						pCoopData->remotePlayerCharacter = remoteSelectedChar;
						pCoopData->remoteID = this->m_remoteNID;
						pCoopData->role = Role::Server;

						isReady = false;
						isRemoteReady = false;

						//loading screen stuff
						{
							delete m_charOneInfo;
							m_charOneInfo = nullptr;
							delete m_charTwoInfo;
							m_charTwoInfo = nullptr;
							delete m_charSelectInfo;
							m_charSelectInfo = nullptr;
							delete m_charSelectionBG;
							m_charSelectionBG = nullptr;
							m_loadingScreen.removeGUI(this->m_charSelectButtons);
							m_loadingScreen.draw();
						}

						this->pushNewState(new PlayState(this->p_renderingManager, (void*)pCoopData));
						
					}
				}
				break;
			case CharacterSelection::Back:
				if (isHosting)
				{
					pNetwork->CloseServer(m_clientIP);
					hasClient = false;
					m_clientIP = RakNet::SystemAddress("0.0.0.0");
					this->selectedHostInfo = "Selected Host:\nNone\n";
				}
				if (hasJoined)
				{
					pNetwork->Disconnect(selectedHost);
					this->selectedHost = RakNet::SystemAddress("0.0.0.0");
					this->selectedHostInfo = "Selected Host:\nNone\n";
				}
				hasCharSelected = false;
				selectedChar = 0;
				hasRemoteCharSelected = false;
				remoteSelectedChar = 0;

				m_charSelectButtons[CharOne]->setTextColor(DefaultColor);
				m_charSelectButtons[CharTwo]->setTextColor(DefaultColor);

				_flushServerList();
				isRemoteReady = false;
				isReady = false;
				isHosting = false;
				hasJoined = false;
				this->_resetLobbyButtonStates();
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
		if (this->m_background)
			this->m_background->Draw();
		if (this->m_infoWindow)
			this->m_infoWindow->Draw();
		for (auto &button : this->m_lobbyButtons)
			button->Draw();
		for (auto & listElement : this->m_hostListButtons)
			listElement->Draw();
	}
	else
	{
		if (this->m_charSelectionBG)
			this->m_charSelectionBG->Draw();
		if (this->m_charOneInfo)
			this->m_charOneInfo->Draw();
		if (this->m_charTwoInfo)
			this->m_charTwoInfo->Draw();
		if (this->m_charSelectInfo)
			this->m_charSelectInfo->Draw();
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
	case Network::ID_CHAR_SELECTED:
		_onCharacterSelectionPacket(packet);
		break;
	case Network::ID_READY_PRESSED:
		_onReadyPacket(packet);
		break;
	case Network::ID_REQUEST_NID:
		_onRequestPacket(id, packet);
		break;
	case Network::ID_REQUEST_SELECTED_CHAR:
		_onRequestPacket(id, packet);
		break;
	case Network::ID_REPLY_NID:
		_onReplyPacket(packet);
		break;
	case Network::ID_GAME_STARTED:
		_onGameStartedPacket(packet);
		break;
	case Network::ID_HOST_NAME:
		_onHostNamePacket(packet);
		break;
	}
}

void LobbyState::_initButtons()
{
	//Lobby buttons
	{
		
		//Host button
		this->m_lobbyButtons.push_back(Quad::CreateButton("Host", 0.3075f, 0.508f, 0.30f, 0.12f));
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setUnpressedTexture("gui_transparent_pixel");
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setPressedTexture("gui_pressed_pixel");
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setHoverTexture("gui_hover_pixel");
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setTextColor(DefaultColor);
		this->m_lobbyButtons[ButtonOrderLobby::Host]->setFont(FontHandler::getFont("consolas32"));
		//Join button
		this->m_lobbyButtons.push_back(Quad::CreateButton("Join", 0.3075f, 0.408f, 0.30f, 0.12f));
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setUnpressedTexture("gui_transparent_pixel");
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setPressedTexture("gui_pressed_pixel");
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setHoverTexture("gui_hover_pixel");
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setTextColor(DefaultColor);
		this->m_lobbyButtons[ButtonOrderLobby::Join]->setFont(FontHandler::getFont("consolas32"));
		//Refresh button
		this->m_lobbyButtons.push_back(Quad::CreateButton("Refresh", 0.3075f, 0.308f, 0.30f, 0.12f));
		this->m_lobbyButtons[ButtonOrderLobby::Refresh]->setUnpressedTexture("gui_transparent_pixel");
		this->m_lobbyButtons[ButtonOrderLobby::Refresh]->setPressedTexture("gui_pressed_pixel");
		this->m_lobbyButtons[ButtonOrderLobby::Refresh]->setHoverTexture("gui_hover_pixel");
		this->m_lobbyButtons[ButtonOrderLobby::Refresh]->setTextColor(DefaultColor);
		this->m_lobbyButtons[ButtonOrderLobby::Refresh]->setFont(FontHandler::getFont("consolas32"));
		//Return button
		this->m_lobbyButtons.push_back(Quad::CreateButton("Return", 0.3075f, 0.195f, 0.27f, 0.12f));
		this->m_lobbyButtons[ButtonOrderLobby::Return]->setUnpressedTexture("gui_transparent_pixel");
		this->m_lobbyButtons[ButtonOrderLobby::Return]->setPressedTexture("gui_pressed_pixel");
		this->m_lobbyButtons[ButtonOrderLobby::Return]->setHoverTexture("gui_hover_pixel");
		this->m_lobbyButtons[ButtonOrderLobby::Return]->setTextColor(DefaultColor);
		this->m_lobbyButtons[ButtonOrderLobby::Return]->setFont(FontHandler::getFont("consolas32"));
	}
	//Character selection buttons
	{
		//Character One
		this->m_charSelectButtons.push_back(Quad::CreateButton("Lejf", 0.254f, 0.28f, 0.29f, 0.11f));
		this->m_charSelectButtons[CharacterSelection::CharOne]->setUnpressedTexture("gui_transparent_pixel");
		this->m_charSelectButtons[CharacterSelection::CharOne]->setPressedTexture("gui_pressed_pixel");
		this->m_charSelectButtons[CharacterSelection::CharOne]->setHoverTexture("gui_hover_pixel");
		this->m_charSelectButtons[CharacterSelection::CharOne]->setTextColor(DefaultColor);
		this->m_charSelectButtons[CharacterSelection::CharOne]->setFont(FontHandler::getFont("consolas16"));
		//Character Two
		this->m_charSelectButtons.push_back(Quad::CreateButton("Billy", 0.748f, 0.28f, 0.2905f, 0.11f));
		this->m_charSelectButtons[CharacterSelection::CharTwo]->setUnpressedTexture("gui_transparent_pixel");
		this->m_charSelectButtons[CharacterSelection::CharTwo]->setPressedTexture("gui_pressed_pixel");
		this->m_charSelectButtons[CharacterSelection::CharTwo]->setHoverTexture("gui_hover_pixel");
		this->m_charSelectButtons[CharacterSelection::CharTwo]->setTextColor(DefaultColor);
		this->m_charSelectButtons[CharacterSelection::CharTwo]->setFont(FontHandler::getFont("consolas16"));
		//Ready
		this->m_charSelectButtons.push_back(Quad::CreateButton("Ready", 0.4f, 0.12f, 0.3f, 0.11f));
		this->m_charSelectButtons[CharacterSelection::Ready]->setUnpressedTexture("gui_transparent_pixel");
		this->m_charSelectButtons[CharacterSelection::Ready]->setPressedTexture("gui_pressed_pixel");
		this->m_charSelectButtons[CharacterSelection::Ready]->setHoverTexture("gui_hover_pixel");
		this->m_charSelectButtons[CharacterSelection::Ready]->setTextColor(DefaultColor);
		this->m_charSelectButtons[CharacterSelection::Ready]->setFont(FontHandler::getFont("consolas32"));
		//Return
		this->m_charSelectButtons.push_back(Quad::CreateButton("Return", 0.6f, 0.12f, 0.3f, 0.11f));
		this->m_charSelectButtons[CharacterSelection::Back]->setUnpressedTexture("gui_transparent_pixel");
		this->m_charSelectButtons[CharacterSelection::Back]->setPressedTexture("gui_pressed_pixelini");
		this->m_charSelectButtons[CharacterSelection::Back]->setHoverTexture("gui_hover_pixel");
		this->m_charSelectButtons[CharacterSelection::Back]->setTextColor(DefaultColor);
		this->m_charSelectButtons[CharacterSelection::Back]->setFont(FontHandler::getFont("consolas32"));
	}
	//Info window
	{
		this->m_infoWindow = Quad::CreateButton("", 0.32f, 0.70f, 0.385f, 0.40f);
		this->m_infoWindow->setUnpressedTexture("gui_transparent_pixel");
		this->m_infoWindow->setPressedTexture("gui_transparent_pixel");
		this->m_infoWindow->setHoverTexture("gui_transparent_pixel");
		this->m_infoWindow->setTextColor(DefaultColor);
		this->m_infoWindow->setFont(FontHandler::getFont("consolas16"));
		this->m_infoWindow->setTextAlignment(Quad::TextAlignment::leftAligned);
	}

	//Background Window
	{
		this->m_background = new Quad();
		this->m_background->init();
		this->m_background->setPivotPoint(Quad::PivotPoint::center);
		this->m_background->setPosition(0.5f, 0.5f);
		this->m_background->setScale(2.0f, 2.0f);
		this->m_background->setUnpressedTexture("gui_main_lobby");
		this->m_background->setPressedTexture("gui_main_lobby");
		this->m_background->setHoverTexture("gui_main_lobby");
	}
	//Char selection background + info window
	{
		this->m_charSelectionBG = new Quad();
		this->m_charSelectionBG->init();
		this->m_charSelectionBG->setPivotPoint(Quad::PivotPoint::center);
		this->m_charSelectionBG->setPosition(0.5f, 0.5f);
		this->m_charSelectionBG->setScale(2.0f, 2.0f);
		this->m_charSelectionBG->setUnpressedTexture("gui_character_selection");
		this->m_charSelectionBG->setPressedTexture("gui_character_selection");
		this->m_charSelectionBG->setHoverTexture("gui_character_selection");

		this->m_charSelectInfo = Quad::CreateButton("", 0.5f, 0.575f, 0.3f, 0.3f);
		this->m_charSelectInfo->setUnpressedTexture("gui_hover_pixel");
		this->m_charSelectInfo->setPressedTexture("gui_hover_pixel");
		this->m_charSelectInfo->setHoverTexture("gui_hover_pixel");
		this->m_charSelectInfo->setTextColor(DefaultColor);
		this->m_charSelectInfo->setFont(FontHandler::getFont("consolas16"));
		this->m_charSelectInfo->setTextAlignment(Quad::TextAlignment::centerAligned);
	}
	//Character info quads
	{
		std::string charOneInfoStr = " Abilities:\n\n  Teleport\n  Knock-out Rock\n";
		std::string charTwoInfoStr = "Abilities:\n\n  Blink\n  Possess\n";

		this->m_charOneInfo = Quad::CreateButton(charOneInfoStr, 0.25f, 0.46f, 0.385f, 0.35f);
		this->m_charOneInfo->setUnpressedTexture("gui_transparent_pixel");
		this->m_charOneInfo->setPressedTexture("gui_transparent_pixel");
		this->m_charOneInfo->setHoverTexture("gui_transparent_pixel");
		this->m_charOneInfo->setTextColor(DefaultColor);
		this->m_charOneInfo->setFont(FontHandler::getFont("consolas16"));
		this->m_charOneInfo->setTextAlignment(Quad::TextAlignment::centerAligned);

		this->m_charTwoInfo = Quad::CreateButton(charTwoInfoStr, 0.747f, 0.46f, 0.385f, 0.35f);
		this->m_charTwoInfo->setUnpressedTexture("gui_transparent_pixel");
		this->m_charTwoInfo->setPressedTexture("gui_transparent_pixel");
		this->m_charTwoInfo->setHoverTexture("gui_transparent_pixel");
		this->m_charTwoInfo->setTextColor(DefaultColor);
		this->m_charTwoInfo->setFont(FontHandler::getFont("consolas16"));
		this->m_charTwoInfo->setTextAlignment(Quad::TextAlignment::centerAligned);
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
	if (!isHosting && !hasJoined && !inServerList)
		this->_keyboardMainLobby();
	if (!isHosting && !hasJoined && inServerList)
		this->_keyboardServerList();
	if (isHosting || hasJoined)
		this->_keyboardCharSelection();
}

void LobbyState::_handleMouseInput()
{
	if (!isHosting && !hasJoined)
		_mouseMainLobby();
	if (!isHosting && !hasJoined && (m_hostListButtons.size() > 0))
		_mouseServerList();
	if (isHosting || hasJoined)
		_mouseCharSelection();
}

void LobbyState::_updateSelectionStates()
{
	//update the selection states
	if (!isHosting && !hasJoined && !inServerList)
	{
		for (size_t i = 0; i < m_lobbyButtons.size(); i++)
		{
			if (i != m_currentButton && !m_lobbyButtons[i]->isSelected())
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
			if (i != m_currentButtonServerList && !m_hostListButtons[i]->isSelected())
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
	this->m_ServerName = "None";
	this->selectedHostInfo = "Selected Host:\nNone\n";
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
		content += "Your Host name:\n" + this->m_MyHostName + "\n";
		content += selectedHostInfo + "\n";
	}
	else if (isHosting)
	{
		content += "Your Host name: " + this->m_MyHostName + "\n";
		if (hasClient)
			content += "Connected to client: " + std::string(m_clientIP.ToString(false)) + "\n";
		else
			content += "No client connected\n";
		switch (selectedChar)
		{
		case 0:
			content += "No character selected\n";
			break;
		case 1:
			content += "Selected Character: Lejf\n";
			break;
		case 2:
			content += "Selected Character: Billy\n";
			break;
		}
	}
	else if (hasJoined)
	{
		content += "Your Host name: " + this->m_MyHostName + "\n";
		content += "Connected to server: " + this->m_ServerName + "\n";
		switch (selectedChar)
		{
		case 0:
			content += "No character selected\n";
			break;
		case 1:
			content += "Selected Character: Lejf\n";
			break;
		case 2:
			content += "Selected Character: Billy\n";
			break;
		}
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
			m_currentButtonServerList = 0;
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
	if (GamePadHandler::IsDownDpadPressed() || GamePadHandler::IsUpDpadPressed())
	{
		switch (m_currentButton)
		{
		case CharacterSelection::CharOne:
			m_currentButton = CharacterSelection::Ready;
			break;
		case CharacterSelection::CharTwo:
			m_currentButton = CharacterSelection::Back;
			break;
		case CharacterSelection::Ready:
			m_currentButton = CharacterSelection::CharOne;
			break;
		case CharacterSelection::Back:
			m_currentButton = CharacterSelection::CharTwo;
			break;
		}
	}
	else if (GamePadHandler::IsRightDpadPressed() || GamePadHandler::IsLeftDpadPressed())
	{
		switch (m_currentButton)
		{
		case CharacterSelection::CharOne:
			m_currentButton = CharacterSelection::CharTwo;
			break;
		case CharacterSelection::CharTwo:
			m_currentButton = CharacterSelection::CharOne;
			break;
		case CharacterSelection::Ready:
			m_currentButton = CharacterSelection::Back;
			break;
		case CharacterSelection::Back:
			m_currentButton = CharacterSelection::Ready;
			break;
		}
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
		if (m_currentButtonServerList == 0)
			m_currentButtonServerList = this->m_hostListButtons.size() - 1;
		else
			m_currentButtonServerList--;
	}
	else if (GamePadHandler::IsDownDpadPressed())
	{
		m_currentButtonServerList++;
		m_currentButtonServerList = m_currentButtonServerList % this->m_hostListButtons.size();
	}
	else if (GamePadHandler::IsLeftDpadPressed())
	{
		m_hostListButtons[m_currentButtonServerList]->setState(ButtonStates::Normal);
		m_hostListButtons[m_currentButtonServerList]->Select(false);

		m_currentButton = (unsigned int)ButtonOrderLobby::Host;
		inServerList = false;
	}

	_updateSelectionStates();

	if (GamePadHandler::IsAPressed())
	{
		if (m_hostListButtons[m_currentButtonServerList]->isSelected())
		{
			this->m_hostListButtons[m_currentButtonServerList]->setState(ButtonStates::Pressed);
		}
	}
}

void LobbyState::_keyboardMainLobby()
{
	if (InputHandler::wasKeyPressed(InputHandler::Up))
	{
		if (m_currentButton == 0)
			m_currentButton = (unsigned int)ButtonOrderLobby::Return;
		else
			m_currentButton--;
	}
	else if (InputHandler::wasKeyPressed(InputHandler::Down))
	{
		m_currentButton++;
		m_currentButton = m_currentButton % ((unsigned int)ButtonOrderLobby::Return + 1);
	}
	else if (InputHandler::wasKeyPressed(InputHandler::Right))
	{
		if (m_hostListButtons.size() > 0)
		{
			m_lobbyButtons[m_currentButton]->setState(ButtonStates::Normal);
			m_lobbyButtons[m_currentButton]->Select(false);
			inServerList = true;
			m_currentButtonServerList = 0;
		}
	}

	_updateSelectionStates();

	if (InputHandler::wasKeyPressed(InputHandler::Enter))
	{
		if (m_lobbyButtons[m_currentButton]->isSelected())
			this->m_lobbyButtons[m_currentButton]->setState(ButtonStates::Pressed);
	}
}

void LobbyState::_keyboardCharSelection()
{
	if (InputHandler::wasKeyPressed(InputHandler::Up) || InputHandler::wasKeyPressed(InputHandler::Down))
	{
		switch (m_currentButton)
		{
			case CharacterSelection::CharOne:
				m_currentButton = CharacterSelection::Ready;
				break;
			case CharacterSelection::CharTwo:
				m_currentButton = CharacterSelection::Back;
				break;
			case CharacterSelection::Ready:
				m_currentButton = CharacterSelection::CharOne;
				break;
			case CharacterSelection::Back:
				m_currentButton = CharacterSelection::CharTwo;
				break;
		}
	}
	else if (InputHandler::wasKeyPressed(InputHandler::Right) || InputHandler::wasKeyPressed(InputHandler::Left))
	{
		switch (m_currentButton)
		{
		case CharacterSelection::CharOne:
			m_currentButton = CharacterSelection::CharTwo;
			break;
		case CharacterSelection::CharTwo:
			m_currentButton = CharacterSelection::CharOne;
			break;
		case CharacterSelection::Ready:
			m_currentButton = CharacterSelection::Back;
			break;
		case CharacterSelection::Back:
			m_currentButton = CharacterSelection::Ready;
			break;
		}
	}

	_updateSelectionStates();

	//Check for action input
	if (InputHandler::wasKeyPressed(InputHandler::Enter))
	{
		if (m_charSelectButtons[m_currentButton]->isSelected())
			this->m_charSelectButtons[m_currentButton]->setState(ButtonStates::Pressed);
	}
}

void LobbyState::_keyboardServerList()
{
	if (InputHandler::wasKeyPressed(InputHandler::Up))
	{
		if (m_currentButtonServerList == 0)
			m_currentButtonServerList = this->m_hostListButtons.size() - 1;
		else
			m_currentButtonServerList--;
	}
	else if (InputHandler::wasKeyPressed(InputHandler::Down))
	{
		m_currentButtonServerList++;
		m_currentButtonServerList = m_currentButtonServerList % this->m_hostListButtons.size();
	}
	else if (InputHandler::wasKeyPressed(InputHandler::Left))
	{
		m_hostListButtons[m_currentButtonServerList]->setState(ButtonStates::Normal);
		m_hostListButtons[m_currentButtonServerList]->Select(false);

		m_currentButton = (unsigned int)ButtonOrderLobby::Host;
		inServerList = false;
	}

	_updateSelectionStates();

	if (InputHandler::wasKeyPressed(InputHandler::Enter))
	{
		if (m_hostListButtons[m_currentButtonServerList]->isSelected())
		{
			this->m_hostListButtons[m_currentButtonServerList]->setState(ButtonStates::Pressed);
		}
	}
}

void LobbyState::_mouseMainLobby()
{
	DirectX::XMFLOAT2 mousePos = InputHandler::getMousePosition();
	DirectX::XMINT2 windowSize = InputHandler::getWindowSize();

	mousePos.x /= windowSize.x;
	mousePos.y /= windowSize.y;

	for (size_t i = 0; i < m_lobbyButtons.size(); i++)
	{
		if (m_lobbyButtons[i]->Inside(mousePos))
		{
			if (i != m_currentButton)
			{
				m_lobbyButtons[m_currentButton]->Select(false);
				m_lobbyButtons[m_currentButton]->setState(ButtonStates::Normal);
				m_currentButton = i;
			}
			//set this button to current and on hover state
			m_lobbyButtons[i]->Select(true);
			m_lobbyButtons[i]->setState(ButtonStates::Hover);
			//check if we released this button
			if (m_lobbyButtons[i]->isReleased(mousePos))
			{
				if (inServerList)
					inServerList = false;
				m_currentButton = i;
				m_lobbyButtons[i]->setState(ButtonStates::Pressed);
			}
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
		else
		{
			m_lobbyButtons[i]->Select(false);
			m_lobbyButtons[i]->setState(ButtonStates::Normal);
		}
	}
}

void LobbyState::_mouseCharSelection()
{
	DirectX::XMFLOAT2 mousePos = InputHandler::getMousePosition();
	DirectX::XMINT2 windowSize = InputHandler::getWindowSize();

	mousePos.x /= windowSize.x;
	mousePos.y /= windowSize.y;

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
		else
		{
			m_charSelectButtons[i]->Select(false);
			m_charSelectButtons[i]->setState(ButtonStates::Normal);
		}
	}

}

void LobbyState::_mouseServerList()
{
	DirectX::XMFLOAT2 mousePos = InputHandler::getMousePosition();
	DirectX::XMINT2 windowSize = InputHandler::getWindowSize();

	mousePos.x /= windowSize.x;
	mousePos.y /= windowSize.y;

	for (size_t i = 0; i < m_hostListButtons.size(); i++)
	{
		if (m_hostListButtons[i]->Inside(mousePos))
		{
			m_hostListButtons[i]->Select(true);
			m_hostListButtons[i]->setState(ButtonStates::Hover);
			if (m_hostListButtons[i]->isReleased(mousePos))
			{
				std::string hostName = m_hostListButtons[i]->getString();
				auto it = m_hostAdressMap.find(hostName);
				if (it != m_hostAdressMap.end())
				{
					if (hostName != m_ServerName)
					{
						this->selectedHost = it->second;
						this->selectedHostInfo = "Selected Host:\n" + hostName;
						this->m_ServerName = hostName;
					}
					else
					{
						m_ServerName = "None";
						this->selectedHost = RakNet::SystemAddress("0.0.0.0");
						this->selectedHostInfo = "Selected Host:\nNone\n";
					}
				}
				else
				{
					this->selectedHost = RakNet::SystemAddress("0.0.0.0");
					this->selectedHostInfo = "Selected Host:\nNone\n";
				}
				m_hostListButtons[i]->setState(ButtonStates::Pressed);
			}
			for (size_t j = 0; j < m_hostListButtons.size(); j++)
			{
				if (i != j)
				{
					m_hostListButtons[j]->Select(false);
					m_hostListButtons[j]->setState(ButtonStates::Normal);
				}
			}
			break;
		}
		else
		{
			m_hostListButtons[i]->Select(false);
			m_hostListButtons[i]->setState(ButtonStates::Normal);
		}
	}
}

void LobbyState::_registerThisInstanceToNetwork()
{
	using namespace Network;
	using namespace std::placeholders;

	if (Multiplayer::LobbyOnReceiveMap.size() > 0)
		Multiplayer::LobbyOnReceiveMap.clear();
	//RakNet
	Multiplayer::addToLobbyOnReceiveMap(DefaultMessageIDTypes::ID_ADVERTISE_SYSTEM, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(DefaultMessageIDTypes::ID_NEW_INCOMING_CONNECTION, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(DefaultMessageIDTypes::ID_CONNECTION_REQUEST_ACCEPTED, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(DefaultMessageIDTypes::ID_CONNECTION_ATTEMPT_FAILED, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(DefaultMessageIDTypes::ID_NO_FREE_INCOMING_CONNECTIONS, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	//User specific
	Multiplayer::addToLobbyOnReceiveMap(Network::ID_CHAR_SELECTED, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(Network::ID_READY_PRESSED, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(Network::ID_REQUEST_NID, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(Network::ID_REPLY_NID, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(Network::ID_GAME_STARTED, std::bind(&LobbyState::HandlePacket, this, _1, _2));
	Multiplayer::addToLobbyOnReceiveMap(Network::ID_HOST_NAME, std::bind(&LobbyState::HandlePacket, this, _1, _2));
}

void LobbyState::_onAdvertisePacket(RakNet::Packet * packet)
{
	if (packet->guid == pNetwork->GetMyGUID())
		return;

	uint64_t uniqueHostID = packet->guid.g;
	RakNet::SystemAddress hostAdress = packet->systemAddress;
	Network::LOBBYEVENTPACKET * data = (Network::LOBBYEVENTPACKET*)packet->data;
	std::string hostName = std::string(data->string);
	//Data is shifted by 1 byte because of RakNet, just remove the first byte of the string to fix this
	hostName.erase(0, 1);
	
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
	pNetwork->setIsConnected(true);
	hasClient = true;
	m_clientIP = data->systemAddress;
	//send a request to retrive the NetworkID of the remote machine
	Network::COMMONEVENTPACKET packet(Network::ID_REQUEST_NID, 0);
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::COMMONEVENTPACKET), PacketPriority::LOW_PRIORITY);
	//send a message of the current state of the Character selection
	_sendCharacterSelectionPacket();
}

void LobbyState::_onFailedPacket(RakNet::Packet * data)
{
	m_lobbyButtons[(unsigned int)ButtonOrderLobby::Join]->setState(ButtonStates::Normal);

	selectedHostInfo = "Failed to connect to host\n";
}

void LobbyState::_onSucceedPacket(RakNet::Packet * data)
{
	pNetwork->setIsConnected(true);
	this->selectedHost = data->systemAddress;
	hasJoined = true;
	_resetCharSelectButtonStates();
	this->m_charSelectInfo->setString("You: " + this->m_MyHostName + "\nConnected to:\n" + this->m_ServerName);
	//send a request to retrive the NetworkID of the remote machine
	Network::COMMONEVENTPACKET packet(Network::ID_REQUEST_NID, 0);
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::COMMONEVENTPACKET), PacketPriority::LOW_PRIORITY);
	//send a request to retrive the Character selection
	Network::COMMONEVENTPACKET packet2(Network::ID_REQUEST_SELECTED_CHAR, 0);
	Network::Multiplayer::SendPacket((const char*)&packet2, sizeof(Network::COMMONEVENTPACKET), PacketPriority::LOW_PRIORITY);

	_sendMyHostNamePacket();
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
		this->m_remoteNID = 0;

		hasCharSelected = false;
		selectedChar = 0;
		hasRemoteCharSelected = false;
		remoteSelectedChar = 0;

		m_charSelectButtons[CharOne]->setTextColor(DefaultColor);
		m_charSelectButtons[CharTwo]->setTextColor(DefaultColor);
	}
	else if (isHosting)
	{
		m_clientIP = RakNet::SystemAddress("0.0.0.0");
		hasClient = false;
		isRemoteReady = false;
		this->m_charSelectInfo->setString("You: " + this->m_MyHostName + "\nConnected to:\nNone");
		this->m_remoteNID = 0;

		if (hasRemoteCharSelected)
			m_charSelectButtons[remoteSelectedChar - 1]->setTextColor(DefaultColor);
		hasRemoteCharSelected = false;
		remoteSelectedChar = 0;
	}
	pNetwork->setIsConnected(false);
}

void LobbyState::_onServerDenied(RakNet::Packet * data)
{
	m_lobbyButtons[(unsigned int)ButtonOrderLobby::Join]->setState(ButtonStates::Normal);

	selectedHostInfo = "Server is full\n";
}

void LobbyState::_onCharacterSelectionPacket(RakNet::Packet * data)
{
	Network::CHARACTERSELECTIONPACKET * packet = (Network::CHARACTERSELECTIONPACKET*)data->data;
	switch (packet->selectedChar)
	{
	case 0:
		hasRemoteCharSelected = false;
		remoteSelectedChar = 0;
		if (this->selectedChar == 0)
		{
			m_charSelectButtons[CharOne]->setTextColor(DefaultColor);
			m_charSelectButtons[CharTwo]->setTextColor(DefaultColor);
		}
		else if (this->selectedChar == 1)
		{
			m_charSelectButtons[CharTwo]->setTextColor(DefaultColor);
		}
		else
			m_charSelectButtons[CharOne]->setTextColor(DefaultColor);
		break;
	case 1:
		if (packet->selectedChar != this->selectedChar)
		{
			hasRemoteCharSelected = true;
			remoteSelectedChar = 1;
			m_charSelectButtons[CharOne]->setTextColor(InactivatedColor);
		}
		break;
	case 2:
		if (packet->selectedChar != this->selectedChar)
		{
			hasRemoteCharSelected = true;
			remoteSelectedChar = 2;
			m_charSelectButtons[CharTwo]->setTextColor(InactivatedColor);
		}
	}
}

void LobbyState::_onReadyPacket(RakNet::Packet * data)
{
	if (isRemoteReady)
		isRemoteReady = false;
	else
		isRemoteReady = true;
}
	
void LobbyState::_onGameStartedPacket(RakNet::Packet * data)
{
	Network::GAMESTARTEDPACKET * packet = (Network::GAMESTARTEDPACKET*)data->data;
	if (pCoopData)
	{
		delete pCoopData;
		pCoopData = nullptr;
	}
	pCoopData = new CoopData();
	pCoopData->seed = packet->seed;
	pCoopData->localPlayerCharacter = selectedChar;
	pCoopData->remotePlayerCharacter = remoteSelectedChar;
	pCoopData->remoteID = packet->remoteID;
	pCoopData->role = Role::Client;

	isReady = false;
	isRemoteReady = false;
	//loading screen stuff
	{
		delete m_charOneInfo;
		m_charOneInfo = nullptr;
		delete m_charTwoInfo;
		m_charTwoInfo = nullptr;
		delete m_charSelectInfo;
		m_charSelectInfo = nullptr;
		delete m_charSelectionBG;
		m_charSelectionBG = nullptr;
		m_loadingScreen.removeGUI(this->m_charSelectButtons);
		m_loadingScreen.draw();
	}
	this->pushNewState(new PlayState(this->p_renderingManager, (void*)pCoopData));
}
void LobbyState::_onRequestPacket(unsigned char id, RakNet::Packet * data)
{
	if (id == Network::ID_REQUEST_NID)
	{
		//Reply with our NID
		Network::COMMONEVENTPACKET packet(Network::ID_REPLY_NID, pNetwork->GetNetworkID());
		Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::COMMONEVENTPACKET), PacketPriority::LOW_PRIORITY);
	}
	else if (id == Network::ID_REQUEST_SELECTED_CHAR)
	{
		_sendCharacterSelectionPacket();
	}
}

void LobbyState::_onReplyPacket(RakNet::Packet * data)
{
	Network::COMMONEVENTPACKET * packet = (Network::COMMONEVENTPACKET*)data->data;
	this->m_remoteNID = packet->nid;
}

void LobbyState::_onHostNamePacket(RakNet::Packet * data)
{
	Network::LOBBYEVENTPACKET* packet = (Network::LOBBYEVENTPACKET*)data->data;
	std::string name = std::string(packet->string);
	this->m_charSelectInfo->setString("You: " + this->m_MyHostName + "\nConnected to:\n" + name);
}

void LobbyState::_sendCharacterSelectionPacket()
{
	if (hasClient || hasJoined)
	{
		Network::CHARACTERSELECTIONPACKET packet;
		packet.id = Network::ID_CHAR_SELECTED;
		packet.selectedChar = this->selectedChar;
		if (isHosting)
			packet.role = Role::Server;
		else
			packet.role = Role::Client;
		Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::CHARACTERSELECTIONPACKET), PacketPriority::LOW_PRIORITY);
	}
}

void LobbyState::_sendReadyPacket()
{
	if (hasClient || hasJoined)
	{
		Network::COMMONEVENTPACKET packet(Network::ID_READY_PRESSED, 0);
		Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::COMMONEVENTPACKET), PacketPriority::LOW_PRIORITY);
	}

}

void LobbyState::_sendGameStartedPacket()
{
	Network::GAMESTARTEDPACKET packet(Network::ID_GAME_STARTED, pNetwork->GenerateSeed(), pNetwork->GetNetworkID());
	
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::GAMESTARTEDPACKET), PacketPriority::IMMEDIATE_PRIORITY);
}

void LobbyState::_sendMyHostNamePacket()
{
	Network::LOBBYEVENTPACKET packet(Network::ID_HOST_NAME, this->m_MyHostName);
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::LOBBYEVENTPACKET), PacketPriority::LOW_PRIORITY);
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
	m_hostListButtons[size]->setUnpressedTexture(("gui_transparent_pixel"));
	m_hostListButtons[size]->setPressedTexture(("gui_hover_pixel"));
	m_hostListButtons[size]->setHoverTexture(("gui_pressed_pixel"));
	m_hostListButtons[size]->setTextColor(DefaultColor);
	m_hostListButtons[size]->setFont(FontHandler::getFont("consolas16"));
}

void LobbyState::Load()
{
	_initButtons();
	m_currentButton = (unsigned int)ButtonOrderLobby::Host;

	m_loadingScreen.Init();

	this->pNetwork = Network::Multiplayer::GetInstance();
	this->pNetwork->Init();
	this->pNetwork->StartUpPeer();
	this->m_MySysAdress = pNetwork->GetMySysAdress();
	//INITIAL RANDOM HOST NAME
	srand(time(0));
	this->m_MyHostName = "Host:" + std::to_string(rand());
	this->m_adPacket = Network::LOBBYEVENTPACKET(Network::ID_SERVER_ADVERTISE, this->m_MyHostName);

	this->_registerThisInstanceToNetwork();

	std::cout << "Lobby Load" << std::endl;
}

void LobbyState::unLoad()
{
	Network::Multiplayer::LobbyOnReceiveMap.clear();
	std::cout << "Lobby unLoad" << std::endl;
}

