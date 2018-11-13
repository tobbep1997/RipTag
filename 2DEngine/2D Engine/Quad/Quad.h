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
		upperRight,
		upperCenter,
		lowerCenter,
		centerLeft,
		centerRight
	};

	enum TextAlignment
	{
		centerAligned,
		leftAligned,
		rightAligned
	};

private:
	enum Vertex_Positions
	{
		Lower_Left = 0,
		Upper_Left,
		Lower_Right,
		Upper_Right
	};

	ID3D11Buffer * m_vertexBuffer;


	ButtonStates m_buttonState = ButtonStates::Normal;
	PivotPoint m_pivotPoint = PivotPoint::center;
	std::string m_textures[3];

	DirectX::SpriteFont * m_spriteFont;
	std::string m_string = "";

	DirectX::XMFLOAT4A m_textColor;

	TextAlignment m_textAlignment = TextAlignment::centerAligned;

	void p_createBuffer();
	void p_setStaticQuadVertex();

	bool m_preState = false;
	bool m_currentState = false;
	bool m_isButton = true; 

	bool m_selected = false;

	float uScale = 1.0f;
	float vScale = 1.0f;

	void _rebuildQuad();
protected:
	QUAD_VERTEX * quadVertex = new QUAD_VERTEX[4];
public:
	Quad();
	virtual~Quad();

	void init(DirectX::XMFLOAT2A position = DirectX::XMFLOAT2A(0,0), DirectX::XMFLOAT2A size = DirectX::XMFLOAT2A(1,1));
	void Draw();
	
	void Release();

	void setPressedTexture(const std::string & texture);
	void setHoverTexture(const std::string & texture);
	void setUnpressedTexture(const std::string & texture);
	void MapTexture();
	
	void setPosition(const float & x, const float & y) override;
	void setPosition(const DirectX::XMFLOAT2A & position) override;

	void setScale(const float & x, const float & y) override;
	void setScale(const DirectX::XMFLOAT2A & size) override;

	void setFont(DirectX::SpriteFont * font);
	void setString(const std::string & string);

	void setIsButton(bool isButton); 

	DirectX::SpriteFont & getSpriteFont() const;
	const std::string & getString() const;
	unsigned int getState(){ return (unsigned int)this->m_buttonState; }
	void getString(std::string & string);

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

	void setTextAlignment(TextAlignment alignment);
	TextAlignment getTextAlignment() const;

	virtual DirectX::XMFLOAT4 getCenter() const;
	virtual unsigned int getType() const;
	virtual const float & getRadie() const;
	virtual const float & getInnerRadie() const;

	virtual const bool getIsButton() const; 

	DirectX::XMFLOAT4 getReferencePosAndSize() const;


	float getU() const;
	float getV() const;
	void setU(const float & u);
	void setV(const float & v);
	
};

