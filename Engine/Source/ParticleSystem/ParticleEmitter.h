#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Particle.h"

class Camera;

namespace DirectX
{
	struct BoundingBox;
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
private:
	int m_RotationOffset;
	int m_SpawnOffset;

	int m_MinParticle;
	int m_MaxParticle;
	
	float m_EmitterLife;
	float m_EmitterCurrentLife;
	DirectX::XMMATRIX rotationMatrix;
	
	DirectX::XMINT2 m_RotationMinMax;
	float m_Speed;
	float m_partialParticle;
	DirectX::XMVECTOR m_SpeedDir;
	DirectX::XMVECTOR m_Dir;
	DirectX::XMVECTOR m_SpawnPosition = DirectX::XMVECTOR{ 4.297f, 5, -1.682f, 1.0f/*Oof*/ };
	std::vector<Particle*> m_Particles;
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_cBuffer = nullptr;
	Vertex* m_VertexData;
	Particle * m_newParticle;
	UINT32 m_StrideSize;
	UINT32 m_Offset = 0;

	D3D11_VIEWPORT m_ParticleViewport;
	ID3D11ShaderResourceView* m_ParticleSRV = nullptr;
	ID3D11DepthStencilView*	m_ParticleDepthStencilView = nullptr;
	ID3D11RenderTargetView*	m_renderTargetView = nullptr;
	ID3D11Resource* m_resource = nullptr;

	DirectX::XMVECTOR m_up;
	DirectX::XMVECTOR m_right;
	DirectX::XMVECTOR m_toCam;
	DirectX::XMVECTOR m_forward;
	DirectX::XMVECTOR m_fakeUp = { 0.0f, 1.0f, 0.0f };

	void _particleVertexCalculation(float timeDelata, Camera * camera);

	DirectX::XMFLOAT4X4A m_worldMatrix;

	DirectX::BoundingBox * m_boundingBox = nullptr;


public:
	ParticleEmitter();
	ParticleEmitter(int type);
	~ParticleEmitter();
	void Update(float timeDelata, Camera * camera);
	float RandomFloat(DirectX::XMINT2 min_max);
	void InitializeBuffer();
	
	void SetBuffer();
	void Draw();
	void Clear();
	DirectX::XMVECTOR RandomOffset(DirectX::XMVECTOR basePos, int offset);
	int nrOfEmittedParticles;
	std::vector<Vertex> vertex;
	int nrOfVertex;
	void setSmoke();

	DirectX::XMINT2 directionMinMax;
	DirectX::XMINT2 spreadMinMax;
	DirectX::XMFLOAT2 scaleOverTime;
	DirectX::XMFLOAT2 scale;
	DirectX::XMINT2 minMaxSpeed;
	DirectX::XMINT2 minMax;
	DirectX::XMINT2 minMaxLife;
	DirectX::XMINT2 spawnSpread;
	bool isSmoke;
	int type;//FIRE = 0 SMOKE = 1

	void releaseVertexBuffer(); 

	bool emitterActiv;

	void Queue();

	void setPosition(const float & x, const float & y, const float & z, const float & w = 1.0f);
	const DirectX::XMVECTOR& getPosition() const; 

	DirectX::XMFLOAT4X4A getWorldMatrix();

	void setEmmiterLife(const float & lifeTime);

	const DirectX::BoundingBox * getBoundingBox() const;

private:
	void _CreateBoundingBox();
};

