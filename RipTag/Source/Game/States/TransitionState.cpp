#include "RipTagPCH.h"
#include "TransitionState.h"


TransitionState::TransitionState(RenderingManager * rm, Transition type, std::string eventString, void * pCoopData, bool partnerLost) : State(rm)
{
	this->m_type = type;
	m_eventString = eventString;
	this->pCoopData = pCoopData;
	m_partnerLost = partnerLost;
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
		if (m_currentButton > m_maxButtons)
			InputHandler::setShowCursor(TRUE);
	//Back to menu
	if (m_backToMenu->isReleased(DirectX::XMFLOAT2(InputHandler::getMousePosition().x / InputHandler::getWindowSize().x, InputHandler::getMousePosition().y / InputHandler::getWindowSize().y)) || (m_currentButton == 0 && m_pressed))
	{
		backToMenu = true;
		BackToMenu();
	}

	if (m_ready->isReleased(DirectX::XMFLOAT2(InputHandler::getMousePosition().x / InputHandler::getWindowSize().x, InputHandler::getMousePosition().y / InputHandler::getWindowSize().y)) || (m_currentButton == 1 && m_pressed))
	{
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

	if (m_type == Transition::Lose)
	{
		if (pCoopData)
		{
			if (isReady && isRemoteReady)
			{
				this->pushAndPop(2, new PlayState(p_renderingManager, pCoopData));
			}
		}
		else if (isReady)
			this->pushAndPop(2, new PlayState(p_renderingManager));

	}

	if (InputHandler::mouseMoved())
		m_currentButton = 16;

	if (GamePadHandler::IsLeftDpadPressed() || GamePadHandler::IsDownDpadPressed() || InputHandler::isKeyReleased(InputHandler::Key::Left))
	{
		if (m_currentButton > m_maxButtons)
			m_currentButton = 1;
		if (m_currentButton - 1 < 0)
			m_currentButton = m_maxButtons - 1;
		else
			m_currentButton--;
	}
	if (GamePadHandler::IsRightDpadPressed() || GamePadHandler::IsUpDpadPressed() || InputHandler::isKeyReleased(InputHandler::Key::Right))
	{
		if (m_currentButton + 1 >= m_maxButtons)
			m_currentButton = 0;
		else
			m_currentButton++;
	}
	if (m_currentButton == 0)
	{
		m_backToMenu->setTextColor(Colors::Purple);
	}
	else
	{
		m_backToMenu->setTextColor(Colors::White);
	}
	if (m_currentButton == 1)
	{
		m_ready->setTextColor(Colors::Purple);
	}
	else
	{
		m_ready->setTextColor(Colors::White);
	}
	if (GamePadHandler::IsAReleased() || InputHandler::isKeyReleased(InputHandler::Key::Return))	
		m_pressed = true;	
	else
		m_pressed = false;
}

void TransitionState::Draw()
{
	static Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);

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
	if (m_background)
		m_background->Draw();

	p_renderingManager->Flush(camera);
}

void TransitionState::Load()
{
	std::cout << "Lose State Load" << std::endl;
	if (pCoopData)
	{
		CoopData * data = (CoopData*)pCoopData;
		if (data->role == 0)
			isServer = true;
		else
			isClient = true;
		this->_registerThisInstanceToNetwork();
	}
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
				std::cout << "Attempting to load: " << file.stem().generic_string() << "\n";
				if (extension == L".DDS" || extension == L".DDS")
					Manager::g_textureManager.loadGUITexture(stem, file.generic_wstring());
			}
		}


		//std::cout << p.path().generic_string() << std::endl;
	}
	_initButtons();
}

void TransitionState::unLoad()
{
	Manager::g_textureManager.UnloadGUITextures();

	std::cout << "Lose State unLoad" << std::endl;
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


		this->m_eventInfo = Quad::CreateButton(m_eventString, 0.5, 0.5f, 0.7f, 0.7f);
		this->m_eventInfo->setUnpressedTexture("gui_transparent_pixel");
		this->m_eventInfo->setPressedTexture("gui_transparent_pixel");
		this->m_eventInfo->setHoverTexture("gui_transparent_pixel");
		this->m_eventInfo->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_eventInfo->setFont(FontHandler::getFont("consolas32"));
		this->m_eventInfo->setTextColor(Colors::White);

		if (m_type == Transition::ThankYou)
			this->m_backToMenu = Quad::CreateButton("Back To Menu", 0.5f, 0.20f, 0.5f, 0.25f);
		else
			this->m_backToMenu = Quad::CreateButton("Back To Menu", 0.3f, 0.20f, 0.5f, 0.25f);
		this->m_backToMenu->setUnpressedTexture("gui_pressed_pixel");
		this->m_backToMenu->setPressedTexture("gui_pressed_pixel");
		this->m_backToMenu->setHoverTexture("gui_pressed_pixel");
		this->m_backToMenu->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_backToMenu->setFont(FontHandler::getFont("consolas16"));

		this->m_ready = Quad::CreateButton("Ready", 0.6f, 0.20f, 0.5f, 0.25f);
		this->m_ready->setUnpressedTexture("gui_pressed_pixel");
		this->m_ready->setPressedTexture("gui_pressed_pixel");
		this->m_ready->setHoverTexture("gui_pressed_pixel");
		this->m_ready->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_ready->setFont(FontHandler::getFont("consolas16"));
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
