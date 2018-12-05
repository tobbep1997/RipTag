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

		DirectX::XMINT2 directionMinMax			= { 0, 0 };
		DirectX::XMINT2 spreadMinMax				= { 0, 0 };
		DirectX::XMINT2 minMaxSpeed				= { 0, 0 };
		DirectX::XMINT2 minMax						= { 0, 0 };
		DirectX::XMINT2 minMaxLife					= { 0, 0 };
		DirectX::XMINT2 spawnSpread				= { 0, 0 };
		DirectX::XMINT2 m_RotationMinMax		= { 0, 0 };

		float m_EmitterLife				= 0;
		float m_Speed						= 0;

		int m_MaxParticle				= 1;
		int m_MinParticle					= 0;
		int m_RotationOffset			= 0;
		int m_SpawnOffset				= 0;
		int m_nrOfEmittParticles		= 0;
	};
}
struct Vertex
{
	DirectX::XMFLOAT4A pos;
	DirectX::XMFLOAT4A normal;
	DirectX::XMFLOAT4A tangent;
	DirectX::XMFLOAT2A uvPos;
};

class ParticleEmitter
{
public:
	ParticleEmitter(DirectX::XMFLOAT4A origin = { 0.0f, 0.0f, 0.0f, 1.0f }, PS::ParticleType type = PS::DEFAULT, float lifeTime = 1.0f);
	~ParticleEmitter();
	void Release();

	void Update(float timeDelata, Camera * camera);
	
	void Queue();
	void Draw();
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
private:
	PS::ParticleType type					= PS::DEFAULT;
	PS::ParticleConfiguration m_config = {};

	ID3D11Buffer* m_vertexBuffer	= nullptr; 
	ID3D11Buffer* m_cBuffer			= nullptr;

	std::vector<Particle*> m_Particles;
	std::vector<Vertex> vertex;

	DirectX::BoundingBox m_boundingBox = DirectX::BoundingBox();


	

	
	DirectX::XMFLOAT4X4A m_worldMatrix		= XMFLOAT4X4_IDENTITY;
	
	UINT32 m_StrideSize		= 0;
	UINT32 m_Offset				= 0;

	Vertex* m_VertexData		= nullptr;
	Particle * m_newParticle	= nullptr;

	float m_EmitterCurrentLife	= 0;
	float m_partialParticle			= 0;

	bool m_emitterActive			= false;
private:
	void InitializeBuffer();
	void SetBuffer();

	float RandomFloat(DirectX::XMINT2 min_max);
	DirectX::XMVECTOR RandomOffset(DirectX::XMVECTOR basePos, int offset);
	void _particleVertexCalculation(float timeDelata, Camera * camera);

	void _applyTextures();

};

