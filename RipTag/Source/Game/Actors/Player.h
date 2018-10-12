#pragma once
//#include "../Engine/Source/3D Engine/Components/Base/Transform.h"

#include "Actor.h"

#include "../Engine/Source/3D Engine/Components/Base/CameraHolder.h"
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include "../../Physics/Wrapper/RayCastListener.h"

class Player : public Actor, public CameraHolder, public PhysicsComponent
{
private:
	RayCastListener *m_rayListener;

	bool isQPressed = false;
	float m_moveSpeed = 200.0f;
	float m_cameraSpeed = 1.0f;

	bool isPressed = false;
	bool isPressed2 = false;

public:
	Player();
	~Player();

	void BeginPlay();
	void Update(double deltaTime);

	void PhysicsUpdate(double deltaTime);

	void setPosition(const float& x, const float& y, const float& z, const float& w) override;

	void Phase();

private:

	void _handleInput(double deltaTime);

};
