#include "Engine2DPCH.h"
#include "FontHandler.h"

FontHandler::FontHandler()
{
}


FontHandler::~FontHandler()
{
}

void FontHandler::loadFont()
{
	this->spriteFont = new DirectX::SpriteFont(DX::g_device, L"../2DEngine/Fonts/consolas32.spritefont");
}

DirectX::SpriteFont * FontHandler::getFont()
{
	return nullptr;
}
