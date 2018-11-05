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

	auto vec = m_dynamicMesh->getVertices();
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

SM::AnimationStateMachine* Drawable::InitStateMachine()
{
	m_stateMachine = new SM::AnimationStateMachine(1);
	return m_stateMachine;
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

void Drawable::p_setMesh(DynamicMesh * dynamicMesh)
{
	this->m_dynamicMesh = dynamicMesh;
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
	m_dynamicMesh = nullptr;
	p_vertexBuffer = nullptr;
	p_color = DirectX::XMFLOAT4A(1, 1, 1, 1);
	
}


Drawable::~Drawable()
{
	DX::SafeRelease(p_vertexBuffer);
	if (m_anim)
		delete m_anim;
	if (m_stateMachine)
		delete m_stateMachine;
}



void Drawable::Draw()
{
		switch (p_objectType)
		{
		case Static:
			if(m_staticMesh)
				DX::g_geometryQueue.push_back(this);
			break;
		case Dynamic:
			if (m_dynamicMesh)
				DX::g_animatedGeometryQueue.push_back(this);
			break;
		}	
}

void Drawable::DrawWireFrame()
{
	if (p_objectType == Static)
	{
		DX::g_wireFrameDrawQueue.push_back(this);
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
		return (UINT)m_dynamicMesh->getVertices().size();
		break;
	default:
		return 0;
		break;
	}
	return 0;
	
}

ID3D11Buffer * Drawable::getBuffer()
{
	return p_vertexBuffer;
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

Animation::AnimatedModel* Drawable::getAnimatedModel()
{
	return m_anim;
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
	p_createBuffer();
}

void Drawable::setModel(DynamicMesh * dynamicMesh)
{
	this->p_objectType = Dynamic;
	setVertexShader(L"../Engine/EngineSource/Shader/AnimatedVertexShader.hlsl");
	setPixelShader(L"../Engine/EngineSource/Shader/PixelShader.hlsl");
	Drawable::p_setMesh(dynamicMesh);
	p_createBuffer();
	m_anim = new Animation::AnimatedModel();
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




