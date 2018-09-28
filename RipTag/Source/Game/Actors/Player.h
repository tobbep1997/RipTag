#pragma once
#include "Source/3D Engine/Components/Base/Transform.h"

#include "Actor.h"
#include "Source/3D Engine/Model/Model.h"
#include "Source/3D Engine/Components/Base/CameraHolder.h"

class Player : public Actor, public CameraHolder
{
private:
	

	float m_moveSpeed = 5.0f;
	float m_cameraSpeed = 1.0f;
public:
	Player();
	~Player();

	void BeginPlay();
	void Update(double deltaTime);

private:

	void _handleInput(double deltaTime);

};
