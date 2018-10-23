#pragma once
#include "../2D Engine/DirectXTK/SpriteFont.h"
class FontHandler
{
private:
	DirectX::SpriteFont * spriteFont;
public:
	FontHandler();
	~FontHandler();

	void loadFont();
	DirectX::SpriteFont * getFont();
	
};

