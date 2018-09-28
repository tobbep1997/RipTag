#pragma once
#include "../Engine/Source/3D Engine/Camera.h"
#include "../Actor.h"
#include "../Engine/Source/3D Engine/Model/Model.h"
class Enemy : public Actor
{
private:
	Camera * m_camera;

public:
	Enemy();
	~Enemy();

	// Inherited via Actor
	virtual void BeginPlay() override;
	virtual void Update(double deltaTime) override;


private:
	void _createCamera();

};

