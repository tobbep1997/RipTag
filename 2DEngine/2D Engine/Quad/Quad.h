#pragma once
#include <d3d11.h>
#include <string>
#include "Components/Button.h"
#include "Components/Transform2D.h"

namespace DirectX
{
	class SpriteFont;
}


enum ButtonStates
{
	Normal,
	Hover,
	Pressed
};

class Texture;
class Quad : public Transform2D , public Button
{
public:
	struct QUAD_VERTEX
	{
		DirectX::XMFLOAT2 position;
		DirectX::XMFLOAT2 UV;
	};

	enum PivotPoint
	{
		center,
		lowerLeft,
		lowerRight,
		upperLeft,
		upperRight
	};

private:
	QUAD_VERTEX * quadVertex = new QUAD_VERTEX[4];

	ID3D11Buffer * m_vertexBuffer;


	ButtonStates m_buttonState = ButtonStates::Normal;
	PivotPoint m_pivotPoint = PivotPoint::center;
	Texture ** m_textures;

	DirectX::SpriteFont * m_spriteFont;
	std::string m_string = "";

	DirectX::XMFLOAT4A m_textColor;

	void p_createBuffer();
	void p_setStaticQuadVertex();

	bool m_preState = false;
	bool m_currentState = false;

	bool m_selected = false;

	void _rebuildQuad();

public:
	Quad();
	~Quad();

	void init(DirectX::XMFLOAT2A position = DirectX::XMFLOAT2A(0,0), DirectX::XMFLOAT2A size = DirectX::XMFLOAT2A(1,1));
	void Draw();
	
	void Release();

	void setPressedTexture(Texture * texture);
	void setHoverTexture(Texture * texture);
	void setUnpressedTexture(Texture * texture);
	void MapTexture();
	
	void setPosition(const float & x, const float & y) override;
	void setPosition(const DirectX::XMFLOAT2A & position) override;

	void setScale(const float & x, const float & y) override;
	void setScale(const DirectX::XMFLOAT2A & size) override;

	void setFont(DirectX::SpriteFont * font);
	void setString(const std::string & string);

	DirectX::SpriteFont & getSpriteFont() const;
	const std::string & getString() const;
	unsigned int getState(){ return (unsigned int)this->m_buttonState; }

	void setTextColor(const DirectX::XMFLOAT4A & color);
	const DirectX::XMFLOAT4A & getTextColor() const;

	ID3D11Buffer * getVertexBuffer() const;

	const bool isPressed(const DirectX::XMFLOAT2 & mousePos);
	const bool isReleased(const DirectX::XMFLOAT2 & mousePos);
	
	void setState(const unsigned int & buttonState);

	void Select(const bool & b);
	const bool & isSelected() const;

	void setPivotPoint(PivotPoint pivotPoint);
	
	//Quick function to create a button, returns a Quad pointer.
	//The button has not set any textures, text color and font yet
	static Quad* CreateButton(std::string string, float px, float py, float sx, float sy);
};

