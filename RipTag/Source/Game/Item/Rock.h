#pragma once

class Rock
{
private:
	BaseActor * m_mesh;

	DirectX::XMFLOAT4A tempPos;
	DirectX::XMFLOAT4A tempRot;

	bool m_deleteRock = false;
public:
	Rock();
	~Rock();

	void Init();
	
	void Release();

	void Update(double deltaTime);

	void Draw();

	bool DeleteRock();
};

