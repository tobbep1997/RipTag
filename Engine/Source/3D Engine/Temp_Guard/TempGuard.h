#pragma once
#include <DirectXMath.h>
#include "../Camera.h"
#include "../Extern.h"
struct Frustum
{
	DirectX::XMFLOAT4A p[36];


	void setPoints()
	{
		float cubeArr[36 * 3] = {
		-1.0f, -1.0f, -1.0f, // triangle 1 : begin
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f, -1.0f, // triangle 2 : begin
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f, // triangle 2 : end
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
		};

		int counter = 0;
		for (int i = 0; i < 36; i++)
		{
			p[i] = DirectX::XMFLOAT4A(cubeArr[counter], cubeArr[counter + 1], cubeArr[counter + 2], 1.0f);
			counter += 3;
		}
	}
};


class Guard
{
private:
	

	DirectX::XMFLOAT4A	m_pos, m_rot;
	Camera				m_cam;
	float				m_range;
	Frustum				m_frustum;
	DirectX::XMFLOAT4X4A m_worldMatrix;


public:
	Guard();
	~Guard() {};
	const Frustum & getFrustum();
	void setPos(float x, float y, float z);
	DirectX::XMFLOAT4A getPos();
	void setDir(float x, float y, float z);
	DirectX::XMFLOAT4A getDir();
	void Rotate(float x, float y, float z);

	Camera & getCamera();
	const DirectX::XMFLOAT4X4A & getWorldMatrix();
	void Draw();
};