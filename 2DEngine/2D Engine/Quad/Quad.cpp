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
	HRESULT hr;
	if (SUCCEEDED(hr = DX::g_device->CreateBuffer(&bufferDesc, &vertexData, &m_vertexBuffer)))
	{
		DX::SetName(m_vertexBuffer, "QUAD: m_vertexBuffer");
	}
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

const float& Quad::getAngle() const
{
	return -1;
}

const bool Quad::Inside(const DirectX::XMFLOAT2 & mousePos)
{
	static long long counter = 0;
	DirectX::XMFLOAT2 mp = mousePos;
	DirectX::XMFLOAT2 pos;

	pos.x = 0.5f * quadVertex[Upper_Left].position.x + 0.5f;
	pos.y = -0.5f * quadVertex[Upper_Left].position.y + 0.5f;
	DirectX::XMFLOAT2A size = { getSize().x * 0.5f, getSize().y * 0.5f };

	return mp.x > pos.x && mp.x < pos.x + size.x &&
		mp.y > pos.y && mp.y < pos.y + size.y;
}

void Quad::_rebuildQuad()
{
	static int counter = 0;
	switch (m_pivotPoint)
	{
	case Quad::center:
		quadVertex[Lower_Left].position.x = ((Transform2D::getPosition().x * 2) - 1) - (this->getSize().x / 2.0f);
		quadVertex[Lower_Left].position.y = ((Transform2D::getPosition().y * 2) - 1) - (this->getSize().y / 2.0f);
		break;
	case Quad::lowerLeft:
		quadVertex[Lower_Left].position.x = ((Transform2D::getPosition().x * 2) - 1);
		quadVertex[Lower_Left].position.y = ((Transform2D::getPosition().y * 2) - 1);
		break;
	case Quad::lowerRight:
		quadVertex[Lower_Left].position.x = ((Transform2D::getPosition().x * 2) - 1) - (this->getSize().x);
		quadVertex[Lower_Left].position.y = ((Transform2D::getPosition().y * 2) - 1);
		break;
	case Quad::upperLeft:
		quadVertex[Lower_Left].position.x = ((Transform2D::getPosition().x * 2) - 1);
		quadVertex[Lower_Left].position.y = ((Transform2D::getPosition().y * 2) - 1) - this->getSize().y;
		break;
	case Quad::upperRight:
		quadVertex[Lower_Left].position.x = ((Transform2D::getPosition().x * 2) - 1) - (this->getSize().x);
		quadVertex[Lower_Left].position.y = ((Transform2D::getPosition().y * 2) - 1) - (this->getSize().y);
		break;
	case Quad::upperCenter:
		quadVertex[Lower_Left].position.x = ((Transform2D::getPosition().x * 2) - 1) - (this->getSize().x * 0.5f);
		quadVertex[Lower_Left].position.y = ((Transform2D::getPosition().y * 2) - 1) - (this->getSize().y);
		break;
	case Quad::lowerCenter:
		quadVertex[Lower_Left].position.x = ((Transform2D::getPosition().x * 2) - 1) - (this->getSize().x * 0.5f);
		quadVertex[Lower_Left].position.y = ((Transform2D::getPosition().y * 2) - 1);
		break;
	case Quad::centerLeft:
		quadVertex[Lower_Left].position.x = ((Transform2D::getPosition().x * 2) - 1);
		quadVertex[Lower_Left].position.y = ((Transform2D::getPosition().y * 2) - 1) - (this->getSize().y) * 0.5f;
		break;
	case Quad::centerRight:
		quadVertex[Lower_Left].position.x = ((Transform2D::getPosition().x * 2) - 1) - (this->getSize().x);
		quadVertex[Lower_Left].position.y = ((Transform2D::getPosition().y * 2) - 1) - (this->getSize().y) * 0.5f;
		break;
	}
	quadVertex[Upper_Left].position.x = quadVertex[0].position.x;
	quadVertex[Upper_Left].position.y = quadVertex[0].position.y + this->getSize().y;

	quadVertex[Lower_Right].position.x = quadVertex[0].position.x + this->getSize().x;
	quadVertex[Lower_Right].position.y = quadVertex[0].position.y;

	quadVertex[Upper_Right].position.x = quadVertex[0].position.x + this->getSize().x;
	quadVertex[Upper_Right].position.y = quadVertex[0].position.y + this->getSize().y;
	
}

Quad::Quad() : Transform2D(), Button(this)
{
}


Quad::~Quad()
{
	delete[] quadVertex;
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

void Quad::setColor(const DirectX::XMFLOAT4A & color)
{
	m_color = color;
}

void Quad::setColor(float r, float g, float b, float a)
{
	m_color = { r, g, b, a };
}

void Quad::setOutlineColor(const DirectX::XMFLOAT4A & color)
{
	m_outlineColor = color;
}

void Quad::setOutlineColor(float r, float g, float b, float a)
{
	m_outlineColor = { r, g, b, a };
}

const DirectX::XMFLOAT4A & Quad::getColor() const
{
	return m_color;
}

const DirectX::XMFLOAT4A & Quad::getOutlineColor() const
{
	return m_outlineColor;
}

void Quad::setPressedTexture(const std::string &  texture)
{
	this->m_textures[ButtonStates::Pressed] = texture;
}

void Quad::setHoverTexture(const std::string & texture)
{
	this->m_textures[ButtonStates::Hover] = texture;
}

void Quad::setUnpressedTexture(const std::string & texture)
{
	this->m_textures[ButtonStates::Normal] = texture;
}

void Quad::MapTexture()
{
	std::string name = this->m_textures[m_buttonState];
	std::wstring wName = std::wstring(name.begin(), name.end());

	if (Manager::g_textureManager.getTexture(name))
	{
		Manager::g_textureManager.getTexture(name)->Bind(1);
	}
	else if (Manager::g_textureManager.getGUITextureByName(wName))
	{
		Manager::g_textureManager.getGUITextureByName(wName)->Bind(1);
	}
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

void Quad::setTag(std::string tag)
{
	m_tag = tag; 
}

void Quad::setFont(DirectX::SpriteFont * font)
{
	this->m_spriteFont = font;
}

void Quad::setString(const std::string & string)
{
	this->m_string = string;
}

void Quad::setIsButton(bool isButton)
{
	m_isButton = isButton; 
}

DirectX::SpriteFont & Quad::getSpriteFont() const
{
	return *m_spriteFont;
}

const std::string & Quad::getString() const
{
	return m_string;
}

void Quad::getString(std::string & string)
{
	string = this->m_string;
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
	if (Inside(mousepos))
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

void Quad::setTextAlignment(Quad::TextAlignment alignment)
{
	this->m_textAlignment = alignment;
}

Quad::TextAlignment Quad::getTextAlignment() const
{
	return m_textAlignment;
}

DirectX::XMFLOAT4 Quad::getCenter() const
{
	return DirectX::XMFLOAT4();
}

void Quad::setType(QuadType qt)
{
	m_qt = qt;
}

unsigned int Quad::getType() const
{
	return (unsigned)m_qt;
}

bool Quad::setRadie(const float & radie)
{
	m_rad = radie;
	return true;
}

const float & Quad::getRadie() const
{
	return m_rad;
}

const float & Quad::getInnerRadie() const
{
	return 0.0f;
}

const std::string Quad::getTag() const
{
	return m_tag; 
}

void Quad::setAngle(const float& angle)
{
	
}

const bool Quad::getIsButton() const
{
	return m_isButton; 
}

DirectX::XMFLOAT4 Quad::getReferencePosAndSize() const
{
	DirectX::XMFLOAT4 posAndSize;
	posAndSize.x = quadVertex[Lower_Left].position.x;
	posAndSize.y = quadVertex[Lower_Left].position.y;
	posAndSize.z = getSize().x;
	posAndSize.w = getSize().y;
	return posAndSize;
}

float Quad::getU() const
{
	return this->uScale;
}

float Quad::getV() const
{
	return this->vScale;
}

void Quad::setU(const float & u)
{
	this->uScale = u;
}

void Quad::setV(const float & v)
{
	this->vScale = v;
}