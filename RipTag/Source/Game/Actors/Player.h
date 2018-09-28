#pragma once
#include "Source/3D Engine/Components/Transform.h"

#include "Actor.h"
#include "Source/3D Engine/Model/Model.h"

class Player : public Actor
{
private:
	Camera * m_camera;

	float m_moveSpeed = 5.0f;
	float m_cameraSpeed = 1.0f;
public:
	Player();
	~Player();

	void BeginPlay();
	void Update(double deltaTime);


	Camera * getCamera();
private:

	void _handleInput(double deltaTime);

};
