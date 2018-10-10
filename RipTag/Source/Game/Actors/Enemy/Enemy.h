#pragma once
//#include "../Engine/Source/3D Engine/Components/Camera.h"
#include "../Actor.h"
//#include "../Engine/Source/3D Engine/Model/Model.h"
#include "../Engine/Source/3D Engine/Components/Base/CameraHolder.h"
#include "VisibilityComponent.h"

class Enemy : public Actor, public CameraHolder
{
private:
	VisibilityComponent m_vc;
public:
	Enemy();
	~Enemy();

	//TEMP
	void setDir(const float & x, const float & y, const float & z);
	Camera * getCamera();
	const int* getPlayerVisibility() const;

	// Inherited via Actor
	virtual void setPosition(const DirectX::XMFLOAT4A & pos) override;
	virtual void setPosition(const float & x, const float & y, const float & z, const float & w = 1.0f) override;
	virtual void BeginPlay() override;
	virtual void Update(double deltaTime) override;
	virtual void Draw() override;
};

