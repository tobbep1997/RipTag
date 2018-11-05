#pragma once
namespace DirectX
{
	class SpriteFont;
}
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

