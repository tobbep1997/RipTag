#include "EnginePCH.h"
#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(DirectX::XMFLOAT4A origin, PS::ParticleType type,  float lifeTime)
{
	switch (type)
	{
	case PS::FIRE:
		SetAsDefaultFire(origin);
		break;
	case PS::SMOKE:
		SetAsDefaultSmoke(origin);
		break;
	case PS::DEFAULT:
		SetAsDefaultNone(origin);
		break;
	default:
		SetAsDefaultNone(origin);
		break;
	}

	m_boundingBox.Center = { m_config.m_SpawnPosition.x, m_config.m_SpawnPosition.y , m_config.m_SpawnPosition.z };
	m_boundingBox.Extents = { 500,500,500 };
}

ParticleEmitter::~ParticleEmitter()
{
	Release();
}

void ParticleEmitter::Release()
{
	if (m_vertexBuffer)
		DX::SafeRelease(m_vertexBuffer);
	if (m_cBuffer)
		DX::SafeRelease(m_cBuffer);

	m_vertexBuffer = nullptr;
	m_cBuffer = nullptr;

	for (auto& particle : m_Particles)
	{
		delete particle;
	}
	m_Particles.clear();
}

void ParticleEmitter::Update(float timeDelata, Camera * camera)
{
	if (m_config.m_EmitterLife == 0)
		m_EmitterCurrentLife = 0;

	else if (m_EmitterCurrentLife > m_config.m_EmitterLife)
	{
		m_config.m_MaxParticle = 0;
		m_config.m_nrOfEmittParticles -= 1;
		if(m_config.m_nrOfEmittParticles <= 0)
			m_emitterActive = false;
	}
	else
		m_EmitterCurrentLife += timeDelata;

	if (m_emitterActive)
	{
		float emission = m_config.m_nrOfEmittParticles * timeDelata;
		float partialEmisson = emission - (long)emission;

		int nrOfParticlesToEmit = emission;

		m_partialParticle += partialEmisson;

		if (m_partialParticle > 1.0f)
		{
			m_config.m_nrOfEmittParticles += 1;
			m_partialParticle -= 1.0f;
		}

		int particleToLast = m_config.m_MinParticle - m_Particles.size();

		if (particleToLast > 0)
		{
			nrOfParticlesToEmit += particleToLast;
		}

		for (int i = 0; i < nrOfParticlesToEmit; i++)
		{
			if (m_Particles.size() >= m_config.m_MaxParticle)
				break;

			m_newParticle = new Particle();

			DirectX::XMVECTOR SpawnPosVec = DirectX::XMLoadFloat4A(&m_config.m_SpawnPosition);

			float tempX = DirectX::XMVectorGetX(SpawnPosVec) + (RandomFloat(m_config.spawnSpread) * 0.5f);
			float tempY = DirectX::XMVectorGetY(SpawnPosVec) + (RandomFloat(m_config.spawnSpread) * 0.5f);
			float tempZ = DirectX::XMVectorGetZ(SpawnPosVec) + (RandomFloat(m_config.spawnSpread) * 0.5f);

			m_newParticle->position = DirectX::XMVECTOR{ tempX, tempY, tempZ };
			m_newParticle->velocity = DirectX::XMVECTOR{ RandomFloat(m_config.spreadMinMax), RandomFloat(m_config.directionMinMax), RandomFloat(m_config.spreadMinMax)};
			//if ((m_EmitterCurrentLife + 0.5) > m_EmitterLife)
			//	m_newParticle->velocity = DirectX::XMVECTOR{ RandomFloat(DirectX::XMINT2 {-2, 3}), RandomFloat(DirectX::XMINT2 {0, 15}), RandomFloat(DirectX::XMINT2 {1, 17}) };
			m_newParticle->speed = m_config.m_Speed;
			float temp = RandomFloat(DirectX::XMINT2 {0, 20});
			m_newParticle->color = DirectX::XMFLOAT4A(temp, temp, temp, 1); //(rand() % 2, rand() % 2, rand() % 2, rand() % 2);
			m_newParticle->scale = m_config.scale;
			m_newParticle->lifeTime = RandomFloat(m_config.minMaxLife);
			m_newParticle->rotation = RandomFloat(m_config.m_RotationMinMax);
			
			m_Particles.push_back(m_newParticle);
		}

		_particleVertexCalculation(timeDelata, camera);
	}
}

void ParticleEmitter::_particleVertexCalculation(float timeDelata, Camera * camera)
{

	for (int i = 0; i < m_Particles.size(); i++)
	{
		DirectX::XMVECTOR m_up				=	DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR m_right			= DirectX::g_XMZero;
		DirectX::XMVECTOR m_toCam		= DirectX::g_XMZero;
		DirectX::XMVECTOR m_forward		= DirectX::g_XMZero;
		DirectX::XMVECTOR m_SpeedDir	= DirectX::g_XMZero;
		DirectX::XMVECTOR m_Dir				= DirectX::g_XMZero;


		DirectX::XMVECTOR cameraPos = DirectX::XMLoadFloat4A(&camera->getPosition());
		m_toCam = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(m_Particles[i]->position, cameraPos));
		m_right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(m_toCam, m_up));
		m_up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(m_toCam, m_right));
		m_forward = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(m_up, m_right));
		m_SpeedDir = DirectX::XMVectorScale(m_Particles[i]->velocity, m_Particles[i]->speed * timeDelata);
		m_Particles[i]->position = DirectX::XMVectorAdd(m_Particles[i]->position, m_SpeedDir);
		
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixIdentity();
		rotationMatrix = DirectX::XMMatrixRotationAxis(m_forward, 43);



		m_up = DirectX::XMVectorScale(m_up, m_Particles[i]->scale.x);
		m_right = DirectX::XMVectorScale(m_right, m_Particles[i]->scale.y);


		m_Particles[i]->scale.x -= m_config.scaleOverTime.x * timeDelata;
		m_Particles[i]->scale.y -= m_config.scaleOverTime.y * timeDelata;


		if (m_Particles[i]->age > m_Particles[i]->lifeTime)
		{
			delete m_Particles[i];
			m_Particles.erase(m_Particles.begin() + i);
			i -= 1;
			continue;
		}
		m_Particles[i]->age += timeDelata;


		Vertex upLeftVertex;
		Vertex upRightVertex;
		Vertex downLeftVertex;
		Vertex downRightVertex;


		DirectX::XMStoreFloat4A(&upLeftVertex.pos, DirectX::XMVectorAdd(DirectX::XMVectorSubtract(m_Particles[i]->position, m_right), m_up));
		upLeftVertex.uvPos = DirectX::XMFLOAT2A(0.0f, 0.0f);
		upLeftVertex.tangent = DirectX::XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMStoreFloat4A(&upLeftVertex.normal, m_forward);

		DirectX::XMStoreFloat4A(&upRightVertex.pos, DirectX::XMVectorAdd(DirectX::XMVectorAdd(m_Particles[i]->position, m_right), m_up));
		upRightVertex.uvPos = DirectX::XMFLOAT2A(1.0f, 0.0f);
		upRightVertex.tangent = DirectX::XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMStoreFloat4A(&upRightVertex.normal, m_forward);

		DirectX::XMStoreFloat4A(&downLeftVertex.pos, DirectX::XMVectorSubtract(DirectX::XMVectorSubtract(m_Particles[i]->position, m_right), m_up));
		downLeftVertex.uvPos = DirectX::XMFLOAT2A(0.0f, 1.0f);
		downLeftVertex.tangent = DirectX::XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMStoreFloat4A(&downLeftVertex.normal, m_forward);

		DirectX::XMStoreFloat4A(&downRightVertex.pos, DirectX::XMVectorSubtract(DirectX::XMVectorAdd(m_Particles[i]->position, m_right), m_up));
		downRightVertex.uvPos = DirectX::XMFLOAT2A(1.0f, 1.0f);
		downRightVertex.tangent = DirectX::XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMStoreFloat4A(&downRightVertex.normal, m_forward);

		upLeftVertex.tangent.x = (m_Particles[i]->lifeTime - m_Particles[i]->age) / m_Particles[i]->lifeTime;
		upRightVertex.tangent.x = (m_Particles[i]->lifeTime - m_Particles[i]->age) / m_Particles[i]->lifeTime;
		downLeftVertex.tangent.x = (m_Particles[i]->lifeTime - m_Particles[i]->age) / m_Particles[i]->lifeTime;
		downRightVertex.tangent.x = (m_Particles[i]->lifeTime - m_Particles[i]->age) / m_Particles[i]->lifeTime;
		
		downLeftVertex.pos.w = 1.0f;
		upLeftVertex.pos.w = 1.0f;
		upRightVertex.pos.w = 1.0f;
		downRightVertex.pos.w = 1.0f;

		vertex.push_back(downLeftVertex);
		vertex.push_back(upLeftVertex);
		vertex.push_back(upRightVertex);
		vertex.push_back(downRightVertex);
		vertex.push_back(downLeftVertex);
		vertex.push_back(upRightVertex);
	}
}

void ParticleEmitter::_applyTextures()
{
	Texture * texture = nullptr;

	texture = Manager::g_textureManager.getDDSTextureByName(m_config.textures[0]);
	if (texture)
		texture->Bind(1);

	texture = Manager::g_textureManager.getDDSTextureByName(m_config.textures[1]);
	if (texture)
		texture->Bind(2);

	texture = Manager::g_textureManager.getDDSTextureByName(m_config.textures[2]);
	if (texture)
		texture->Bind(3);
}

void ParticleEmitter::InitializeBuffers()
{
	if (m_vertexBuffer)
	{
		DX::SafeRelease(m_vertexBuffer);
		m_vertexBuffer = nullptr;
	}

	if (m_cBuffer)
	{
		DX::SafeRelease(m_cBuffer);
		m_cBuffer = nullptr;
	}

	//Vertex Buffer
	{
		int nrOfVertex = m_config.m_MaxParticle * 6;
		D3D11_SUBRESOURCE_DATA data;
		D3D11_BUFFER_DESC bufferDesc;

		HRESULT hr;
		Vertex * initData = new Vertex[nrOfVertex];
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(Vertex) * nrOfVertex;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		data.pSysMem = initData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		hr = DX::g_device->CreateBuffer(&bufferDesc, &data, &m_vertexBuffer);

		delete[] initData;
	}
	//Constant Buffer
	{
		D3D11_SUBRESOURCE_DATA data;
		D3D11_BUFFER_DESC bufferDesc;

		HRESULT hr;
		
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(ConstantBufferData);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		data.pSysMem = &this->m_cData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		hr = DX::g_device->CreateBuffer(&bufferDesc, &data, &m_cBuffer);
	}
}

void ParticleEmitter::SetBuffers()
{

	UINT32 m_StrideSize = 0;
	UINT32 m_Offset = 0;

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	DX::g_deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

	m_VertexData = (Vertex*)mappedSubresource.pData;
	int particleSize = vertex.size();

	memcpy(m_VertexData, (void*)&vertex[0], sizeof(Vertex) *  vertex.size());

	DX::g_deviceContext->Unmap(m_vertexBuffer, 0);
	m_StrideSize = sizeof(Vertex);
	DX::g_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &m_StrideSize, &m_Offset);
	DX::g_deviceContext->PSSetConstantBuffers(0, 1, &m_cBuffer);

	vertex.clear();
}

void ParticleEmitter::Draw()
{
	int nrOfVerts = vertex.size();

	if (nrOfVerts == 0)
		return;

	SetBuffers();
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/ParticleVertex.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/Source/Shader/ParticleVertex.hlsl"), nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/Source/Shader/ParticlePixel.hlsl"), nullptr, 0);

	_applyTextures();
	DX::g_deviceContext->Draw(nrOfVerts, 0);
}

void ParticleEmitter::Clear()
{
	vertex.clear();
}

void ParticleEmitter::SetAsDefaultNone(DirectX::XMFLOAT4A origin)
{

	m_emitterActive = 1;
	m_EmitterCurrentLife = 0;

	m_config.m_EmitterLife = 0;
	m_config.m_MaxParticle = 10;
	m_config.m_MinParticle = 1;
	m_config.m_nrOfEmittParticles = 10;
	m_config.m_Speed = 0.1f;

	m_config.m_SpawnPosition = origin;

	m_config.scaleOverTime = DirectX::XMFLOAT2{ 0.90f, 0.90f };
	m_config.scale = DirectX::XMFLOAT2(0.5f, 0.5f);

	m_config.m_RotationMinMax = DirectX::XMINT2{ 1, 360 };
	m_config.spreadMinMax = DirectX::XMINT2{ -20, 20 };
	m_config.directionMinMax = DirectX::XMINT2{ 4, 10 };
	m_config.minMaxLife = DirectX::XMINT2{ 0, 10 };
	m_config.spawnSpread = DirectX::XMINT2{ -1, 1 };

	InitializeBuffers();
}

void ParticleEmitter::SetAsDefaultFire(DirectX::XMFLOAT4A origin)
{
	m_emitterActive						= 1;
	m_EmitterCurrentLife				= 0;

	m_config.m_EmitterLife				= 0;
	m_config.m_MaxParticle				= 25;
	m_config.m_MinParticle				= 25;
	m_config.m_nrOfEmittParticles	= 25;
	m_config.m_Speed						= 0.1f;

	m_config.m_SpawnPosition			= origin;

	m_config.scaleOverTime				= DirectX::XMFLOAT2{ 0.45f, 0.45f };
	m_config.scale								= DirectX::XMFLOAT2(0.2f, 0.2f);

	m_config.m_RotationMinMax		= DirectX::XMINT2{ 1, 360 };
	m_config.spreadMinMax				= DirectX::XMINT2{ -1, 1 };
	m_config.directionMinMax			= DirectX::XMINT2{ 6, 10 };
	m_config.minMaxLife					= DirectX::XMINT2{ 0, 1 };
	m_config.spawnSpread				= DirectX::XMINT2{ 0, 0 };

	m_cData.alphaMultipliers[0] = m_config.alphaMultipliers[0] = 1.0f;
	m_cData.alphaMultipliers[1] = m_config.alphaMultipliers[1] = 1.5f;
	m_cData.alphaMultipliers[2] = m_config.alphaMultipliers[2] = 1.5f;

	m_cData.fadePoints[0] = m_config.fadingPoints[0] = 1.0f;
	m_cData.fadePoints[1] = m_config.fadingPoints[1] = 0.35f;
	m_cData.fadePoints[2] = m_config.fadingPoints[2] = 0.15f;

	m_config.textures[0] = L"FIRE";
	m_config.textures[1] = L"FIRE_CLEAR";
	m_config.textures[2] = L"SMOKE";

	InitializeBuffers();
}

void ParticleEmitter::SetAsDefaultSmoke(DirectX::XMFLOAT4A origin)
{
	m_emitterActive						= true;
	m_EmitterCurrentLife				= 0;

	m_config.m_EmitterLife				= 5;
	m_config.m_MaxParticle				= 10;
	m_config.m_MinParticle				= 10;
	m_config.m_nrOfEmittParticles	= 10;
	m_config.m_Speed						= 0.001f;

	m_config.m_SpawnPosition			= origin;

	m_config.scaleOverTime				= DirectX::XMFLOAT2{ -1.0f, -1.0f };
	m_config.scale								= DirectX::XMFLOAT2(0.01f, 0.01f);

	m_config.m_RotationMinMax		= DirectX::XMINT2{ 360, 360 };
	m_config.directionMinMax			= DirectX::XMINT2{ -4, 4 };
	m_config.minMaxLife					= DirectX::XMINT2{ 5, 5 };

	m_config.spreadMinMax				= DirectX::XMINT2{ -0, 0 };
	m_config.spawnSpread				= DirectX::XMINT2{ -0, 0 };

	m_cData.alphaMultipliers[0] = m_config.alphaMultipliers[0] = 1.0f;
	m_cData.alphaMultipliers[1] = m_config.alphaMultipliers[1] = 0.5f;
	m_cData.alphaMultipliers[2] = m_config.alphaMultipliers[2] = 0.01f;

	m_cData.fadePoints[0] = m_config.fadingPoints[0] = 1.0f;
	m_cData.fadePoints[1] = m_config.fadingPoints[1] = 0.99f;
	m_cData.fadePoints[2] = m_config.fadingPoints[2] = 0.25f;

	m_cData.colorModifiers[0] = 0.8f,
	m_cData.colorModifiers[1] = 0.9f;
	m_cData.colorModifiers[2] = 1.0f;

	m_config.textures[0] = L"SMOKE";
	m_config.textures[1] = L"SMOKE";
	m_config.textures[2] = L"SMOKE";

	InitializeBuffers();
}

void ParticleEmitter::SetConfiguration(PS::ParticleConfiguration & config)
{
	m_config = config;
}

void ParticleEmitter::SetPosition(DirectX::XMFLOAT4A origin)
{
	m_config.m_SpawnPosition = origin;
}

void ParticleEmitter::SetEmitterLife(float lifetime)
{
	m_config.m_EmitterLife = lifetime;
}

DirectX::XMVECTOR ParticleEmitter::RandomOffset(DirectX::XMVECTOR basePos, int offset)
{
	float x = DirectX::XMVectorGetX(basePos);
	float y = DirectX::XMVectorGetY(basePos);
	float z = DirectX::XMVectorGetZ(basePos);
	basePos = DirectX::XMVECTOR{ x,y,z };
	return basePos;
}

void ParticleEmitter::Queue()
{
	DX::g_emitters.push_back(this);
}

const DirectX::XMFLOAT4A & ParticleEmitter::getPosition() const
{
	return m_config.m_SpawnPosition; 
}

const DirectX::BoundingBox& ParticleEmitter::getBoundingBox() const
{
	return m_boundingBox;
}

DirectX::XMFLOAT4X4A ParticleEmitter::getWorldMatrix()
{
	using namespace DirectX;
	
	XMMATRIX matrix = XMMatrixTranspose(XMMatrixTranslation(m_config.m_SpawnPosition.x, m_config.m_SpawnPosition.y, m_config.m_SpawnPosition.z));
	XMStoreFloat4x4A(&m_worldMatrix, matrix);
	return m_worldMatrix;
}

float ParticleEmitter::RandomFloat(DirectX::XMINT2 min_max)
{
	float ret = 0;
	if (min_max.y != 0)
	{
		ret = rand() % (min_max.y * 10) + (min_max.x * 10);
		ret = ret * 0.1;
	}
	return ret;
}



