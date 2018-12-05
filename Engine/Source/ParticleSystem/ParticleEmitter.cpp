#include "EnginePCH.h"
#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter()
{
	isSmoke = false;
	emitterActiv = 1;
	m_EmitterCurrentLife = 0;
	m_EmitterLife = 0;
	m_RotationMinMax = DirectX::XMINT2{1, 360};
	m_MaxParticle = 100;
	m_MinParticle = 3;
	nrOfEmittedParticles = 100;
	m_Speed = 0.2f;
	m_SpawnPosition = DirectX::XMVECTOR{0,0,0};
	scaleOverTime = DirectX::XMFLOAT2{ 0.45f, 0.45f };
	scale = DirectX::XMFLOAT2(0.2f, 0.2f);
	spreadMinMax = DirectX::XMINT2{-2, 4 };
	directionMinMax = DirectX::XMINT2{ 4, 10 };
	minMaxLife = DirectX::XMINT2{ 0, 1 };
	spawnSpread = DirectX::XMINT2{ 0, 0 };

	_CreateBoundingBox();
	InitializeBuffer();
}

ParticleEmitter::ParticleEmitter(int type)
{
	isSmoke = false;
	emitterActiv = 1;
	m_EmitterCurrentLife = 0;
	m_EmitterLife = 0;
	m_RotationMinMax = DirectX::XMINT2{ 1, 360 };
	m_MaxParticle = 100;
	m_MinParticle = 3;
	nrOfEmittedParticles = 100;
	m_Speed = 0.2f;
	m_SpawnPosition = DirectX::XMVECTOR{ 0,0,0 };
	scaleOverTime = DirectX::XMFLOAT2{ 0.45f, 0.45f };
	scale = DirectX::XMFLOAT2(0.2f, 0.2f);
	spreadMinMax = DirectX::XMINT2{ -2, 4 };
	directionMinMax = DirectX::XMINT2{ 4, 10 };
	minMaxLife = DirectX::XMINT2{ 0, 1 };
	spawnSpread = DirectX::XMINT2{ 0, 0 };

	if (type == 1)
		setSmoke();

	_CreateBoundingBox();
	InitializeBuffer();
}

void ParticleEmitter::setSmoke()
{
	isSmoke = true;
	m_EmitterLife = 1;
	m_RotationMinMax = DirectX::XMINT2{ 1, 360 };
	m_MaxParticle = 100;
	m_MinParticle = 3;
	nrOfEmittedParticles = 100;
	m_Speed = 0.01f;
	m_SpawnPosition = DirectX::XMVECTOR{ 0,0,0 };
	scaleOverTime = DirectX::XMFLOAT2{ -0.5f, -0.5f };
	scale = DirectX::XMFLOAT2(0.2f, 0.2f);
	spreadMinMax = DirectX::XMINT2{ -10, 20 };
	directionMinMax = DirectX::XMINT2{ -10, 20 };
	minMaxLife = DirectX::XMINT2{ 2, 4 };
	spawnSpread = DirectX::XMINT2{ -1, 2 };


	_CreateBoundingBox();
	InitializeBuffer();
}

ParticleEmitter::~ParticleEmitter()
{
	DX::SafeRelease(m_vertexBuffer);
	DX::SafeRelease(m_cBuffer);
	for (auto& particle : m_Particles)
	{
		delete particle;
	}
	delete this->m_boundingBox;


}

void ParticleEmitter::Update(float timeDelata, Camera * camera)
{
	if (m_EmitterLife == 0)
		m_EmitterCurrentLife = 0;

	else if (m_EmitterCurrentLife > m_EmitterLife)
	{
		m_MaxParticle = 0;
		nrOfEmittedParticles -= 1;
		if(nrOfEmittedParticles <= 0)
			emitterActiv = false;
	}
	else
		m_EmitterCurrentLife += timeDelata;

	if (emitterActiv)
	{
		float emission = nrOfEmittedParticles * timeDelata;
		float partialEmisson = emission - (long)emission;

		int nrOfParticlesToEmit = emission;

		m_partialParticle += partialEmisson;

		if (m_partialParticle > 1.0f)
		{
			nrOfEmittedParticles += 1;
			m_partialParticle -= 1.0f;
		}

		int particleToLast = m_MinParticle - m_Particles.size();

		if (particleToLast > 0)
		{
			nrOfParticlesToEmit += particleToLast;
		}

		for (int i = 0; i < nrOfParticlesToEmit; i++)
		{
			if (m_Particles.size() >= m_MaxParticle)
				break;

			m_newParticle = new Particle();

			float tempX = DirectX::XMVectorGetX(m_SpawnPosition) + (RandomFloat(spawnSpread) * 0.5f);
			float tempY = DirectX::XMVectorGetY(m_SpawnPosition) + (RandomFloat(spawnSpread) * 0.5f);
			float tempZ = DirectX::XMVectorGetZ(m_SpawnPosition) + (RandomFloat(spawnSpread) * 0.5f);

			m_newParticle->position = DirectX::XMVECTOR{ tempX, tempY, tempZ };
			m_newParticle->velocity = DirectX::XMVECTOR{ RandomFloat(spreadMinMax), RandomFloat(directionMinMax), RandomFloat(spreadMinMax)};
			//if ((m_EmitterCurrentLife + 0.5) > m_EmitterLife)
			//	m_newParticle->velocity = DirectX::XMVECTOR{ RandomFloat(DirectX::XMINT2 {-2, 3}), RandomFloat(DirectX::XMINT2 {0, 15}), RandomFloat(DirectX::XMINT2 {1, 17}) };
			m_newParticle->speed = m_Speed;
			float temp = RandomFloat(DirectX::XMINT2 {0, 20});
			m_newParticle->color = DirectX::XMFLOAT4A(temp, temp, temp, 1); //(rand() % 2, rand() % 2, rand() % 2, rand() % 2);
			m_newParticle->scale = scale;
			m_newParticle->lifeTime = RandomFloat(minMaxLife);
			m_newParticle->rotation = RandomFloat(m_RotationMinMax);
			
			m_Particles.push_back(m_newParticle);
		}

		_particleVertexCalculation(timeDelata, camera);
	}
}

void ParticleEmitter::_particleVertexCalculation(float timeDelata, Camera * camera)
{

	for (int i = 0; i < m_Particles.size(); i++)
	{
		DirectX::XMVECTOR cameraPos = DirectX::XMLoadFloat4A(&camera->getPosition());
		m_toCam = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(m_Particles[i]->position, cameraPos));
		m_right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(m_toCam, m_fakeUp));
		m_up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(m_toCam, m_right));
		m_forward = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(m_up, m_right));
		m_SpeedDir = DirectX::XMVectorScale(m_Particles[i]->velocity, m_Particles[i]->speed * timeDelata);
		m_Particles[i]->position = DirectX::XMVectorAdd(m_Particles[i]->position, m_SpeedDir);
		
		
		rotationMatrix = DirectX::XMMatrixIdentity();
		rotationMatrix = DirectX::XMMatrixRotationAxis(m_forward, 43);//m_Particles[i]->rotation);
		//m_up = DirectX::XMVector3Transform(m_right, rotationMatrix);
		//m_right = DirectX::XMVector3Transform(m_up, rotationMatrix);
		//m_right = DirectX::XMVectorRotateLeft(m_up, 45);
		//m_up = DirectX::XMVectorRotateLeft(m_up, 45);


		m_up = DirectX::XMVectorScale(m_up, m_Particles[i]->scale.x);
		m_right = DirectX::XMVectorScale(m_right, m_Particles[i]->scale.y);


		m_Particles[i]->scale.x -= scaleOverTime.x * timeDelata;
		m_Particles[i]->scale.y -= scaleOverTime.y * timeDelata;


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

		upLeftVertex.tangent.x = m_Particles[i]->lifeTime - m_Particles[i]->age;
		upRightVertex.tangent.x = m_Particles[i]->lifeTime - m_Particles[i]->age;
		downLeftVertex.tangent.x = m_Particles[i]->lifeTime - m_Particles[i]->age;
		downRightVertex.tangent.x = m_Particles[i]->lifeTime - m_Particles[i]->age;

		upLeftVertex.tangent.y = isSmoke;
		upRightVertex.tangent.y = isSmoke;
		downLeftVertex.tangent.y = isSmoke;
		downRightVertex.tangent.y = isSmoke;
		
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

void ParticleEmitter::InitializeBuffer()
{
	nrOfVertex = m_MaxParticle * 6;
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
	if (SUCCEEDED(hr = DX::g_device->CreateBuffer(&bufferDesc, &data, &m_vertexBuffer)))
	{
		DX::SetName(m_vertexBuffer, "Particle: m_vertexBuffer");
	}

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	DX::g_shaderManager.VertexInputLayout(L"../Engine/Source/Shader/ParticleVertex.hlsl", "main", inputDesc, 4);
	

	DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/ParticleVertex.hlsl");
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/ParticlePixel.hlsl");

	delete[] initData;
}

void ParticleEmitter::SetBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	DX::g_deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

	m_VertexData = (Vertex*)mappedSubresource.pData;
	int particleSize = vertex.size();

	memcpy(m_VertexData, (void*)&vertex[0], sizeof(Vertex) *  vertex.size());

	DX::g_deviceContext->Unmap(m_vertexBuffer, 0);
	m_StrideSize = sizeof(Vertex);
	DX::g_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &m_StrideSize, &m_Offset);
}

void ParticleEmitter::Draw()
{
	int nrOfVerts = vertex.size();

	if (nrOfVerts == 0)
		return;

	SetBuffer();
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/ParticleVertex.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/Source/Shader/ParticleVertex.hlsl"), nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/Source/Shader/ParticlePixel.hlsl"), nullptr, 0);

	Manager::g_textureManager.getTexture("FIRE")->Bind(1);
	DX::g_deviceContext->Draw(nrOfVerts, 0);
}

void ParticleEmitter::Clear()
{
	vertex.clear();
}

DirectX::XMVECTOR ParticleEmitter::RandomOffset(DirectX::XMVECTOR basePos, int offset)
{
	float x = DirectX::XMVectorGetX(basePos);
	float y = DirectX::XMVectorGetY(basePos);
	float z = DirectX::XMVectorGetZ(basePos);
	basePos = DirectX::XMVECTOR{ x,y,z };
	return basePos;
}

void ParticleEmitter::releaseVertexBuffer()
{
	//m_vertexBuffer->Release();
	DX::SafeRelease(m_vertexBuffer);
}

void ParticleEmitter::Queue()
{
	DX::g_emitters.push_back(this);
}

void ParticleEmitter::setPosition(const float & x, const float & y, const float & z, const float & w)
{
	m_SpawnPosition = DirectX::XMVECTOR{ x,y,z,w };
	_CreateBoundingBox();
}

const DirectX::XMVECTOR & ParticleEmitter::getPosition() const
{
	return m_SpawnPosition; 
}

DirectX::XMFLOAT4X4A ParticleEmitter::getWorldMatrix()
{
	using namespace DirectX;
	XMFLOAT4 pos; XMStoreFloat4(&pos, m_SpawnPosition);
	
	XMMATRIX matrix = XMMatrixTranspose(XMMatrixTranslation(pos.x, pos.y, pos.z));
	XMStoreFloat4x4A(&m_worldMatrix, matrix);
	return m_worldMatrix;
}

void ParticleEmitter::setEmmiterLife(const float & lifeTime)
{
	m_EmitterLife = lifeTime;
}

const DirectX::BoundingBox* ParticleEmitter::getBoundingBox() const
{
	return this->m_boundingBox;
}

void ParticleEmitter::_CreateBoundingBox()
{
	using namespace DirectX;

	if (this->m_boundingBox)
		delete this->m_boundingBox;

	XMFLOAT3 center; XMStoreFloat3(&center, this->m_SpawnPosition);
	this->m_boundingBox = new BoundingBox(center, XMFLOAT3(5,5,5));
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



