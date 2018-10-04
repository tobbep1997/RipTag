#pragma once
//#include "../Engine/Source/3D Engine/Components/Base/Transform.h"

#include "Actor.h"

#include "../Engine/Source/3D Engine/Components/Base/CameraHolder.h"
#include "../../Physics/Wrapper/PhysicsComponent.h"

class Player : public Actor, public CameraHolder, public PhysicsComponent
{
private:
	

	float m_moveSpeed = 100.0f;
	float m_cameraSpeed = 1.0f;

	bool isPressed = false;
public:
	Player();
	~Player();

	void BeginPlay();
	void Update(double deltaTime);

	void PhysicsUpdate(double deltaTime);

	void setPosition(const float& x, const float& y, const float& z, const float& w) override;

private:

	void _handleInput(double deltaTime);

};
