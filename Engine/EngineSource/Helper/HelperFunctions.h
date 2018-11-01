#pragma once
#include <DirectXMath.h>

void AssertNotZero(float val);
void AssertNotZero(const DirectX::XMVECTOR& val);
void AssertNotZero(const DirectX::XMFLOAT4A& val);
void AssertHasLength(const DirectX::XMVECTOR& val);

void AssertNotNAN(float val);

