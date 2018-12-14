#include "RipTagPCH.h"
#include "TransitionState.h"


TransitionState::TransitionState(RenderingManager * rm, Transition type, std::string eventString, void * pCoopData, int currentRoom, bool partnerLost) : State(rm)
{
	this->m_type = type;
	m_eventString = eventString;
	this->pCoopData = pCoopData;
	m_partnerLost = partnerLost;
	m_currentRoom = currentRoom; 
}


TransitionState::~TransitionState()
{
	if (m_header)
	{
		m_header->Release();
		delete m_header;
		m_header = nullptr;
	}
	if (m_eventInfo)
	{
		m_eventInfo->Release();
		delete m_eventInfo;
		m_eventInfo = nullptr;
	}
	if (m_backToMenu)
	{
		m_backToMenu->Release();
		delete m_backToMenu;
		m_backToMenu = nullptr;
	}
	if (m_ready)
	{
		m_ready->Release();
		delete m_ready;
		m_ready = nullptr;
	}
	if (m_background)
	{
		m_background->Release();
		delete m_background;
		m_background = nullptr;
	}
	if (m_victory)
	{
		m_victory->Release();
		delete m_victory;
		m_victory = nullptr;
	}

}

void TransitionState::Update(double deltaTime)
{
	if (!InputHandler::getShowCursor())
			InputHandler::setShowCursor(TRUE);

	m_inputTimer += deltaTime;

	_handleMouseInput();
	_handleKeyboardInput();
	_handleGamepadInput();

	_buttonStateCheck();

	if (m_type == Transition::Lose || m_type	== Transition::Win)
	{
		if (pCoopData)
		{
			if (isReady && isRemoteReady)
			{
				this->pushAndPop(2, new PlayState(p_renderingManager, pCoopData, m_currentRoom)); 
			}
		}
		else if (isReady)
		{
			this->pushAndPop(2, new PlayState(p_renderingManager, pCoopData, m_currentRoom)); 
		}
	}
}

void TransitionState::Draw()
{
	static Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);
	//WE HAVE NO Z ORDERING PLEASE DO THE DRAW ORDER YOURSELF PROPERLY, jesus christ
	if (m_background)
		m_background->Draw();
	if (m_header)
		m_header->Draw();
	if (m_eventInfo)
		m_eventInfo->Draw();
	if (m_backToMenu)
		m_backToMenu->Draw();
	if (m_ready)
	{
		if (m_type != Transition::ThankYou)
			m_ready->Draw();
	}

	p_renderingManager->Flush(camera);
}

void TransitionState::Load()
{
	if (pCoopData)
	{
		CoopData * data = (CoopData*)pCoopData;
		if (data->role == 0)
			isServer = true;
		else
			isClient = true;
		this->_registerThisInstanceToNetwork();
	}
	_loadTextures();
	_initButtons();
}

void TransitionState::unLoad()
{
	Manager::g_textureManager.UnloadGUITextures();
}

void TransitionState::HandlePacket(unsigned char id, unsigned char * data)
{
	switch (id)
	{
	case Network::ID_READY_PRESSED:
		_onReadyPacket();
		break;
	case Network::ID_PLAYER_DISCONNECT:
		_onDisconnectPacket();
		break;
	case DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION:
		_onDisconnectPacket();
		break;
	}
}

bool TransitionState::ReadyToLoadNextRoom()
{
	if (pCoopData)
		return isReady && isRemoteReady;
	else
		return isReady;
}

bool TransitionState::BackToMenuBool()
{
	return backToMenu;
}

void TransitionState::_initButtons()
{
	//Buttons
	{
		if (m_type == Transition::Lose)
		{
			this->m_header = Quad::CreateButton("", 0.5f, 0.7f, 0.5f, 0.25f);
			this->m_header->setTextColor(Colors::Red);
			
		}
		else if (m_type == Transition::Win)
		{
			this->m_header = Quad::CreateButton("", 0.5f, 0.7f, 0.5f, 0.25f);
			this->m_header->setTextColor(Colors::Green);
		}
		else
		{
			this->m_header = Quad::CreateButton("Thank you!", 0.5f, 0.7f, 0.5f, 0.25f);
			this->m_header->setTextColor(Colors::Gold);
		}
		this->m_header->setUnpressedTexture("gui_transparent_pixel");
		this->m_header->setPressedTexture("gui_transparent_pixel");
		this->m_header->setHoverTexture("gui_transparent_pixel");
		this->m_header->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_header->setFont(FontHandler::getFont("consolas32"));


		this->m_eventInfo = Quad::CreateButton("", 0.5, 0.5f, 0.7f, 0.7f);
		this->m_eventInfo->setUnpressedTexture("gui_transparent_pixel");
		this->m_eventInfo->setPressedTexture("gui_transparent_pixel");
		this->m_eventInfo->setHoverTexture("gui_transparent_pixel");
		this->m_eventInfo->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_eventInfo->setFont(FontHandler::getFont("consolas32"));
		this->m_eventInfo->setTextColor(Colors::White);

		if (m_type == Transition::ThankYou)
			this->m_backToMenu = Quad::CreateButton("Back To Menu", 0.5f, 0.20f, 0.5f, 0.25f);
		else
			this->m_backToMenu = Quad::CreateButton("Back To Menu", 0.35f, 0.20f, 0.5f, 0.25f);
		this->m_backToMenu->setUnpressedTexture("gui_hover_pixel");
		this->m_backToMenu->setPressedTexture("gui_pressed_pixel");
		this->m_backToMenu->setHoverTexture("pause_menu_background");
		this->m_backToMenu->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_backToMenu->setFont(FontHandler::getFont("consolas32"));

		this->m_ready = Quad::CreateButton("Ready", 0.65f, 0.20f, 0.5f, 0.25f);
		this->m_ready->setUnpressedTexture("gui_hover_pixel");
		this->m_ready->setPressedTexture("gui_pressed_pixel");
		this->m_ready->setHoverTexture("pause_menu_background");
		this->m_ready->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_ready->setFont(FontHandler::getFont("consolas32"));
		this->m_ready->setState(ButtonStates::Hover);
	}
	//Background
	{
		this->m_background = Quad::CreateButton("", 0.5f, 0.5f, 2.0f, 2.0f);
		this->m_background->setPivotPoint(Quad::PivotPoint::center);
		if (m_type == Transition::Lose && m_partnerLost == false)
		{
			this->m_background->setUnpressedTexture("ENDGAME");
			this->m_background->setPressedTexture("ENDGAME");
			this->m_background->setHoverTexture("ENDGAME");
		}
		else if (m_type == Transition::Lose && m_partnerLost == true)
		{
			this->m_background->setUnpressedTexture("gui_temp_bg");
			this->m_background->setPressedTexture("gui_temp_bg");
			this->m_background->setHoverTexture("gui_temp_bg");
		}
		else
		{
			this->m_background->setUnpressedTexture("gui_temp_bg1");
			this->m_background->setPressedTexture("gui_temp_bg1");
			this->m_background->setHoverTexture("gui_temp_bg1");
		}
	}
}

void TransitionState::_loadTextures()
{
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
				if (extension == L".DDS")
					Manager::g_textureManager.loadGUITexture(stem, file.generic_wstring());
			}
		}
	}
}

void TransitionState::_registerThisInstanceToNetwork()
{
	using namespace Network;
	using namespace std::placeholders;

	Network::Multiplayer::RemotePlayerOnReceiveMap.clear();

	Multiplayer::addToOnReceiveFuncMap(ID_READY_PRESSED, std::bind(&TransitionState::HandlePacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(ID_PLAYER_DISCONNECT, std::bind(&TransitionState::HandlePacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION, std::bind(&TransitionState::HandlePacket, this, _1, _2));
}

void TransitionState::_onDisconnectPacket()
{
	this->m_eventInfo->setString("Your partner has abandoned you...\nYou are on your own now!");
	pCoopData = nullptr;
}

void TransitionState::_onReadyPacket()
{
	if (isRemoteReady)
		isRemoteReady = false;
	else
		isRemoteReady = true;
}

void TransitionState::_sendDisconnectPacket()
{
	Network::COMMONEVENTPACKET packet(Network::ID_PLAYER_DISCONNECT);
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::COMMONEVENTPACKET), PacketPriority::IMMEDIATE_PRIORITY);
}

void TransitionState::_sendReadyPacket()
{
	Network::COMMONEVENTPACKET packet(Network::ID_READY_PRESSED);
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::COMMONEVENTPACKET), PacketPriority::IMMEDIATE_PRIORITY);
}

void TransitionState::_handleMouseInput()
{
	DirectX::XMFLOAT2 mousePos = InputHandler::getMousePosition();
	DirectX::XMINT2 windowSize = InputHandler::getWindowSize();

	mousePos.x /= windowSize.x;
	mousePos.y /= windowSize.y;

	bool moved = InputHandler::mouseMoved();

	if (m_backToMenu->Inside(mousePos))
	{
		if (moved)
		{
			m_currentButton = Quit;
			m_backToMenu->setState(ButtonStates::Hover);
		}

		if (InputHandler::isMLeftPressed(true))
		{
			m_currentButton = Quit;
			m_backToMenu->setState(ButtonStates::Pressed);
		}
		if (m_backToMenu->isReleased(mousePos))
		{
			m_currentButton = Quit;
			m_backToMenu->setTextColor(Colors::Red);
			this->BackToMenu();
		}
	}
	else if (!m_backToMenu->isSelected())
	{
		m_backToMenu->setState(ButtonStates::Normal);
	}

	if (m_ready->Inside(mousePos))
	{
		if (moved)
		{
			m_currentButton = Ready;
			m_ready->setState(ButtonStates::Hover);
		}

		if (InputHandler::isMLeftPressed(true))
		{
			m_currentButton = Ready;
			m_ready->setState(ButtonStates::Pressed);
		}
		if (m_ready->isReleased(mousePos))
		{
			m_currentButton = Ready;
			if (isReady)
			{
				isReady = false;
				m_ready->setTextColor(Colors::White);
			}
			else
			{
				isReady = true;
				m_ready->setTextColor(Colors::Green);
			}
			if (pCoopData)
			{
				_sendReadyPacket();
			}
		}
	}
	else if (!m_ready->isSelected())
	{
		m_ready->setState(ButtonStates::Normal);
	}
}

void TransitionState::_handleKeyboardInput()
{
	bool left = InputHandler::wasKeyPressed(InputHandler::Left);
	bool right = InputHandler::wasKeyPressed(InputHandler::Right);

	if (InputHandler::isKeyPressed(InputHandler::Left) && m_inputTimer >= m_timerLimit)
	{
		left = true;
		m_inputTimer = 0.0;
	}
	if (InputHandler::isKeyPressed(InputHandler::Right) && m_inputTimer >= m_timerLimit)
	{
		right = true;
		m_inputTimer = 0.0;
	}

	if (left || right)
		m_currentButton = (m_currentButton + 1) % m_maxButtons;

	bool enter = InputHandler::wasKeyPressed(InputHandler::Enter);

	if (enter)
	{
		switch (m_currentButton)
		{
		case Quit:
			m_backToMenu->setTextColor(Colors::Red);
			this->BackToMenu();
			break;
		case Ready:
			if (isReady)
			{
				isReady = false;
				m_ready->setTextColor(Colors::White);
			}
			else
			{
				isReady = true;
				m_ready->setTextColor(Colors::Green);
			}
			if (pCoopData)
			{
				_sendReadyPacket();
			}
			break;
		}
	}

}

void TransitionState::_handleGamepadInput()
{
	bool left = GamePadHandler::IsLeftDpadPressed();
	bool right = GamePadHandler::IsRightDpadPressed();

	if ((GamePadHandler::GetLeftStickXPosition() > 0) && m_inputTimer >= m_timerLimit)
	{
		left = true;
		m_inputTimer = 0.0;
	}
	if ((GamePadHandler::GetLeftStickXPosition() < 0) && m_inputTimer >= m_timerLimit)
	{
		right = true;
		m_inputTimer = 0.0;
	}

	if (left || right)
		m_currentButton = (m_currentButton + 1) % m_maxButtons;

	bool enter = GamePadHandler::IsAReleased();

	if (enter)
	{
		switch (m_currentButton)
		{
		case Quit:
			m_backToMenu->setTextColor(Colors::Red);
			this->BackToMenu();
			break;
		case Ready:
			if (isReady)
			{
				isReady = false;
				m_ready->setTextColor(Colors::White);
			}
			else
			{
				isReady = true;
				m_ready->setTextColor(Colors::Green);
			}
			if (pCoopData)
			{
				_sendReadyPacket();
			}
			break;
		}
	}
}

void TransitionState::_buttonStateCheck()
{
	switch (m_currentButton)
	{
	case ButtonOrder::Quit:
		if (m_backToMenu->getState() != ButtonStates::Pressed)
		{
			m_backToMenu->setState(ButtonStates::Hover);
			m_ready->setState(ButtonStates::Normal);
		}
		break;
	case ButtonOrder::Ready:
		if (m_ready->getState() != ButtonStates::Pressed)
		{
			m_ready->setState(ButtonStates::Hover);
			m_backToMenu->setState(ButtonStates::Normal);
		}
		break;
	}
}
