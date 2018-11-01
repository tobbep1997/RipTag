#pragma once
#include <DirectXMath.h>
#include "../Source/Physics/Bounce.h"
#include "../RipTag/Source/Physics/Wrapper/ContactListener.h"
#include "../RipTag/Source/Physics/Wrapper/RayCastListener.h"
#include "../RipTag/Source/Game/Actors/Enemy/Enemy.h"
namespace RipExtern
{
	extern b3World * g_world;
	extern ContactListener * m_contactListener;
	extern RayCastListener * m_rayListener;
}

class XMMATH
{
public:
	static DirectX::XMFLOAT4A add(const DirectX::XMFLOAT4A & a, const DirectX::XMFLOAT4A & b)
	{
		DirectX::XMFLOAT4A re;
		
		DirectX::XMVECTOR A, B, C;
		A = DirectX::XMLoadFloat4A(&a);
		B = DirectX::XMLoadFloat4A(&b);
		C = DirectX::XMVectorAdd(A, B);
		DirectX::XMStoreFloat4A(&re, C);

		return re;
	}
	static DirectX::XMFLOAT4A subtract(const DirectX::XMFLOAT4A & a, const DirectX::XMFLOAT4A & b)
	{
		DirectX::XMFLOAT4A re;

		DirectX::XMVECTOR A, B, C;
		A = DirectX::XMLoadFloat4A(&a);
		B = DirectX::XMLoadFloat4A(&b);
		C = DirectX::XMVectorSubtract(A, B);
		DirectX::XMStoreFloat4A(&re, C);

		return re;
	}
	static DirectX::XMFLOAT4A multiply(const DirectX::XMFLOAT4A & a, const DirectX::XMFLOAT4A & b)
	{
		DirectX::XMFLOAT4A re;

		DirectX::XMVECTOR A, B, C;
		A = DirectX::XMLoadFloat4A(&a);
		B = DirectX::XMLoadFloat4A(&b);
		C = DirectX::XMVectorMultiply(A, B);
		DirectX::XMStoreFloat4A(&re, C);

		return re;
	}
	static DirectX::XMFLOAT4A divide(const DirectX::XMFLOAT4A & a, const DirectX::XMFLOAT4A & b)
	{
		DirectX::XMFLOAT4A re;

		DirectX::XMVECTOR A, B, C;
		A = DirectX::XMLoadFloat4A(&a);
		B = DirectX::XMLoadFloat4A(&b);
		C = DirectX::XMVectorDivide(A, B);
		DirectX::XMStoreFloat4A(&re, C);

		return re;
	}
	static DirectX::XMFLOAT4A scale(const DirectX::XMFLOAT4A & a, float scale)
	{
		DirectX::XMFLOAT4A re;

		DirectX::XMVECTOR A, C;
		A = DirectX::XMLoadFloat4A(&a);
		C = DirectX::XMVectorScale(A, scale);
		DirectX::XMStoreFloat4A(&re, C);

		return re;
	}
};