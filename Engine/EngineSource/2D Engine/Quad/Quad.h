#pragma once
#include "Components/Transform2D.h"
#include <d3d11.h>
#include "../../3D Engine/Model/Texture.h"

class Quad : public Transform2D
{
public:
	struct QUAD_VERTEX
	{
		QUAD_VERTEX()
		{
			position = DirectX::XMFLOAT2A(0, 0);
			UV = DirectX::XMFLOAT2A(0, 0);
		}
		DirectX::XMFLOAT2A position;
		DirectX::XMFLOAT2A UV;
	};


private:
	QUAD_VERTEX * quadVertex = new QUAD_VERTEX[4];

	ID3D11Buffer * m_vertexBuffer;

	Texture * m_texture;

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

	ID3D11Buffer * getVertexBuffer() const;
};

