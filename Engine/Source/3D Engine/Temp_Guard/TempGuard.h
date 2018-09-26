#pragma once
#include <DirectXMath.h>
#include "../Camera.h"
#include "../Extern.h"
struct Frustum
{
	static const short int NR_OF_VERTICES = 36;
	DirectX::XMFLOAT4A p[NR_OF_VERTICES];
	
	//TODO:: CPU SHIT stefan saaaaa
	void setPoints()
	{
		float cubeArr[NR_OF_VERTICES * 3] = {
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
		for (int i = 0; i < NR_OF_VERTICES; i++)
		{
			p[i] = DirectX::XMFLOAT4A(cubeArr[counter], cubeArr[counter + 1], cubeArr[counter + 2], 1.0f);
			counter += 3;
		}
	}
};

struct FrustumVertex
{
	DirectX::XMFLOAT4A pos;
};


class Guard
{
private:
	

	DirectX::XMFLOAT4A	m_pos, m_rot;
	Camera				m_cam;
	float				m_range;
	Frustum				m_frustum;
	DirectX::XMFLOAT4X4A m_worldMatrix;
	ID3D11Buffer*		m_vertexBuffer;

	int m_vis = 0;


	ID3D11Texture2D* m_uavTextureBuffer;		//IsReleased
	ID3D11Texture2D* m_uavTextureBufferCPU;		//IsReleased
	//ID3D11Texture2D* m_uavKILLER;				//IsReleased
	ID3D11UnorderedAccessView* m_visabilityUAV;	//IsReleased
public:
	Guard();
	~Guard();
	const Frustum & getFrustum();
	void setPos(float x, float y, float z);
	DirectX::XMFLOAT4A getPos();
	void setDir(float x, float y, float z);
	DirectX::XMFLOAT4A getDir();
	void Rotate(float x, float y, float z);

	ID3D11Buffer * getVertexBuffer();

	UINT32 getSizeOfStruct();
	UINT getNrVertices();

	Camera & getCamera();
	const DirectX::XMFLOAT4X4A & getWorldMatrix();
	void Draw();

	void calcVisability();

	ID3D11UnorderedAccessView * getUAV();

private:
	void _createUAV();

};