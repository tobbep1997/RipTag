#pragma once
#include <windows.h>
#include <DirectXMath.h>

struct StaticVertex
{
	DirectX::XMFLOAT4A pos;
	DirectX::XMFLOAT4A normal;
	DirectX::XMFLOAT4A tangent;
	DirectX::XMFLOAT4A uvPos;
};

struct DynamicVertex
{
	DirectX::XMFLOAT4A pos;
	DirectX::XMFLOAT4A normal;
	DirectX::XMFLOAT4A tangent;
	DirectX::XMFLOAT2A uvPos;
	DirectX::XMUINT4 influencingJoint;
	DirectX::XMFLOAT4A jointWeights;

};