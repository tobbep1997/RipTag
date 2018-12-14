#include "EnginePCH.h"
#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
	_loadParticleShaders();
	_loadParticleTextures();
}


ParticleSystem::~ParticleSystem()
{
	clearEmitters();	
}

void ParticleSystem::Queue()
{
	for (size_t i = 0; i < m_ParticleEmitter.size(); i++)
	{
		m_ParticleEmitter[i]->Queue();
	}
}

void ParticleSystem::Update(float timeDelata, Camera * camera)
{
	for (size_t i = 0; i < m_ParticleEmitter.size(); i++)
	{
		m_ParticleEmitter[i]->Update(timeDelata, camera);
	}
	//Remove expired emitters
	for (size_t i = 0; i < m_ParticleEmitter.size(); i++)
	{
		if (m_ParticleEmitter[i]->Expired())
		{
			delete m_ParticleEmitter[i];
			m_ParticleEmitter[i] = nullptr;
		}
	}
	std::vector<ParticleEmitter*> temp;
	for (size_t i = 0; i < m_ParticleEmitter.size(); i++)
	{
		if (m_ParticleEmitter[i])
			temp.push_back(m_ParticleEmitter[i]);
	}

	m_ParticleEmitter.clear();
	m_ParticleEmitter = temp;
	temp.clear();
}

void ParticleSystem::AddEmitter(ParticleEmitter * pEmitter)
{
	this->m_ParticleEmitter.push_back(pEmitter);
}

void ParticleSystem::clearEmitters()
{
	for (size_t i = 0; i < m_ParticleEmitter.size(); i++)
	{
		if (m_ParticleEmitter[i])
		{
			m_ParticleEmitter[i]->Release();
			delete m_ParticleEmitter[i];
			m_ParticleEmitter[i] = nullptr;
		}
	}
	m_ParticleEmitter.clear();
}

void ParticleSystem::_loadParticleShaders()
{
	//D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//};
	//
	//DX::g_shaderManager.VertexInputLayout(L"../Engine/Source/Shader/ParticleVertex.hlsl", "main", inputDesc, 4);
	//DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/ParticleVertex.hlsl");
	//DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/ParticlePixel.hlsl");
}

void ParticleSystem::_loadParticleTextures()
{
	std::string path = "../Assets/PARTICLEFOLDER";
	for (auto & p : std::filesystem::directory_iterator(path))
	{
		if (p.is_regular_file())
		{
			auto file = p.path();
			if (file.has_filename() && file.has_extension())
			{
				std::wstring stem = file.stem().generic_wstring();
				std::wstring extension = file.extension().generic_wstring();
				if (extension == L".DDS")
					Manager::g_textureManager.loadDDSTexture(stem, file.generic_wstring(), L"_256");
			}
		}
	}
}
