#include "RipTagPCH.h"
#include "LoseState.h"


LoseState::LoseState(RenderingManager * rm, std::string eventString, void * pCoopData = nullptr) : State(rm)
{
	m_eventString = eventString;
	this->pCoopData = pCoopData;
}


LoseState::~LoseState()
{
	if (m_gameOver)
	{
		m_gameOver->Release();
		delete m_gameOver;
		m_gameOver = nullptr;
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
	if (m_Retry)
	{
		m_Retry->Release();
		delete m_Retry;
		m_Retry = nullptr;
	}
	if (m_backGround)
	{
		m_backGround->Release();
		delete m_backGround;
		m_backGround = nullptr;
	}
}

void LoseState::Update(double deltaTime)
{
	if (!InputHandler::getShowCursor())
		InputHandler::setShowCursor(TRUE);
	//Back to menu
	if (m_backToMenu->isReleased(DirectX::XMFLOAT2(InputHandler::getMousePosition().x / InputHandler::getWindowSize().x, InputHandler::getMousePosition().y / InputHandler::getWindowSize().y)))
	{
		BackToMenu();
	}
	if (InputHandler::wasKeyPressed(InputHandler::Enter) || InputHandler::wasKeyPressed(InputHandler::Esc))
		BackToMenu();
	if (Input::isUsingGamepad())
		if (GamePadHandler::IsAPressed() || GamePadHandler::IsBPressed())
			BackToMenu();
	//Retry
	if (m_Retry->isReleased(DirectX::XMFLOAT2(InputHandler::getMousePosition().x / InputHandler::getWindowSize().x, InputHandler::getMousePosition().y / InputHandler::getWindowSize().y)))
	{
		if (isReady)
		{
			isReady = false;
			m_Retry->setTextColor(Colors::White);
		}
		else
		{
			isReady = true;
			m_Retry->setTextColor(Colors::Green);
		}
		if (pCoopData)
		{
			_sendReadyPacket();
		}
	}

	if (pCoopData)
	{
		if (isReady && isRemoteReady)
		{
			this->pushNewState(new PlayState(p_renderingManager, pCoopData));
		}
	}
	else if (isReady)
		this->pushNewState(new PlayState(p_renderingManager));
	
}

void LoseState::Draw()
{
	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);

	if (m_gameOver)
		m_gameOver->Draw();
	if (m_eventInfo)
		m_eventInfo->Draw();
	if (m_backToMenu)
		m_backToMenu->Draw();
	if (m_Retry)
		m_Retry->Draw();
	if (m_backGround)
		m_backGround->Draw();

	p_renderingManager->Flush(camera);
}

void LoseState::Load()
{
	std::cout << "Loose Load" << std::endl;
	if (pCoopData)
	{
		CoopData * data = (CoopData*)pCoopData;
		if (data->role == 0)
			isServer = true;
		else
			isClient = true;
	}
	_initButtons();
}

void LoseState::unLoad()
{

	std::cout << "Loose unLoad" << std::endl;
}

void LoseState::_initButtons()
{
	//Buttons
	{
		this->m_gameOver = Quad::CreateButton("Game Over", 0.5f, 0.7f, 0.5f, 0.25f);
		this->m_gameOver->setUnpressedTexture("gui_transparent_pixel");
		this->m_gameOver->setPressedTexture("gui_transparent_pixel");
		this->m_gameOver->setHoverTexture("gui_transparent_pixel");
		this->m_gameOver->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_gameOver->setFont(FontHandler::getFont("consolas32"));
		this->m_gameOver->setTextColor(Colors::Red);

		this->m_eventInfo = Quad::CreateButton(m_eventString, 0.5, 0.5f, 0.7f, 0.7f);
		this->m_eventInfo->setUnpressedTexture("gui_transparent_pixel");
		this->m_eventInfo->setPressedTexture("gui_transparent_pixel");
		this->m_eventInfo->setHoverTexture("gui_transparent_pixel");
		this->m_eventInfo->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_eventInfo->setFont(FontHandler::getFont("consolas32"));
		this->m_eventInfo->setTextColor(Colors::White);

		this->m_backToMenu = Quad::CreateButton("Back To Menu", 0.3f, 0.20f, 0.5f, 0.25f);
		this->m_backToMenu->setUnpressedTexture("gui_pressed_pixel");
		this->m_backToMenu->setPressedTexture("gui_pressed_pixel");
		this->m_backToMenu->setHoverTexture("gui_pressed_pixel");
		this->m_backToMenu->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_backToMenu->setFont(FontHandler::getFont("consolas16"));

		this->m_Retry = Quad::CreateButton("Retry", 0.6f, 0.20f, 0.5f, 0.25f);
		this->m_Retry->setUnpressedTexture("gui_pressed_pixel");
		this->m_Retry->setPressedTexture("gui_pressed_pixel");
		this->m_Retry->setHoverTexture("gui_pressed_pixel");
		this->m_Retry->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
		this->m_Retry->setFont(FontHandler::getFont("consolas16"));
	}
	//Background
	{
		this->m_backGround = Quad::CreateButton("", 0.5f, 0.5f, 2.0f, 2.0f);
		this->m_backGround->setPivotPoint(Quad::PivotPoint::center);
		this->m_backGround->setUnpressedTexture("gui_temp_bg");
		this->m_backGround->setPressedTexture("gui_temp_bg");
		this->m_backGround->setHoverTexture("gui_temp_bg");
	}
	


}
