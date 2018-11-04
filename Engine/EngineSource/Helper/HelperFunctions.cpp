#include "EnginePCH.h"
#include "HelperFunctions.h"

using namespace DirectX;
void AssertNotZero(float val)
{
	assert(val > 0.0 || val < 0.0);
}

void AssertNotZero(const DirectX::XMVECTOR & val)
{
	XMVECTOR res = XMVectorEqual(val, XMVectorZero());
	assert(!(bool)XMVectorGetX(res) &&
		!(bool)XMVectorGetY(res) &&
		!(bool)XMVectorGetZ(res) &&
		!(bool)XMVectorGetW(res));
}

void AssertNotZero(const DirectX::XMFLOAT4A& val)
{
	assert(val.x != 0.0f && val.y != 0.0f && val.z != 0.0f && val.w != 0.0f);
}

void AssertHasLength(const DirectX::XMVECTOR& val)
{
	assert(XMVectorGetX(XMVector3Length(val)) > 0.0f);
}

void AssertNotNAN(float val)
{
	auto s = std::to_string(val);
	auto it = s.find("nan");
	assert(it == std::string::npos);
}
