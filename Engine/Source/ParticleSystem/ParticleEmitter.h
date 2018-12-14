#pragma once
#define XMFLOAT4X4_IDENTITY DirectX::XMFLOAT4X4A( 1.0f, 0.0f, 0.0f, 0.0f,     0.0f, 1.0f, 0.0f, 0.0f,    0.0f, 0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f, 1.0f)

#include <d3d11.h>
#include <DirectXMath.h>
#include "Particle.h"

class Camera;

namespace PS
{
	struct ParticleConfiguration
	{
		DirectX::XMFLOAT4A m_SpawnPosition = { 0.0f, 0.0f, 0.0f, 1.0f };

		DirectX::XMFLOAT2 scaleOverTime			= { 0.f, 0.f };
		DirectX::XMFLOAT2 scale						= { 0.f, 0.f };

		DirectX::XMFLOAT2 directionMinMax			= { 0, 0 };
		DirectX::XMFLOAT2 spreadMinMax				= { 0, 0 };
		DirectX::XMFLOAT2 minMaxSpeed				= { 0, 0 };
		DirectX::XMFLOAT2 minMax						= { 0, 0 };
		DirectX::XMFLOAT2 minMaxLife					= { 0, 0 };
		DirectX::XMFLOAT2 spawnSpread				= { 0, 0 };
		DirectX::XMFLOAT2 m_RotationMinMax		= { 0, 0 };

		float m_EmitterLife				= 0;
		float m_Speed						= 0;

		int m_MaxParticle				= 1;
		int m_MinParticle					= 0;
		int m_RotationOffset			= 0;
		int m_SpawnOffset				= 0;
		int m_nrOfEmittParticles		= 0;
		int m_nrOfCreatedParticles = 0;

		std::wstring textures[3]	= { L"NONE", L"NONE", L"NONE" };
		float fadingPoints[3]		= { 1.0f, 1.0f, 1.0f };
		float alphaMultipliers[3]	= { 1.0f, 1.0f, 1.0f };
	};

}
struct Vertex
{
	DirectX::XMFLOAT4A pos;
	DirectX::XMFLOAT4A normal;
	DirectX::XMFLOAT4A tangent;
	DirectX::XMFLOAT2A uvPos;
};

//__declspec(align(32))
struct ConstantBufferData
{
	float alphaMultipliers[4]	= { 1.0f, 1.0f, 1.0f, 0.0f };
	float fadePoints[4]			= { 1.0f, 1.0f, 1.0f, 0.0f };
	float colorModifiers[4]		= { 1.0f, 1.0f, 1.0f, 1.0f };
};

class Texture;

class ParticleEmitter
{
public:
	ParticleEmitter(DirectX::XMFLOAT4A origin = { 0.0f, 0.0f, 0.0f, 1.0f }, PS::ParticleType type = PS::DEFAULT, float lifeTime = 1.0f);
	~ParticleEmitter();
	void Release();

	void Update(float timeDelata, Camera * camera);
	
	void Queue();
	bool Draw();
	void Clear();
	
	//SETTERS
	void SetAsDefaultNone(DirectX::XMFLOAT4A origin);
	void SetAsDefaultFire(DirectX::XMFLOAT4A origin);
	void SetAsDefaultSmoke(DirectX::XMFLOAT4A origin);

	void SetConfiguration(PS::ParticleConfiguration &config);
	void SetPosition(DirectX::XMFLOAT4A origin);
	void SetEmitterLife(float lifetime);
	//GETTERS
	DirectX::XMFLOAT4X4A getWorldMatrix();
	const DirectX::XMFLOAT4A& getPosition() const;
	const DirectX::BoundingBox& getBoundingBox() const;
	bool Expired() { return m_expired; }
	void ApplyTextures();

	const PS::ParticleConfiguration & getConfig() const;

	UINT getBufferSize() const;
	UINT getVertexSize() const;
	UINT getSize() const;
	const std::vector<Vertex> & getVertex() const;

	const PS::ParticleType & getType() const;

private:
	PS::ParticleType type					= PS::DEFAULT;
	PS::ParticleConfiguration m_config = {};

	ID3D11Buffer* m_vertexBuffer	= nullptr; 
	ID3D11Buffer* m_cBuffer			= nullptr;
	ConstantBufferData m_cData;

	std::vector<Particle*> m_Particles;
	std::vector<Vertex> vertex;

	DirectX::BoundingBox m_boundingBox;

	DirectX::XMFLOAT4X4A m_worldMatrix		= XMFLOAT4X4_IDENTITY;

	Vertex* m_VertexData		= nullptr;
	Particle * m_newParticle	= nullptr;

	float m_EmitterCurrentLife	= 0;
	float m_partialParticle			= 0;

	bool m_emitterActive			= false;
	bool m_expired					= false;
private:
	void InitializeBuffers();
	void SetBuffers();

	float RandomFloat(DirectX::XMFLOAT2 min_max);
	DirectX::XMVECTOR RandomOffset(DirectX::XMVECTOR basePos, int offset);
	void _particleVertexCalculation(float timeDelata, Camera * camera);

	void _createSmokeParticles();
	void _createParticles(); //Uses Henriks lögic :]
};

