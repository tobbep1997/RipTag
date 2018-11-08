#include "Engine2DPCH.h"
#include "Quad.h"





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
	quadVertex[0].UV = DirectX::XMFLOAT2A(0, 1);

	quadVertex[1].position = DirectX::XMFLOAT2A(-1, 1);
	quadVertex[1].UV = DirectX::XMFLOAT2A(0, 0);

	quadVertex[2].position = DirectX::XMFLOAT2A(1, -1);
	quadVertex[2].UV = DirectX::XMFLOAT2A(1, 1);

	quadVertex[3].position = DirectX::XMFLOAT2A(1, 1);
	quadVertex[3].UV = DirectX::XMFLOAT2A(1, 0);

}

void Quad::_rebuildQuad()
{
	switch (m_pivotPoint)
	{
	case Quad::center:
		quadVertex[0].position.x = ((Transform2D::getPosition().x * 2) - 1) - (this->getSize().x / 2.0f);
		quadVertex[0].position.y = ((Transform2D::getPosition().y * 2) - 1) - (this->getSize().y / 2.0f);

		quadVertex[1].position.x = quadVertex[0].position.x;
		quadVertex[1].position.y = quadVertex[0].position.y + this->getSize().y;

		quadVertex[2].position.x = quadVertex[0].position.x + this->getSize().x;
		quadVertex[2].position.y = quadVertex[0].position.y;

		quadVertex[3].position.x = quadVertex[0].position.x + this->getSize().x;
		quadVertex[3].position.y = quadVertex[0].position.y + this->getSize().y;
		break;
	case Quad::lowerLeft:
		quadVertex[0].position.x = ((Transform2D::getPosition().x * 2) - 1);
		quadVertex[0].position.y = ((Transform2D::getPosition().y * 2) - 1);

		quadVertex[1].position.x = quadVertex[0].position.x;
		quadVertex[1].position.y = quadVertex[0].position.y + this->getSize().y;

		quadVertex[2].position.x = quadVertex[0].position.x + this->getSize().x;
		quadVertex[2].position.y = quadVertex[0].position.y;

		quadVertex[3].position.x = quadVertex[0].position.x + this->getSize().x;
		quadVertex[3].position.y = quadVertex[0].position.y + this->getSize().y;
		break;
	case Quad::lowerRight:
		quadVertex[0].position.x = ((Transform2D::getPosition().x * 2) - 1) - (this->getSize().x / 2.0f);
		quadVertex[0].position.y = ((Transform2D::getPosition().y * 2) - 1) - (this->getSize().y / 2.0f);

		quadVertex[1].position.x = quadVertex[0].position.x;
		quadVertex[1].position.y = quadVertex[0].position.y + this->getSize().y;

		quadVertex[2].position.x = quadVertex[0].position.x + this->getSize().x;
		quadVertex[2].position.y = quadVertex[0].position.y;

		quadVertex[3].position.x = quadVertex[0].position.x + this->getSize().x;
		quadVertex[3].position.y = quadVertex[0].position.y + this->getSize().y;
		break;
	case Quad::upperLeft:
		quadVertex[0].position.x = ((Transform2D::getPosition().x * 2) - 1) - (this->getSize().x / 2.0f);
		quadVertex[0].position.y = ((Transform2D::getPosition().y * 2) - 1) - (this->getSize().y / 2.0f);

		quadVertex[1].position.x = quadVertex[0].position.x;
		quadVertex[1].position.y = quadVertex[0].position.y + this->getSize().y;

		quadVertex[2].position.x = quadVertex[0].position.x + this->getSize().x;
		quadVertex[2].position.y = quadVertex[0].position.y;

		quadVertex[3].position.x = quadVertex[0].position.x + this->getSize().x;
		quadVertex[3].position.y = quadVertex[0].position.y + this->getSize().y;
		break;
	case Quad::upperRight:
		quadVertex[0].position.x = ((Transform2D::getPosition().x * 2) - 1) - (this->getSize().x / 2.0f);
		quadVertex[0].position.y = ((Transform2D::getPosition().y * 2) - 1) - (this->getSize().y / 2.0f);

		quadVertex[1].position.x = quadVertex[0].position.x;
		quadVertex[1].position.y = quadVertex[0].position.y + this->getSize().y;

		quadVertex[2].position.x = quadVertex[0].position.x + this->getSize().x;
		quadVertex[2].position.y = quadVertex[0].position.y;

		quadVertex[3].position.x = quadVertex[0].position.x + this->getSize().x;
		quadVertex[3].position.y = quadVertex[0].position.y + this->getSize().y;
		break;
	default:
		break;
	}

	

	
}

Quad::Quad() : Transform2D(), Button(this)
{
	m_textures = new Texture*[3];
}


Quad::~Quad()
{
	delete m_spriteFont; // this is temp
	delete[] quadVertex;
	delete[] m_textures;
}

void Quad::init(DirectX::XMFLOAT2A position, DirectX::XMFLOAT2A size)
{
	p_setStaticQuadVertex();
	setPosition(position);
	setScale(size);
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
	this->m_textures[ButtonStates::Pressed] = texture;
}

void Quad::setHoverTexture(Texture * texture)
{
	this->m_textures[ButtonStates::Hover] = texture;
}

void Quad::setUnpressedTexture(Texture * texture)
{
	this->m_textures[ButtonStates::Normal] = texture;
}

void Quad::MapTexture()
{
	this->m_textures[m_buttonState]->Bind(1);
}

void Quad::setPosition(const float & x, const float & y)
{
	this->setPosition(DirectX::XMFLOAT2A(x, y));
}

void Quad::setPosition(const DirectX::XMFLOAT2A & position)
{
	Transform2D::setPosition(position);
	_rebuildQuad();
	p_createBuffer();
}

void Quad::setScale(const float & x, const float & y)
{
	this->setScale(DirectX::XMFLOAT2A(x, y));
}

void Quad::setScale(const DirectX::XMFLOAT2A & size)
{
	Transform2D::setScale(size);
	_rebuildQuad();
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
	m_preState = m_currentState;
	if (Button::Inside(mousepos))
	{
		if (InputHandler::isMLeftPressed(true))
		{
			m_buttonState = ButtonStates::Pressed;
		}
		else
			m_buttonState = ButtonStates::Hover;
	}
	else
		m_buttonState = ButtonStates::Normal;

	

	if (m_buttonState == ButtonStates::Pressed)
		m_currentState = true;
	else
		m_currentState = false;
	return m_currentState;
}

const bool Quad::isReleased(const DirectX::XMFLOAT2 & mousePos)
{
	bool b = !this->isPressed(mousePos) && m_preState && this->Inside(mousePos);	
	if (m_selected)
		m_buttonState = ButtonStates::Hover;
	return b;
}

//0 == Normal
//1 == Hover
//2 == Pressed
void Quad::setState(const unsigned int & bs)
{
	this->m_buttonState = (ButtonStates)bs;
}

void Quad::Select(const bool & b)
{
	m_selected = b;
}

const bool & Quad::isSelected() const
{
	return m_selected;
}

void Quad::setPivotPoint(PivotPoint pivotPoint)
{
	this->m_pivotPoint = pivotPoint;
	this->setPosition(this->getPosition());
}

Quad * Quad::CreateButton(std::string string, float px, float py, float sx, float sy)
{
	Quad * ptr = nullptr;
	ptr = new Quad();
	ptr->init({ px, py }, { sx, sy });
	ptr->setString(string);
	return ptr;
}
