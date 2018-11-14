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

	DirectX::XMVECTOR position = DirectX::XMVECTOR{0, 0, 0};
	DirectX::XMVECTOR velocity = DirectX::XMVECTOR{ 0,0.1f,0 };
	DirectX::XMFLOAT4A color = DirectX::XMFLOAT4A{ 0.5f,0.5f,0.5f,0};
	float rotation = 1;
	float size = 1;
	float endSize = 1;
	float speed = 10.0f;
	float age = 0;
	float lifeTime = 0.7f;
	float acceleration = 0.1f;


};

