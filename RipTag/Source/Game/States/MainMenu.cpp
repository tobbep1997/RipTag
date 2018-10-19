#include "MainMenu.h"
#include "PlayState.h"
MainMenu::MainMenu(RenderingManager * rm) : State(rm)
{
	Manager::g_textureManager.loadTextures("KOMBIN");
	Manager::g_textureManager.loadTextures("SPHERE");
	Manager::g_textureManager.loadTextures("PIRASRUM");

	playButton = new Quad();
	playButton->init();
	playButton->setPosition(0.5f, 0.5f);
	playButton->setScale(0.5f, 0.25f);

	playButton->setString("Play Game");
	playButton->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	playButton->setPressedTexture(Manager::g_textureManager.getTexture("KOMBIN"));
	playButton->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	playButton->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	playButton->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
}

MainMenu::~MainMenu()
{
	playButton->Release();
	delete playButton;
}

void MainMenu::Update(double deltaTime)
{
	if (InputHandler::getShowCursor() != TRUE)
		InputHandler::setShowCursor(TRUE);

	m_playCurrent = playButton->isPressed(DirectX::XMFLOAT2(InputHandler::getMousePosition().x / 1280, InputHandler::getMousePosition().y / 720));
	if (m_playCurrent == false && m_playPrev == true && playButton->Inside(DirectX::XMFLOAT2(InputHandler::getMousePosition().x / 1280, InputHandler::getMousePosition().y / 720)))
		pushNewState(new PlayState(this->p_renderingManager));
	m_playPrev = m_playCurrent;
}

void MainMenu::Draw()
{
	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);
	camera.setPosition(0, 0, -10);
	
	playButton->Draw();

	p_renderingManager->Flush(camera);
}
