#include "EnginePCH.h"
#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter()
{
	m_EmitterActiv = 1;
	m_EmitterCurrentLife = 0;
	m_EmitterLife = 1000;
	m_fakeUp;
	m_Rotation = 10;
	m_MaxParticle = 1000;
	m_MinParticle = 10;
	nrOfEmittedParticles = 2000;
	m_SpawnPosition = DirectX::XMVECTOR{0,0,0};


	InitializeBuffer();
	srand(time(0));
}

ParticleEmitter::~ParticleEmitter()
{
	for (auto& particle : m_Particles)
	{
		delete particle;
	}
	
	//m_ParticleSRV->Release();
	//m_renderTargetView->Release();

	//m_ParticleDepthStencilView->Release();
	m_vertexBuffer->Release();

}

void ParticleEmitter::Update(float timeDelata, Camera * camera)
{
	if (m_EmitterLife == 0)
		m_EmitterCurrentLife = 0;
	else if (m_EmitterCurrentLife > m_EmitterLife)
		m_EmitterActiv = false;
	else
		m_EmitterCurrentLife += timeDelata;

	if (m_EmitterActiv)
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
		DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(5, 5, 5);
		for (int i = 0; i < nrOfParticlesToEmit; i++)
		{
			if (m_Particles.size() >= m_MaxParticle)
				break;

			m_newParticle = new Particle();

			m_newParticle->position =  DirectX::XMVectorAdd(m_SpawnPosition, DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(0.f*i,-.510,-1.0*i,1)));//RandomOffset(m_SpawnPosition, m_SpawnOffset);
			m_newParticle->rotation = m_Rotation;// +(rand() % m_RotationOffset * 1 + (m_RotationOffset *-1));
			m_Particles.push_back(m_newParticle);
		}

		_particleVertexCalculation(camera);
	}
}

void ParticleEmitter::_particleVertexCalculation(Camera * camera)
{


	for (int i = 0; i < m_Particles.size(); i++)
	{


		DirectX::XMMATRIX rotationMatrix;

		float life = m_Particles[i]->age;

		if (life > m_Particles[i]->lifeTime)
		{
			delete m_Particles[i];
			m_Particles.erase(m_Particles.begin() + i);
			i -= 1;
			continue;
		}
		m_Particles[i]->age += timeDelata;

		//m_Particles[i]->velocity = DirectX::XMVectorScale(m_Particles[i]->velocity, (m_Particles[i]->speed * timeDelata));
		//m_Particles[i]->position = DirectX::XMVECTOR{ 4.297f, 5, -1.682f };;//DirectX::XMVectorMultiply(m_Particles[i]->position, m_Particles[i]->velocity);
		DirectX::XMVECTOR cameraPos = DirectX::XMLoadFloat4A(&camera->getPosition());
		m_toCam = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(m_Particles[i]->position, cameraPos));
		m_right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(m_toCam, m_fakeUp));
		m_up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(m_toCam, m_right));
		m_forward = m_toCam; //DirectX::XMVector3Normalize(DirectX::XMVector3Cross(m_up, m_right));
		rotationMatrix = DirectX::XMMatrixRotationAxis(m_forward, m_Particles[i]->rotation);
		
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

void ParticleEmitter::_depthRenderTarget()
{
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;

	depthStencilDesc.Flags = 0;
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;

	depthStencilDesc.Texture2DArray.FirstArraySlice = 0;
	depthStencilDesc.Texture2DArray.ArraySize = 0;
	depthStencilDesc.Texture2DArray.MipSlice = 0;

	DX::g_device->CreateDepthStencilView(m_resource, &depthStencilDesc, &m_ParticleDepthStencilView);
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
	hr = DX::g_device->CreateBuffer(&bufferDesc, &data, &m_vertexBuffer);

	
	//_depthRenderTarget();

	m_ParticleViewport.Width = 1280;
	m_ParticleViewport.Height = 720;
	m_ParticleViewport.MinDepth = 0.0f;
	m_ParticleViewport.MaxDepth = 1.0f;
	m_ParticleViewport.TopLeftX = 0;
	m_ParticleViewport.TopLeftY = 0;

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

	vertex.clear();
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
	//DX::g_deviceContext->RSSetViewports(1, &m_ParticleViewport);
	//DX::g_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_ParticleDepthStencilView);

	DX::g_deviceContext->Draw(nrOfVerts, 0);
}

DirectX::XMFLOAT3 ParticleEmitter::RandomOffset(DirectX::XMFLOAT3 basePos, int offset)
{
	basePos.x += rand() % offset * 1 + (offset *-1);
	basePos.y += rand() % offset * 1 + (offset *-1);
	basePos.z += rand() % offset * 1 + (offset *-1);

	return basePos;
}

DirectX::XMFLOAT3 ParticleEmitter::Float3scale(DirectX::XMFLOAT3 basePos, float scale)
{
	basePos.x *= scale;
	basePos.y *= scale;
	basePos.z *= scale;

	return basePos;
}

DirectX::XMFLOAT3 ParticleEmitter::Float3add(DirectX::XMFLOAT3 basePos, DirectX::XMFLOAT3 basePos2)
{
	basePos.x += basePos2.x;
	basePos.y += basePos2.y;
	basePos.z += basePos2.z;

	return basePos;
}

void ParticleEmitter::Queue()
{
	DX::g_emitters.push_back(this);
}

void ParticleEmitter::setPosition(const float & x, const float & y, const float & z, const float & w)
{
	m_SpawnPosition = DirectX::XMVECTOR{ x,y,z,w };
}

DirectX::XMFLOAT4X4A ParticleEmitter::getWorldMatrix()
{
	using namespace DirectX;
	XMFLOAT4 pos; XMStoreFloat4(&pos, m_SpawnPosition);
	
	XMMATRIX matrix = XMMatrixTranspose(XMMatrixTranslation(pos.x, pos.y, pos.z));
	XMStoreFloat4x4A(&m_worldMatrix, matrix);
	return m_worldMatrix;
}


