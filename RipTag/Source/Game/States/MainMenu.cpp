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

	quitButton = new Quad();
	quitButton->init();
	quitButton->setPosition(0.5f, 0.25f);
	quitButton->setScale(0.4f, 0.2f);

	quitButton->setString("Quit Game");
	quitButton->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	quitButton->setPressedTexture(Manager::g_textureManager.getTexture("KOMBIN"));
	quitButton->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	quitButton->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	quitButton->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
}

MainMenu::~MainMenu()
{
	playButton->Release();
	delete playButton;

	quitButton->Release();
	delete quitButton;
}

void MainMenu::Update(double deltaTime)
{
	if (InputHandler::getShowCursor() != TRUE)
		InputHandler::setShowCursor(TRUE);

	if (playButton->isReleased(DirectX::XMFLOAT2(InputHandler::getMousePosition().x / 1920, InputHandler::getMousePosition().y / 1080)))
		pushNewState(new PlayState(this->p_renderingManager));

	if (quitButton->isReleased(DirectX::XMFLOAT2(InputHandler::getMousePosition().x / 1920, InputHandler::getMousePosition().y / 1080)))
		PostQuitMessage(0);
	
}

void MainMenu::Draw()
{
	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);
	camera.setPosition(0, 0, -10);
	
	playButton->Draw();
	quitButton->Draw();
	p_renderingManager->Flush(camera);
}
