#pragma once
#include <d3d11.h>
#include <d3dcsx.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>
#include <time.h>

class Particle
{
public:
	Particle();
	~Particle();

	void Reset();

	DirectX::XMFLOAT3 position = DirectX::XMFLOAT3{0,0,0};
	DirectX::XMFLOAT3 velocity = DirectX::XMFLOAT3{ 0,0,0 };
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4{ 0.5f,0.5f,0.5f,0};
	float rotation = 1;
	float size = 1;
	float endSize = 1;
	float speed = 1;
	float age = 0;
	float lifeTime = 1;
	float acceleration = 0.1f;


};

