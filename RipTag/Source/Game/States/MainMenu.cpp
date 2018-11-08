#include "RipTagPCH.h"
#include "MainMenu.h"


MainMenu::MainMenu(RenderingManager * rm) : State(rm)
{
	Manager::g_textureManager.loadTextures("KOMBIN");
	Manager::g_textureManager.loadTextures("SPHERE");
	Manager::g_textureManager.loadTextures("PIRASRUM");
	Manager::g_textureManager.loadTextures("DAB");


	playButton = new Quad();
	playButton->init();
	playButton->setPosition(0.5f, 0.5f);
	playButton->setScale(0.5f, 0.25f);

	playButton->setString("Play Game");
	playButton->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	playButton->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	playButton->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	playButton->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	playButton->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));

	quitButton = new Quad();
	quitButton->init();
	quitButton->setPosition(0.5f, 0.25f);
	quitButton->setScale(0.4f, 0.2f);

	quitButton->setString("Quit Game");
	quitButton->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	quitButton->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	quitButton->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	quitButton->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	quitButton->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));

	c = new Circle();
	c->init(DirectX::XMFLOAT2A(.25, .5), DirectX::XMFLOAT2A(2.f / 16.0f, 2.f / 9.0f));
	c->setUnpressedTexture(Manager::g_textureManager.getTexture("DAB"));

	m_textInput = new TextInput();
	m_textInput->init(DirectX::XMFLOAT2A(0.5f, .75f), DirectX::XMFLOAT2A(2.0f, 0.2f));
	m_textInput->getQuad()->setTextAlignment(Quad::TextAlignment::leftAligned);
	m_textInput->getQuad()->setUnpressedTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_textInput->getQuad()->setPressedTexture(Manager::g_textureManager.getTexture("DAB"));
	m_textInput->getQuad()->setHoverTexture(Manager::g_textureManager.getTexture("PIRASRUM"));
	m_textInput->getQuad()->setFont(new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont"));
	m_textInput->getQuad()->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
}

MainMenu::~MainMenu()
{
	playButton->Release();
	delete playButton;

	quitButton->Release();
	delete quitButton;

	c->Release();
	delete c;

	m_textInput->getQuad()->Release();
	delete m_textInput;
}
#include "InputManager/XboxInput/GamePadHandler.h"
void MainMenu::Update(double deltaTime)
{
	if (InputHandler::getShowCursor() != TRUE)
		InputHandler::setShowCursor(TRUE);

	if (Input::isUsingGamepad())
	{
		if (GamePadHandler::IsUpDpadPressed())
		{
			playButton->Select(true);
			playButton->setState(1);
			quitButton->Select(false);
			quitButton->setState(0);
		}
		else if (GamePadHandler::IsDownDpadPressed())
		{
			playButton->Select(false);
			playButton->setState(0);

			quitButton->Select(true);
			quitButton->setState(1);

		}
		if (GamePadHandler::IsAPressed())
		{
			if (playButton->isSelected())
			{
				playButton->setState(2);
				pushNewState(new PlayState(this->p_renderingManager));
			}
			else if (quitButton->isSelected())
			{
				quitButton->setState(2);
				InputHandler::CloseGame();
				PostQuitMessage(0);
			}
		}
	}
	else
	{
		if (playButton->isReleased(DirectX::XMFLOAT2(InputHandler::getMousePosition().x / InputHandler::getWindowSize().x, InputHandler::getMousePosition().y / InputHandler::getWindowSize().y)))
			pushNewState(new PlayState(this->p_renderingManager));

		if (quitButton->isReleased(DirectX::XMFLOAT2(InputHandler::getMousePosition().x / InputHandler::getWindowSize().x, InputHandler::getMousePosition().y / InputHandler::getWindowSize().y)))
		{
			InputHandler::CloseGame();
			PostQuitMessage(0);
		}
			
	}

	
	cTimer += deltaTime;
	double radie = (sin(10 * cTimer)*sin(cTimer * 0.5) + 1.5)*0.175;
	c->setRadie(radie);
	
	m_textInput->Update("");
}

void MainMenu::Draw()
{
	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);
	camera.setPosition(0, 0, -10);
	
	playButton->Draw();
	quitButton->Draw();
	c->Draw();
	m_textInput->Draw();
	p_renderingManager->Flush(camera);
}
