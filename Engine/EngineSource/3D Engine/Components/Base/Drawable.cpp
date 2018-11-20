#include "EnginePCH.h"
#include "Drawable.h"

void Drawable::_setStaticBuffer()
{
	DX::SafeRelease(p_vertexBuffer);

	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(StaticVertex) * (UINT)m_staticMesh->getVertice().size();


	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_staticMesh->getRawVertice();
	HRESULT hr = DX::g_device->CreateBuffer(&bufferDesc, &vertexData, &p_vertexBuffer);
}

void Drawable::_setDynamicBuffer()
{
	DX::SafeRelease(p_vertexBuffer);

	UINT32 vertexSize = sizeof(DynamicVertex);
	UINT32 offset = 0;
	
	struct stuff
	{
		float pX, pY, pZ, pW;
		float nX, nY, nZ, nW;
		float tX, tY, tZ, tW;
		float u, v;
		uint32_t i1, i2, i3, i4;
		float w1, w2, w3, w4;
		float g1, g2;
	};

	auto vec = m_skinnedMesh->getVertices();
	std::vector<stuff> stuffs;
	for (auto& v : vec)
	{
		stuff s =
		{
			v.pos.x, v.pos.y, v.pos.z, v.pos.w,
			v.normal.x, v.normal.y, v.normal.z, v.normal.w,
			v.tangent.x, v.tangent.y, v.tangent.z, v.tangent.w,
			v.uvPos.x, v.uvPos.y,
			v.influencingJoint.x, v.influencingJoint.y, v.influencingJoint.z, v.influencingJoint.w,
			v.jointWeights.x, v.jointWeights.y, v.jointWeights.z, v.jointWeights.w
		};
		stuffs.push_back(s);
	}


	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(stuff) * (UINT)stuffs.size();


	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = stuffs.data();
	HRESULT hr = DX::g_device->CreateBuffer(&bufferDesc, &vertexData, &p_vertexBuffer);
}

void Drawable::setOutline(bool outline)
{
	m_outline = outline;
}

bool Drawable::getOutline()
{
	return m_outline;
}

void Drawable::setOutlineColor(const DirectX::XMFLOAT4A& color)
{
	m_outLineColor = color;
}

DirectX::XMFLOAT4A Drawable::getOutlineColor()
{
	return m_outLineColor;
}

void Drawable::SetTransparant(const bool& bo)
{
	m_transparant = bo;
}

bool Drawable::GetTransparant()
{
	return m_transparant;
}

std::string Drawable::getTextureName() const
{
	return std::string(this->p_texture->getName().begin(), this->p_texture->getName().end());
}


void Drawable::p_createBuffer()
{
	switch (p_objectType)
	{
	case Static:
		_setStaticBuffer();
		break;
	case Dynamic:
		_setDynamicBuffer();
		break;
	}
}

void Drawable::p_setMesh(StaticMesh * staticMesh)
{
	this->m_staticMesh = staticMesh;
}

void Drawable::p_setMesh(SkinnedMesh * skinnedMesh)
{
	this->m_skinnedMesh = skinnedMesh;
}

void Drawable::p_createBoundingBox(const DirectX::XMFLOAT3 & center, const DirectX::XMFLOAT3 & extens)
{
	if (m_bb)
		delete m_bb;
	m_bb = nullptr;
	this->m_bb = new DirectX::BoundingBox(center, extens);
	this->m_bb->Transform(*m_bb, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&getWorldmatrix())));
}

void Drawable::setTexture(Texture * texture)
{
	this->p_texture = texture;
}



void Drawable::BindTextures()
{ 
	if (p_texture)
		p_texture->Bind(1);
}

Drawable::Drawable() : Transform()
{	
	m_staticMesh = nullptr;
	m_skinnedMesh = nullptr;
	p_vertexBuffer = nullptr;
	p_color = DirectX::XMFLOAT4A(1, 1, 1, 1);
	m_hidden = false;
	m_outLineColor = DirectX::XMFLOAT4A(1, 1, 1, 1);
	m_outline = false;
	m_transparant = false;
}


Drawable::~Drawable()
{
	DX::SafeRelease(p_vertexBuffer);
	if (m_anim)
		delete m_anim;

	if (m_bb)
		delete m_bb;
}



void Drawable::Draw()
{
		switch (p_objectType)
		{
		case Static:
			if(m_staticMesh)
				DX::INSTANCING::submitToInstance(this);
			break;
		case Dynamic:
			if (m_skinnedMesh)
				DX::g_animatedGeometryQueue.push_back(this);
			break;
		}	
}

void Drawable::DrawWireFrame()
{
	if (p_objectType == Static)
	{
		//DX::g_wireFrameDrawQueue.push_back(this);
		DX::INSTANCING::submitToWireframeInstance(this);
	}
}

void Drawable::setVertexShader(const std::wstring & path)
{
	this->p_vertexPath = path;
}

void Drawable::setPixelShader(const std::wstring & path)
{
	this->p_pixelPath = path;
}



std::wstring Drawable::getVertexPath() const
{
	return this->p_vertexPath;
}

std::wstring Drawable::getPixelPath() const
{
	return this->p_pixelPath;
}

UINT Drawable::getVertexSize()
{
	switch (p_objectType)
	{
	case Static:
		return (UINT)m_staticMesh->getVertice().size();
		break;
	case Dynamic:
		return (UINT)m_skinnedMesh->getVertices().size();
		break;
	default:
		return 0;
		break;
	}
	return 0;
	
}

ID3D11Buffer * Drawable::getBuffer()
{
	if (m_skinnedMesh)
		return p_vertexBuffer;
	else
		return m_staticMesh->getBuffer();
}

ObjectType Drawable::getObjectType()
{
	return p_objectType;
}

EntityType Drawable::getEntityType()
{
	return this->p_entityType;
}

void Drawable::setEntityType(EntityType en)
{
	this->p_entityType = en;
}

Animation::AnimationPlayer* Drawable::getAnimationPlayer()
{
	return m_anim;
}

StaticMesh* Drawable::getStaticMesh()
{
	return this->m_staticMesh;
}

void Drawable::setTextureTileMult(float u, float v)
{
	this->m_textureTileMult.x = u;
	this->m_textureTileMult.y = v;
}

const DirectX::XMFLOAT2A & Drawable::getTextureTileMult() const
{
	return this->m_textureTileMult;
}

bool Drawable::isTextureAssigned()
{
	return p_texture;
}

void Drawable::setModel(StaticMesh * staticMesh)
{
	this->p_objectType = Static;
	setVertexShader(L"../Engine/EngineSource/Shader/VertexShader.hlsl");
	setPixelShader(L"../Engine/EngineSource/Shader/PixelShader.hlsl");
	Drawable::p_setMesh(staticMesh);
}

void Drawable::setModel(SkinnedMesh * skinnedMesh)
{
	this->p_objectType = Dynamic;
	setVertexShader(L"../Engine/EngineSource/Shader/AnimatedVertexShader.hlsl");
	setPixelShader(L"../Engine/EngineSource/Shader/PixelShader.hlsl");
	Drawable::p_setMesh(skinnedMesh);
	p_createBuffer();
	m_anim = new Animation::AnimationPlayer(this);
}

void Drawable::setColor(const DirectX::XMFLOAT4A& color)
{
	this->p_color = color;
}

void Drawable::setColor(const float& x, const float& y, const float& z, const float& w)
{
	setColor(DirectX::XMFLOAT4A(x, y, z, w));
}

const DirectX::XMFLOAT4A& Drawable::getColor() const
{
	return this->p_color;
}

void Drawable::setHidden(bool hidden)
{
	m_hidden = hidden;
}

bool Drawable::getHidden()
{
	return m_hidden;
}

DirectX::BoundingBox * Drawable::getBoundingBox()
{
	return this->m_bb;
}


