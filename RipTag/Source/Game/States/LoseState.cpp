#include "RipTagPCH.h"
#include "LoseState.h"


LoseState::LoseState(RenderingManager * rm) : State(rm)
{
	this->m_youLost = Quad::CreateButton("You lost", 0.5f, 0.5f, 0.5f, 0.25f);
	this->m_youLost->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	this->m_youLost->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	this->m_youLost->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	this->m_youLost->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	this->m_youLost->setFont(FontHandler::getFont("consolas32"));


	this->m_backToMenu = Quad::CreateButton("Play Game", 0.5f, 0.25f, 0.5f, 0.25f);
	this->m_backToMenu->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	this->m_backToMenu->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	this->m_backToMenu->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	this->m_backToMenu->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	this->m_backToMenu->setFont(FontHandler::getFont("consolas16"));
	this->m_backToMenu->setString("Back To\nMenu");
}


LoseState::~LoseState()
{
	m_youLost->Release();
	delete m_youLost;
	m_backToMenu->Release();
	delete m_backToMenu;
}

void LoseState::Update(double deltaTime)
{
	if (!InputHandler::getShowCursor())
		InputHandler::setShowCursor(TRUE);
	if (m_backToMenu->isReleased(DirectX::XMFLOAT2(InputHandler::getMousePosition().x / InputHandler::getWindowSize().x, InputHandler::getMousePosition().y / InputHandler::getWindowSize().y)))
	{
		BackToMenu();
	}
	
}

void LoseState::Draw()
{
	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);

	m_youLost->Draw();
	m_backToMenu->Draw();

	p_renderingManager->Flush(camera);
}

void LoseState::Load()
{
	std::cout << "Loose Load" << std::endl;
}

void LoseState::unLoad()
{
	std::cout << "Loose unLoad" << std::endl;
}
