#pragma once
#include <d3d11.h>
#include <d3dcsx.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>
#include "Particle.h"
#include <math.h>

class Camera;

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
	float m_Rotation;
	float m_partialParticle;
	int m_Speed;
	bool m_EmitterActiv;
	DirectX::XMVECTOR m_SpeedDir;
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
	void _depthRenderTarget();


	DirectX::XMFLOAT4X4A m_worldMatrix;



public:
	ParticleEmitter();
	~ParticleEmitter();
	void Update(float timeDelata, Camera * camera);
	float RandomFloat();
	void InitializeBuffer();
	void SetBuffer();
	void Draw();
	DirectX::XMVECTOR RandomOffset(DirectX::XMVECTOR basePos, int offset);
	DirectX::XMFLOAT3 Float3scale(DirectX::XMFLOAT3 basePos, float scale);
	DirectX::XMFLOAT3 Float3add(DirectX::XMFLOAT3 basePos, DirectX::XMFLOAT3 basePos2);
	int nrOfEmittedParticles;
	std::vector<Vertex> vertex;
	int nrOfVertex;

	void Queue();

	void setPosition(const float & x, const float & y, const float & z, const float & w = 1.0f);

	DirectX::XMFLOAT4X4A getWorldMatrix();

};

