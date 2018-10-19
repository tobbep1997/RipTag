#include "Quad.h"
#include "EngineSource/3D Engine/Extern.h"
#include "InputManager/InputHandler.h"

void Quad::p_createBuffer()
{
	DX::SafeRelease(m_vertexBuffer);
	
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(QUAD_VERTEX) * 4;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = quadVertex;
	HRESULT hr = DX::g_device->CreateBuffer(&bufferDesc, &vertexData, &m_vertexBuffer);
}

void Quad::p_setStaticQuadVertex()
{
	quadVertex[0].position = DirectX::XMFLOAT2A(-1, -1);
	quadVertex[0].UV = DirectX::XMFLOAT2A(0, 0);

	quadVertex[1].position = DirectX::XMFLOAT2A(-1, 1);
	quadVertex[1].UV = DirectX::XMFLOAT2A(0, 1);

	quadVertex[2].position = DirectX::XMFLOAT2A(1, -1);
	quadVertex[2].UV = DirectX::XMFLOAT2A(1, 0);

	quadVertex[3].position = DirectX::XMFLOAT2A(1, 1);
	quadVertex[3].UV = DirectX::XMFLOAT2A(1, 1);

}

Quad::Quad() : Transform2D(), Button(this)
{
}


Quad::~Quad()
{
	delete m_spriteFont;
	delete[] quadVertex;
}

void Quad::init(DirectX::XMFLOAT2A position, DirectX::XMFLOAT2A size)
{
	Transform2D::setPosition(position);
	Transform2D::setScale(size);
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

void Quad::setPressedTexture(Texture * texture)
{
	this->m_pressedTexture = texture;
}

void Quad::setUnpressedTexture(Texture * texture)
{
	this->m_unpressedTexture = texture;
}

void Quad::MapTexture()
{
	if (this->m_isPressed)
		this->m_pressedTexture->Bind(1);
	else
		this->m_unpressedTexture->Bind(1);
}

void Quad::setPosition(const float & x, const float & y)
{
	this->setPosition(DirectX::XMFLOAT2A(x, y));
}

void Quad::setPosition(const DirectX::XMFLOAT2A & position)
{
	Transform2D::setPosition(position);
	quadVertex[0].position.x = ((position.x * 2) - 1) - (this->getSize().x / 2.0f);
	quadVertex[0].position.y = ((position.y * 2) - 1) - (this->getSize().y / 2.0f);

	quadVertex[1].position.x = quadVertex[0].position.x;
	quadVertex[1].position.y = quadVertex[0].position.y + this->getSize().y;

	quadVertex[2].position.x = quadVertex[0].position.x + this->getSize().x;
	quadVertex[2].position.y = quadVertex[0].position.y;

	quadVertex[3].position.x = quadVertex[0].position.x + this->getSize().x;
	quadVertex[3].position.y = quadVertex[0].position.y + this->getSize().y;

	p_createBuffer();
}

void Quad::setScale(const float & x, const float & y)
{
	this->setScale(DirectX::XMFLOAT2A(x, y));
}

void Quad::setScale(const DirectX::XMFLOAT2A & size)
{
	Transform2D::setScale(size);

	quadVertex[0].position.x = ((Transform2D::getPosition().x * 2) - 1) - (this->getSize().x / 2.0f);
	quadVertex[0].position.y = ((Transform2D::getPosition().y * 2) - 1) - (this->getSize().y / 2.0f);

	quadVertex[1].position.x = quadVertex[0].position.x;
	quadVertex[1].position.y = quadVertex[0].position.y + this->getSize().y;

	quadVertex[2].position.x = quadVertex[0].position.x + this->getSize().x;
	quadVertex[2].position.y = quadVertex[0].position.y;

	quadVertex[3].position.x = quadVertex[0].position.x + this->getSize().x;
	quadVertex[3].position.y = quadVertex[0].position.y + this->getSize().y;

	p_createBuffer();
}

void Quad::setFont(DirectX::SpriteFont * font)
{
	this->m_spriteFont = font;
}

void Quad::setString(const std::string & string)
{
	this->m_string = string;
}

DirectX::SpriteFont & Quad::getSpriteFont() const
{
	return *m_spriteFont;
}

const std::string & Quad::getString() const
{
	return m_string;
}

void Quad::setTextColor(const DirectX::XMFLOAT4A & color)
{
	this->m_textColor = color;
}

const DirectX::XMFLOAT4A & Quad::getTextColor() const
{
	return m_textColor;
}

ID3D11Buffer * Quad::getVertexBuffer() const
{
	return m_vertexBuffer;
}

const bool Quad::isPressed(const DirectX::XMFLOAT2 & mousepos)
{
	if (InputHandler::isMLeftPressed(true))
		this->m_isPressed = Button::Inside(mousepos);
	else
		this->m_isPressed = false;
	return this->m_isPressed;
}