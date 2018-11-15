#include "RipTagPCH.h"
#include "LoadingScreen.h"

LoadingScreen::LoadingScreen()
{
	
}

void LoadingScreen::Init()
{
	if (DX::g_device != nullptr)
		FontHandler::loadFont("consolas32");


	if (m_loadingText)
	{
		m_loadingText->Release();
		delete m_loadingText;
	}
	m_loadingText = new Quad();
	m_loadingText->init(DirectX::XMFLOAT2A(0.5f, 0.5f), DirectX::XMFLOAT2A(2.0f, 2.0f));
	m_loadingText->setPivotPoint(Quad::center);
	m_loadingText->setIsButton(false);
	m_loadingText->setString("Loading...");

	m_loadingText->setUnpressedTexture("gui_loading_screen");
	m_loadingText->setFont(FontHandler::getFont("consolas32"));
	m_loadingText->setTextColor(DirectX::XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f));
}

LoadingScreen::~LoadingScreen()
{
	if (m_loadingText != nullptr)
	{
		m_loadingText->Release(); 
		delete m_loadingText; 
	}
}

void LoadingScreen::removeGUI(std::vector<Quad*>& GUICollection)
{ 
	for (int i = 0; i < GUICollection.size(); i++)
	{
		GUICollection.at(i)->Release(); 
		delete GUICollection.at(i); 
	}
	GUICollection.clear(); 
	DX::g_2DQueue.clear(); 
}

void LoadingScreen::draw()
{
	DX::g_2DQueue.push_back(m_loadingText); 
}

void LoadingScreen::update()
{
	m_alphaValue += 2 * sin(m_frequency); 
	m_loadingText->setTextColor(DirectX::XMFLOAT4A(1.0f, 1.0f, 1.0f, m_alphaValue)); 
}
