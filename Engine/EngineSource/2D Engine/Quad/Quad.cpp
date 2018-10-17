#include "Quad.h"
#include "../../3D Engine/Extern.h"


void Quad::p_createBuffer()
{
	DX::SafeRelease(m_vertexBuffer);

	UINT32 vertexSize = sizeof(QUAD_VERTEX);
	UINT32 offset = 0;

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(QUAD_VERTEX) * (UINT)4;


	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = quadVertex;
	HRESULT hr = DX::g_device->CreateBuffer(&bufferDesc, &vertexData, &m_vertexBuffer);
}

void Quad::p_setStaticQuadVertex()
{
	quadVertex[0].position = DirectX::XMFLOAT2A(0, 0);
	quadVertex[0].UV = DirectX::XMFLOAT2A(0, 0);

	quadVertex[1].position = DirectX::XMFLOAT2A(0, 1);
	quadVertex[1].UV = DirectX::XMFLOAT2A(0, 1);

	quadVertex[2].position = DirectX::XMFLOAT2A(1, 1);
	quadVertex[2].UV = DirectX::XMFLOAT2A(1, 1);

	quadVertex[3].position = DirectX::XMFLOAT2A(1, 0);
	quadVertex[3].UV = DirectX::XMFLOAT2A(1, 0);

}

Quad::Quad()
{
}


Quad::~Quad()
{
	delete[] quadVertex;
}

void Quad::init(DirectX::XMFLOAT2A position, DirectX::XMFLOAT2A size)
{
	this->setPosition(position);
	this->setScale(size);
	p_setStaticQuadVertex();
	p_createBuffer();
}

void Quad::Draw()
{
	DX::g_2DQueue.push_back(this);
}

void Quad::Release()
{
	DX::SafeRelease(m_vertexBuffer);
}

void Quad::setTexture(Texture * texture)
{
	this->m_texture = texture;
}

void Quad::MapTexture()
{
	this->m_texture->Bind(1);
}

ID3D11Buffer * Quad::getVertexBuffer() const
{
	return m_vertexBuffer;
}
