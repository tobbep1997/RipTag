#pragma once
#include <DirectXMath.h>
#include "../Engine/Source/3D Engine/Extern.h"
#include <vector>

class VisibilityComponent
{
public:
	struct FrustumVertex
	{
		DirectX::XMFLOAT4A position;

		DirectX::XMFLOAT4A UV;
		DirectX::XMFLOAT4A Alpha;
	};
	struct NDCBOX
	{
		// THE UV COORDS ARE MAPPED WRONG
		static std::vector<FrustumVertex> GetNDCBox()
		{
			const float aMin = 0.05f;
			const float aMax = 0.5f;
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
				fv.UV = DirectX::XMFLOAT4A(rawData[i + 3], rawData[i + 4], 0, 0);
				fv.Alpha = DirectX::XMFLOAT4A(rawData[i + 5], 0, 0, 0);
				ndcBox.push_back(fv);
				i += 6;
			}
			return ndcBox;
		}
	};
private:
	struct FrustumContent
	{
		ID3D11Buffer*				s_frustumBuffer = nullptr;
		std::vector<FrustumVertex>	s_frustumData;
	};
	struct DirectXContent
	{
		ID3D11Texture2D*			uavTextureBuffer = nullptr;		//IsReleased
		ID3D11Texture2D*			uavTextureBufferCPU = nullptr;	//IsReleased
		ID3D11UnorderedAccessView*	UAV = nullptr;					//IsReleased
	};

private:
	FrustumContent	m_frustum;
	DirectXContent	m_UAV;

	Camera * m_pCam;
	int m_playerVisability[2];

public:
	VisibilityComponent();
	~VisibilityComponent();
	void Init(Camera * cam);
	const std::vector<VisibilityComponent::FrustumVertex> * getFrustum();
	ID3D11Buffer * getFrustumBuffer();
	Camera * getCamera();

	const int* getVisibilityForPlayers() const;

	void QueueVisibility();
	void CalculateVisabilityFor(int playerIndex);
	UINT32 sizeOfFrustumVertex();

	ID3D11UnorderedAccessView * getUAV();

private:
	void _createUAV();

};