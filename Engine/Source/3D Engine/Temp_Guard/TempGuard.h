#pragma once
#include <DirectXMath.h>
#include "../Camera.h"
#include "../Extern.h"
struct FrustumVertex
{
	DirectX::XMFLOAT4A position;

	DirectX::XMFLOAT4A UV;
	DirectX::XMFLOAT4A Alpha;
};
const float aMin = 0.05f;
const float aMax = 0.5f;
struct NDCBOX
{
	// THE UV COORDS ARE MAPPED WRONG
	static std::vector<FrustumVertex> GetNDCBox()
	{
		float rawData[] =
		{
		-1, -1, 0,
		0, 0,
		aMax,

		-1, 1, 0,
		0, 1,
		aMax,
		
		1, -1, 0,
		1, 0,
		aMax,
		
		-1, 1, 0,
		0, 1,
		aMax,
		
		1, 1, 0,
		1, 1,
		aMax,
		
		1, -1, 0,
		1, 0,
		aMax,
		
		-1, 1, 0,
		0, 1,
		aMax,
		
		-1, 1, 1,
		0, 1,
		
		0,
		1, 1, 0,
		1, 1,
		aMax,
		
		-1, 1, 1,
		0, 1,
		aMin,
		
		1, 1, 1,
		1, 1,
		aMin,
		
		1, 1, 0,
		1, 1,
		aMax,
		
		-1, 1, 1,
		0, 1,
		aMin,
		
		-1, -1, 1,
		0, 0,
		aMin,
		
		1, 1, 1,
		1, 1,
		aMin,
		
		-1, -1, 1,
		0, 0,
		aMin,
		
		1, -1, 1,
		1, 0,
		aMin,
		
		1, 1, 1,
		1, 1,
		aMin,
		
		-1, -1, 1,
		0, 0,
		aMin,
		
		-1, -1, 0,
		0, 0,
		aMax,
		
		1, -1, 1,
		1, 0,
		aMin,
		
		-1, -1, 0,
		0, 0,
		aMax,
		
		1, -1, 0,
		1, 0,
		aMax,
		
		1, -1, 1,
		1, 0,
		aMin,
		
		1, -1, 0,
		1, 0,
		aMax,
		
		1, 1, 0,
		1, 1,
		aMax,
		
		1, -1, 1,
		1, 0,
		aMin,
		
		1, 1, 0,
		1, 1,
		aMax,
		
		1, 1, 1,
		1, 1,
		aMin,
		
		1, -1, 1,
		1, 0,
		aMin,
		
		-1, -1, 1,
		0, 0,
		aMin,
		
		-1, 1, 1,
		0, 1,
		aMin,
		
		-1, -1, 0,
		0, 0,
		aMax,
		
		-1, 1, 1,
		0, 1,
		aMin,
		
		-1, 1, 0,
		0, 1,
		aMax,
		
		-1, -1, 0,
		0, 0,
		aMax
		};
		std::vector<FrustumVertex> ndcBox;
		int i = 0;
		while (i < 216)
		{
			FrustumVertex fv;
			fv.position = DirectX::XMFLOAT4A(rawData[i], rawData[i + 1], rawData[i + 2], 1.0f);
			fv.UV = DirectX::XMFLOAT4A(rawData[i + 3], rawData[i + 4], 0,0);
			fv.Alpha = DirectX::XMFLOAT4A(rawData[i + 5], 0,0,0);
			ndcBox.push_back(fv);
			i += 6;
		}
		return ndcBox;
	}
};

class Guard
{
private:
	

	DirectX::XMFLOAT4A	m_pos, m_rot;
	Camera				m_cam;
	float				m_range;
	std::vector<FrustumVertex>	m_frustum;
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
	const std::vector<FrustumVertex> * getFrustum();
	void setPos(float x, float y, float z);
	DirectX::XMFLOAT4A getPos();
	void setDir(float x, float y, float z);
	DirectX::XMFLOAT4A getDir();
	void Rotate(float x, float y, float z);

	ID3D11Buffer * getVertexBuffer();

	UINT32 getSizeOfStruct();

	Camera & getCamera();
	const DirectX::XMFLOAT4X4A & getWorldMatrix();
	void Draw();

	void calcVisability();

	ID3D11UnorderedAccessView * getUAV();

private:
	void _createUAV();

};