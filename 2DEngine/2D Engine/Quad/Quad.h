#pragma once
#include "Components/Transform2D.h"
#include <d3d11.h>
#include "EngineSource/3D Engine/Model/Texture.h"
#include "Components/Button.h"
#include "../DirectXTK/SpriteFont.h"

class Quad : public Transform2D , public Button
{
public:
	struct QUAD_VERTEX
	{
		DirectX::XMFLOAT2 position;
		DirectX::XMFLOAT2 UV;
	};


private:
	QUAD_VERTEX * quadVertex = new QUAD_VERTEX[4];

	ID3D11Buffer * m_vertexBuffer;
	Texture * m_texture;

	DirectX::SpriteFont * m_spriteFont;
	std::string m_string;

	void p_createBuffer();
	void p_setStaticQuadVertex();
public:
	Quad();
	~Quad();

	void init(DirectX::XMFLOAT2A position = DirectX::XMFLOAT2A(0,0), DirectX::XMFLOAT2A size = DirectX::XMFLOAT2A(1,1));
	void Draw();
	
	void Release();

	void setTexture(Texture * texture);
	void MapTexture();
	
	void setPosition(const float & x, const float & y) override;
	void setPosition(const DirectX::XMFLOAT2A & position) override;

	void setScale(const float & x, const float & y) override;
	void setScale(const DirectX::XMFLOAT2A & size) override;

	void setFont(DirectX::SpriteFont * font);
	void setString(const std::string & string);

	ID3D11Buffer * getVertexBuffer() const;
};

