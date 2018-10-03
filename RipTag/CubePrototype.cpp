#include "CubePrototype.h"


CubePrototype::CubePrototype(float x, float y, float z)
{
	Network::Multiplayer * pMp = Network::Multiplayer::GetInstance();
	this->SetNetworkIDManager(pMp->pNetworkIDManager);
	this->SetNetworkID(this->GetNetworkID());

	this->s = new StaticMesh();
	this->s->LoadModel("../Assets/sphere.bin");

	this->m = Model(ObjectType::Static);
	this->m.SetVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	this->m.SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
	this->m.SetModel(s);
	this->m.setPosition(x, y, z);
	this->m.setScale(1, 1, 1);
}

CubePrototype::CubePrototype(RakNet::NetworkID nid, float x, float y, float z)
{
	Network::Multiplayer * pMp = Network::Multiplayer::GetInstance();
	this->SetNetworkIDManager(pMp->pNetworkIDManager);
	this->SetNetworkID(nid);

	this->s = new StaticMesh();
	this->s->LoadModel("../Assets/sphere.bin");

	this->m = Model(ObjectType::Static);
	this->m.SetVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	this->m.SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
	this->m.SetModel(s);
	this->m.setPosition(x, y, z);
	this->m.setScale(1, 1, 1);
}

CubePrototype::~CubePrototype()
{
	delete this->s;
}

void CubePrototype::setPosition(DirectX::XMFLOAT4A pos)
{
	this->m.setPosition(DirectX::XMFLOAT4A(pos.x, pos.y, pos.z + 2, pos.w));
	RakNet::NetworkID nId = this->GetNetworkID();
	
}

void CubePrototype::lerpPosition(DirectX::XMFLOAT4 pos, RakNet::Time time)
{
	//c - current, n - new, f - final
	DirectX::XMVECTOR cPos, nPos, fPos;

	DirectX::XMFLOAT4A currPos = m.getPosition();

	cPos = DirectX::XMLoadFloat4A(&currPos);
	nPos = DirectX::XMLoadFloat4A(&pos);

	fPos = DirectX::XMVectorLerp(cPos, nPos, time);

	DirectX::XMFLOAT4A finalPos;

	DirectX::XMStoreFloat4A(&finalPos, fPos);

	m.setPosition(finalPos);
}

void CubePrototype::Draw()
{
	this->m.Draw();
}