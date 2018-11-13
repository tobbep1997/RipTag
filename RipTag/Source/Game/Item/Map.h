#pragma once

class Map
{
private:
	BaseActor * m_mesh;

	DirectX::XMFLOAT4A tempPos;
	DirectX::XMFLOAT4A tempRot;
public:
	Map();
	~Map();
	void Init();

	void Update(double deltaTime);

	void Draw();

};

